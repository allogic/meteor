#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <macros.h>
#include <fileutl.h>

#include <graphics/vulkan.h>

#include <platform/window.h>

#include <vulkan/vulkan.h>

#ifdef OS_WINDOWS
#	include <windows.h>
#	include <vulkan/vulkan_win32.h>
#endif

#ifdef OS_LINUX
#	include <wayland-client.h>
#	include <vulkan/vulkan_wayland.h>
#endif

struct xVulkan_t {
	VkInstance xInstance;
#ifdef DEBUG
	VkDebugUtilsMessengerEXT xDebugMessenger;
#endif
	VkSurfaceKHR xSurface;
	VkPhysicalDevice xPhysicalDevice;
	VkSurfaceFormatKHR xPreferedSurfaceFormat;
	VkPresentModeKHR xPreferedPresentMode;
	VkDevice xDevice;
	VkQueue xGraphicsQueue;
	VkQueue xPresentQueue;
	VkExtent2D xSwapChainExtent;
	VkSwapchainKHR xSwapChain;
	uint32_t nSwapChainImageCount;
	VkImage* pxSwapChainImages;
	VkImageView* pxSwapChainImageViews;
	VkRenderPass xRenderPass;
	VkShaderModule xVertModule;
	VkShaderModule xFragModule;
	VkPipelineLayout xPipelineLayout;
	VkPipeline xGraphicsPipeline;
	VkFramebuffer* pxFrameBuffers;
	VkCommandPool xCommandPool;
	VkCommandBuffer xCommandBuffer;
	VkSemaphore xImageAvailableSemaphore;
	VkSemaphore xRenderFinishedSemaphore;
	VkFence xInFlightFence;
};

static struct xVulkan_t s_xVulkan;

static const char* s_apLayerExtensions[] = {
	"VK_KHR_surface",
#ifdef DEBUG
	"VK_EXT_debug_utils",
#endif
#ifdef OS_WINDOWS
	"VK_KHR_win32_surface",
#endif
#ifdef OS_LINUX
	"VK_KHR_wayland_surface",
#endif
};

static const char* s_apDeviceExtensions[] = {
	"VK_KHR_swapchain",
};

#ifdef DEBUG
static const char* s_apValidationLayers[] = {
	"VK_LAYER_KHRONOS_validation",
};

static VkResult CreateDebugUtilsMessengerEXT(VkInstance xInstance, const VkDebugUtilsMessengerCreateInfoEXT* pxCreateInfo, const VkAllocationCallbacks* pxAllocator, VkDebugUtilsMessengerEXT* pxDebugMessenger) {
	PFN_vkCreateDebugUtilsMessengerEXT pVkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(xInstance, "vkCreateDebugUtilsMessengerEXT");

	if (pVkCreateDebugUtilsMessengerEXT) {
		return pVkCreateDebugUtilsMessengerEXT(xInstance, pxCreateInfo, pxAllocator, pxDebugMessenger);
	}

	return VK_ERROR_EXTENSION_NOT_PRESENT;
}

static void DestroyDebugUtilsMessengerEXT(VkInstance xInstance, VkDebugUtilsMessengerEXT xDebugMessenger, const VkAllocationCallbacks* pxAllocator) {
	PFN_vkDestroyDebugUtilsMessengerEXT pVkDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(xInstance, "vkDestroyDebugUtilsMessengerEXT");

	if (pVkDestroyDebugUtilsMessengerEXT) {
		pVkDestroyDebugUtilsMessengerEXT(xInstance, xDebugMessenger, pxAllocator);
	}
}

static VkBool32 DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT xMessageSeverity, VkDebugUtilsMessageTypeFlagsEXT xMessageType, const VkDebugUtilsMessengerCallbackDataEXT* pxCallbackData, void* pUserData) {
	UNUSED(xMessageSeverity);
	UNUSED(xMessageType);
	UNUSED(pUserData);

	printf("%s\n", pxCallbackData->pMessage);

	return VK_FALSE;
}
#endif

static void Vulkan_Cleanup(void) {
	if ((s_xVulkan.xDevice != VK_NULL_HANDLE) && (s_xVulkan.xInFlightFence != VK_NULL_HANDLE)) {
		vkDestroyFence(s_xVulkan.xDevice, s_xVulkan.xInFlightFence, 0);
	}

	if ((s_xVulkan.xDevice != VK_NULL_HANDLE) && (s_xVulkan.xRenderFinishedSemaphore != VK_NULL_HANDLE)) {
		vkDestroySemaphore(s_xVulkan.xDevice, s_xVulkan.xRenderFinishedSemaphore, 0);
	}

	if ((s_xVulkan.xDevice != VK_NULL_HANDLE) && (s_xVulkan.xImageAvailableSemaphore != VK_NULL_HANDLE)) {
		vkDestroySemaphore(s_xVulkan.xDevice, s_xVulkan.xImageAvailableSemaphore, 0);
	}

	if ((s_xVulkan.xDevice != VK_NULL_HANDLE) && (s_xVulkan.xCommandPool != VK_NULL_HANDLE)) {
		vkDestroyCommandPool(s_xVulkan.xDevice, s_xVulkan.xCommandPool, 0);
	}

	if ((s_xVulkan.xDevice != VK_NULL_HANDLE) && s_xVulkan.pxFrameBuffers && s_xVulkan.nSwapChainImageCount) {
		for (uint32_t i = 0; i < s_xVulkan.nSwapChainImageCount; ++i) {
			vkDestroyFramebuffer(s_xVulkan.xDevice, s_xVulkan.pxFrameBuffers[i], 0);
		}

		free(s_xVulkan.pxFrameBuffers);
	}

	if ((s_xVulkan.xDevice != VK_NULL_HANDLE) && (s_xVulkan.xGraphicsPipeline != VK_NULL_HANDLE)) {
		vkDestroyPipeline(s_xVulkan.xDevice, s_xVulkan.xGraphicsPipeline, 0);
	}

	if ((s_xVulkan.xDevice != VK_NULL_HANDLE) && (s_xVulkan.xPipelineLayout != VK_NULL_HANDLE)) {
		vkDestroyPipelineLayout(s_xVulkan.xDevice, s_xVulkan.xPipelineLayout, 0);
	}

	if ((s_xVulkan.xDevice != VK_NULL_HANDLE) && (s_xVulkan.xFragModule != VK_NULL_HANDLE)) {
		vkDestroyShaderModule(s_xVulkan.xDevice, s_xVulkan.xFragModule, 0);
	}

	if ((s_xVulkan.xDevice != VK_NULL_HANDLE) && (s_xVulkan.xVertModule != VK_NULL_HANDLE)) {
		vkDestroyShaderModule(s_xVulkan.xDevice, s_xVulkan.xVertModule, 0);
	}

	if ((s_xVulkan.xDevice != VK_NULL_HANDLE) && (s_xVulkan.xRenderPass != VK_NULL_HANDLE)) {
		vkDestroyRenderPass(s_xVulkan.xDevice, s_xVulkan.xRenderPass, 0);
	}

	if ((s_xVulkan.xDevice != VK_NULL_HANDLE) && s_xVulkan.pxSwapChainImageViews && s_xVulkan.nSwapChainImageCount) {
		for (uint32_t i = 0; i < s_xVulkan.nSwapChainImageCount; ++i) {
			vkDestroyImageView(s_xVulkan.xDevice, s_xVulkan.pxSwapChainImageViews[i], 0);
		}

		free(s_xVulkan.pxSwapChainImageViews);
	}

	if (s_xVulkan.pxSwapChainImages) {
		free(s_xVulkan.pxSwapChainImages);
	}

	if ((s_xVulkan.xDevice != VK_NULL_HANDLE) && (s_xVulkan.xSwapChain != VK_NULL_HANDLE)) {
		vkDestroySwapchainKHR(s_xVulkan.xDevice, s_xVulkan.xSwapChain, 0);
	}

	if (s_xVulkan.xDevice != VK_NULL_HANDLE) {
		vkDestroyDevice(s_xVulkan.xDevice, 0);
	}

	if ((s_xVulkan.xInstance != VK_NULL_HANDLE) && (s_xVulkan.xSurface != VK_NULL_HANDLE)) {
		vkDestroySurfaceKHR(s_xVulkan.xInstance, s_xVulkan.xSurface, 0);
	}

#ifdef DEBUG
	if ((s_xVulkan.xInstance != VK_NULL_HANDLE) && (s_xVulkan.xDebugMessenger != VK_NULL_HANDLE)) {
		DestroyDebugUtilsMessengerEXT(s_xVulkan.xInstance, s_xVulkan.xDebugMessenger, 0);
	}
#endif

	if (s_xVulkan.xInstance != VK_NULL_HANDLE) {
		vkDestroyInstance(s_xVulkan.xInstance, 0);
	}
}

static bool Vulkan_CreateInstance(void) {
	VkApplicationInfo xAppInfo;
	memset(&xAppInfo, 0, sizeof(xAppInfo));
	xAppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	xAppInfo.pApplicationName = APPLICATION_NAME;
	xAppInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	xAppInfo.pEngineName = ENGINE_NAME;
	xAppInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	xAppInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo xInstanceCreateInfo;
	memset(&xInstanceCreateInfo, 0, sizeof(xInstanceCreateInfo));
	xInstanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	xInstanceCreateInfo.pApplicationInfo = &xAppInfo;
	xInstanceCreateInfo.enabledExtensionCount = ARRAY_LENGTH(s_apLayerExtensions);
	xInstanceCreateInfo.ppEnabledExtensionNames = s_apLayerExtensions;

#ifdef DEBUG
	VkDebugUtilsMessengerCreateInfoEXT xDebugCreateInfo;
	memset(&xDebugCreateInfo, 0, sizeof(xDebugCreateInfo));
	xDebugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	xDebugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	xDebugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	xDebugCreateInfo.pfnUserCallback = DebugCallback;

	xInstanceCreateInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&xDebugCreateInfo;
	xInstanceCreateInfo.enabledLayerCount = ARRAY_LENGTH(s_apValidationLayers);
	xInstanceCreateInfo.ppEnabledLayerNames = s_apValidationLayers;
#endif

	if (vkCreateInstance(&xInstanceCreateInfo, 0, &s_xVulkan.xInstance) != VK_SUCCESS) {
		printf("Failed creating vulkan instance\n");
		return false;
	}

#ifdef DEBUG
	if (CreateDebugUtilsMessengerEXT(s_xVulkan.xInstance, &xDebugCreateInfo, 0, &s_xVulkan.xDebugMessenger) != VK_SUCCESS) {
		printf("Failed setting up vulkan debug messenger\n");
		return false;
	}
#endif

	return true;
}

static bool Vulkan_CreateSurface(struct xWindow_t* pxWindow) {
#ifdef OS_WINDOWS
	VkWin32SurfaceCreateInfoKHR xSurfaceCreateInfo;
	memset(&xSurfaceCreateInfo, 0, sizeof(xSurfaceCreateInfo));
	xSurfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	xSurfaceCreateInfo.hwnd = Window_GetWindowHandle(pxWindow);
	xSurfaceCreateInfo.hinstance = Window_GetModuleHandle(pxWindow);

	if (vkCreateWin32SurfaceKHR(s_xVulkan.xInstance, &xSurfaceCreateInfo, 0, &s_xVulkan.xSurface) != VK_SUCCESS) {
		printf("Failed creating win32 window surface\n");
		return false;
	}
#endif

#ifdef OS_LINUX
	VkWaylandSurfaceCreateInfoKHR xSurfaceCreateInfo;
	memset(&xSurfaceCreateInfo, 0, sizeof(xSurfaceCreateInfo));
	xSurfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR;
	xSurfaceCreateInfo.display = Window_GetDisplayHandle(pxWindow);
	xSurfaceCreateInfo.surface = Window_GetSurfaceHandle(pxWindow);

	if (vkCreateWaylandSurfaceKHR(s_xVulkan.xInstance, &xSurfaceCreateInfo, 0, &s_xVulkan.xSurface) != VK_SUCCESS) {
		printf("Failed creating wayland window surface\n");
		return false;
	}
#endif

	return true;
}

static bool Vulkan_FindPhysicalDevice(void) {
	uint32_t nPhysicalDeviceCount;
	vkEnumeratePhysicalDevices(s_xVulkan.xInstance, &nPhysicalDeviceCount, 0);
	if (nPhysicalDeviceCount == 0) {
		printf("Failed finding physical devices\n");
		return false;
	}

	VkPhysicalDevice* pxPhysicalDevices = (VkPhysicalDevice*)malloc(sizeof(VkPhysicalDevice) * nPhysicalDeviceCount);
	vkEnumeratePhysicalDevices(s_xVulkan.xInstance, &nPhysicalDeviceCount, pxPhysicalDevices);

	for (uint32_t i = 0; i < nPhysicalDeviceCount; ++i) {
		VkPhysicalDeviceProperties xPhysicalDeviceProperties;
		VkPhysicalDeviceFeatures xPhysicalDeviceFeatures;

		vkGetPhysicalDeviceProperties(pxPhysicalDevices[i], &xPhysicalDeviceProperties);
		vkGetPhysicalDeviceFeatures(pxPhysicalDevices[i], &xPhysicalDeviceFeatures);

		if (xPhysicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
			if (xPhysicalDeviceFeatures.geometryShader) {
				memcpy(&s_xVulkan.xPhysicalDevice, &pxPhysicalDevices[i], sizeof(s_xVulkan.xPhysicalDevice));
				break;
			}
		}
	}

	free(pxPhysicalDevices);

	if (s_xVulkan.xPhysicalDevice == 0) {
		printf("Failed finding physical device\n");
		return false;
	}

	return true;
}

static bool Vulkan_FindQueueFamilies(int32_t* pnGraphicsQueueIndex, int32_t* pnPresentQueueIndex) {
	uint32_t nQueueFamilyCount;
	vkGetPhysicalDeviceQueueFamilyProperties(s_xVulkan.xPhysicalDevice, &nQueueFamilyCount, 0);

	if (nQueueFamilyCount == 0) {
		printf("Failed finding queue family properties\n");
		return false;
	}

	VkQueueFamilyProperties* pxQueueFamilyProperties = (VkQueueFamilyProperties*)malloc(sizeof(VkQueueFamilyProperties) * nQueueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(s_xVulkan.xPhysicalDevice, &nQueueFamilyCount, pxQueueFamilyProperties);

	for (uint32_t i = 0; i < nQueueFamilyCount; ++i) {
		uint32_t nGraphicsSupport = 0;
		uint32_t nPresentSupport = 0;

		nGraphicsSupport = pxQueueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT;

		vkGetPhysicalDeviceSurfaceSupportKHR(s_xVulkan.xPhysicalDevice, i, s_xVulkan.xSurface, &nPresentSupport);

		if (nGraphicsSupport && (*pnGraphicsQueueIndex == -1)) {
			*pnGraphicsQueueIndex = i;
		} else if (nPresentSupport && (*pnPresentQueueIndex == -1)) {
			*pnPresentQueueIndex = i;
		}

		if ((*pnGraphicsQueueIndex != -1) && (*pnPresentQueueIndex != -1)) {
			break;
		}
	}

	free(pxQueueFamilyProperties);

	if (*pnGraphicsQueueIndex == -1) {
		printf("Failed to find graphics family\n");
		return false;
	}

	if (*pnPresentQueueIndex == -1) {
		printf("Failed to find present support\n");
		return false;
	}

	return true;
}

static bool Vulkan_CheckPhysicalDeviceExtensions(void) {
	uint32_t nAvailableDeviceExtensionCount;
	vkEnumerateDeviceExtensionProperties(s_xVulkan.xPhysicalDevice, 0, &nAvailableDeviceExtensionCount, 0);

	VkExtensionProperties* pxAvailableDeviceExtensions = (VkExtensionProperties*)malloc(sizeof(VkExtensionProperties) * nAvailableDeviceExtensionCount);
	vkEnumerateDeviceExtensionProperties(s_xVulkan.xPhysicalDevice, 0, &nAvailableDeviceExtensionCount, pxAvailableDeviceExtensions);

	bool nRequiredDeviceExtensionsAvailable = true;

	for (uint32_t i = 0; i < ARRAY_LENGTH(s_apDeviceExtensions); ++i) {
		bool nDeviceExtensionsAvailable = false;

		for (uint32_t j = 0; j < nAvailableDeviceExtensionCount; ++j) {
			if (strcmp(s_apDeviceExtensions[i], pxAvailableDeviceExtensions[j].extensionName) == 0) {
#ifdef DEBUG
				printf("Found %s\n", s_apDeviceExtensions[i]);
#endif
				nDeviceExtensionsAvailable = true;
				break;
			}
		}

		if (!nDeviceExtensionsAvailable) {
#ifdef DEBUG
			printf("Missing %s\n", s_apDeviceExtensions[i]);
#endif

			nRequiredDeviceExtensionsAvailable = false;
			break;
		}
	}

	free(pxAvailableDeviceExtensions);

	if (!nRequiredDeviceExtensionsAvailable) {
		printf("Failed to find required device extensions\n");
		return false;
	}

	return true;
}

static bool Vulkan_CreateLogicalDevice(int32_t nGraphicsQueueIndex, int32_t nPresentQueueIndex) {
#ifdef DEBUG
	printf("GraphicsQueueIndex %d\n", nGraphicsQueueIndex);
	printf("PresentQueueIndex %d\n", nPresentQueueIndex);
#endif

	float fQueuePriority = 1.0f;

	VkDeviceQueueCreateInfo xQueueCreateInfos[2];
	memset(&xQueueCreateInfos, 0, sizeof(xQueueCreateInfos));

	xQueueCreateInfos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	xQueueCreateInfos[0].queueFamilyIndex = nGraphicsQueueIndex;
	xQueueCreateInfos[0].queueCount = 1;
	xQueueCreateInfos[0].pQueuePriorities = &fQueuePriority;

	xQueueCreateInfos[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	xQueueCreateInfos[1].queueFamilyIndex = nPresentQueueIndex;
	xQueueCreateInfos[1].queueCount = 1;
	xQueueCreateInfos[1].pQueuePriorities = &fQueuePriority;

	VkPhysicalDeviceFeatures xPhysicalDeviceFeatures;
	memset(&xPhysicalDeviceFeatures, 0, sizeof(xPhysicalDeviceFeatures));

	VkDeviceCreateInfo xDeviceCreateInfo;
	memset(&xDeviceCreateInfo, 0, sizeof(xDeviceCreateInfo));
	xDeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	xDeviceCreateInfo.pQueueCreateInfos = xQueueCreateInfos;
	xDeviceCreateInfo.queueCreateInfoCount = ARRAY_LENGTH(xQueueCreateInfos);
	xDeviceCreateInfo.pEnabledFeatures = &xPhysicalDeviceFeatures;
	xDeviceCreateInfo.enabledExtensionCount = ARRAY_LENGTH(s_apDeviceExtensions);
	xDeviceCreateInfo.ppEnabledExtensionNames = s_apDeviceExtensions;
#ifdef DEBUG
	xDeviceCreateInfo.enabledLayerCount = ARRAY_LENGTH(s_apValidationLayers);
	xDeviceCreateInfo.ppEnabledLayerNames = s_apValidationLayers;
#endif

	if (vkCreateDevice(s_xVulkan.xPhysicalDevice, &xDeviceCreateInfo, 0, &s_xVulkan.xDevice) != VK_SUCCESS) {
		printf("Failed to create logical device");
		return false;
	}

	vkGetDeviceQueue(s_xVulkan.xDevice, nGraphicsQueueIndex, 0, &s_xVulkan.xGraphicsQueue);
	vkGetDeviceQueue(s_xVulkan.xDevice, nPresentQueueIndex, 0, &s_xVulkan.xPresentQueue);

	return true;
}

static bool Vulkan_CheckSwapChainCapabilities(void) {
	uint32_t nSurfaceFormatCount;
	uint32_t nPresentModeCount;

	vkGetPhysicalDeviceSurfaceFormatsKHR(s_xVulkan.xPhysicalDevice, s_xVulkan.xSurface, &nSurfaceFormatCount, 0);
	vkGetPhysicalDeviceSurfacePresentModesKHR(s_xVulkan.xPhysicalDevice, s_xVulkan.xSurface, &nPresentModeCount, 0);

	VkSurfaceFormatKHR* pxSurfaceFormats = (VkSurfaceFormatKHR*)malloc(sizeof(VkSurfaceFormatKHR) * nSurfaceFormatCount);
	VkPresentModeKHR* pxPresentModes = (VkPresentModeKHR*)malloc(sizeof(VkPresentModeKHR) * nPresentModeCount);

	vkGetPhysicalDeviceSurfaceFormatsKHR(s_xVulkan.xPhysicalDevice, s_xVulkan.xSurface, &nSurfaceFormatCount, pxSurfaceFormats);
	vkGetPhysicalDeviceSurfacePresentModesKHR(s_xVulkan.xPhysicalDevice, s_xVulkan.xSurface, &nPresentModeCount, pxPresentModes);

	bool bPreferedSurfaceFormatAvailable;
	bool bPreferedPresentModeAvailable;

	for (uint32_t i = 0; i < nSurfaceFormatCount; ++i) {
		if ((pxSurfaceFormats[i].format == VK_FORMAT_B8G8R8A8_SRGB) && (pxSurfaceFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)) {
			memcpy(&s_xVulkan.xPreferedSurfaceFormat, &pxSurfaceFormats[i], sizeof(s_xVulkan.xPreferedSurfaceFormat));
			bPreferedSurfaceFormatAvailable = true;
		}
	}

	for (uint32_t i = 0; i < nPresentModeCount; ++i) {
		if (pxPresentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
			memcpy(&s_xVulkan.xPreferedPresentMode, &pxPresentModes[i], sizeof(s_xVulkan.xPreferedPresentMode));
			bPreferedPresentModeAvailable = true;
		}
	}

	free(pxSurfaceFormats);
	free(pxPresentModes);

	if (!bPreferedSurfaceFormatAvailable) {
		printf("Failed finding prefered surface format\n");
		return false;
	}

	if (!bPreferedPresentModeAvailable) {
		printf("Failed finding prefered present mode\n");
		return false;
	}

	return true;
}

static bool Vulkan_CreateSwapChain(struct xWindow_t* pxWindow, int32_t nGraphicsQueueIndex, int32_t nPresentQueueIndex) {
	s_xVulkan.xSwapChainExtent.width = Window_GetWidth(pxWindow);
	s_xVulkan.xSwapChainExtent.height = Window_GetHeight(pxWindow);

	VkSurfaceCapabilitiesKHR xSurfaceCapabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(s_xVulkan.xPhysicalDevice, s_xVulkan.xSurface, &xSurfaceCapabilities);

	uint32_t nMinImageCount = xSurfaceCapabilities.minImageCount + 1;

	VkSwapchainCreateInfoKHR xSwapChaincreateInfo;
	memset(&xSwapChaincreateInfo, 0, sizeof(xSwapChaincreateInfo));
	xSwapChaincreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	xSwapChaincreateInfo.surface = s_xVulkan.xSurface;
	xSwapChaincreateInfo.minImageCount = nMinImageCount;
	xSwapChaincreateInfo.imageFormat = s_xVulkan.xPreferedSurfaceFormat.format;
	xSwapChaincreateInfo.imageColorSpace = s_xVulkan.xPreferedSurfaceFormat.colorSpace;
	xSwapChaincreateInfo.imageExtent = s_xVulkan.xSwapChainExtent;
	xSwapChaincreateInfo.imageArrayLayers = 1;
	xSwapChaincreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	xSwapChaincreateInfo.preTransform = xSurfaceCapabilities.currentTransform;
	xSwapChaincreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	xSwapChaincreateInfo.presentMode = s_xVulkan.xPreferedPresentMode;
	xSwapChaincreateInfo.clipped = VK_TRUE;
	xSwapChaincreateInfo.oldSwapchain = VK_NULL_HANDLE;

	uint32_t anQueueFamilies[] = { nGraphicsQueueIndex, nPresentQueueIndex };

	if (nGraphicsQueueIndex == nPresentQueueIndex) {
		xSwapChaincreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		xSwapChaincreateInfo.queueFamilyIndexCount = 0;
		xSwapChaincreateInfo.pQueueFamilyIndices = 0;
	} else {
		xSwapChaincreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		xSwapChaincreateInfo.queueFamilyIndexCount = ARRAY_LENGTH(anQueueFamilies);
		xSwapChaincreateInfo.pQueueFamilyIndices = anQueueFamilies;
	}

	if (vkCreateSwapchainKHR(s_xVulkan.xDevice, &xSwapChaincreateInfo, 0, &s_xVulkan.xSwapChain) != VK_SUCCESS) {
		printf("Failed creating swap chain\n");
		return false;
	}

	vkGetSwapchainImagesKHR(s_xVulkan.xDevice, s_xVulkan.xSwapChain, &s_xVulkan.nSwapChainImageCount, 0);

	if (s_xVulkan.nSwapChainImageCount == 0) {
		printf("Failed retrieving swap chain images");
		return false;
	}

	s_xVulkan.pxSwapChainImages = (VkImage*)malloc(sizeof(VkImage) * s_xVulkan.nSwapChainImageCount);
	vkGetSwapchainImagesKHR(s_xVulkan.xDevice, s_xVulkan.xSwapChain, &s_xVulkan.nSwapChainImageCount, s_xVulkan.pxSwapChainImages);

	return true;
}

static bool Vulkan_CreateImageViews(void) {
	s_xVulkan.pxSwapChainImageViews = (VkImageView*)malloc(sizeof(VkImageView) * s_xVulkan.nSwapChainImageCount);

	VkImageViewCreateInfo xImageViewCreateInfo;

	for (uint32_t i = 0; i < s_xVulkan.nSwapChainImageCount; ++i) {
		memset(&xImageViewCreateInfo, 0, sizeof(xImageViewCreateInfo));
		xImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		xImageViewCreateInfo.image = s_xVulkan.pxSwapChainImages[i];
		xImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		xImageViewCreateInfo.format = s_xVulkan.xPreferedSurfaceFormat.format;
		xImageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		xImageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		xImageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		xImageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		xImageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		xImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
		xImageViewCreateInfo.subresourceRange.levelCount = 1;
		xImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		xImageViewCreateInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(s_xVulkan.xDevice, &xImageViewCreateInfo, 0, &s_xVulkan.pxSwapChainImageViews[i]) != VK_SUCCESS) {
			printf("Failed creating image view\n");
			return false;
		}
	}

	return true;
}

static bool Vulkan_CreateRenderPass(void) {
	VkAttachmentDescription xColorAttachmentDesc;
	memset(&xColorAttachmentDesc, 0, sizeof(xColorAttachmentDesc));
	xColorAttachmentDesc.format = s_xVulkan.xPreferedSurfaceFormat.format;
	xColorAttachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
	xColorAttachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	xColorAttachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	xColorAttachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	xColorAttachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	xColorAttachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	xColorAttachmentDesc.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference xColorAttachmentRef;
	memset(&xColorAttachmentRef, 0, sizeof(xColorAttachmentRef));
	xColorAttachmentRef.attachment = 0;
	xColorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription xSubpassDesc;
	memset(&xSubpassDesc, 0, sizeof(xSubpassDesc));
	xSubpassDesc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	xSubpassDesc.colorAttachmentCount = 1;
	xSubpassDesc.pColorAttachments = &xColorAttachmentRef;

	VkSubpassDependency xSubpassDependency;
	memset(&xSubpassDependency, 0, sizeof(xSubpassDependency));
	xSubpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	xSubpassDependency.dstSubpass = 0;
	xSubpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	xSubpassDependency.srcAccessMask = 0;
	xSubpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	xSubpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkRenderPassCreateInfo xRenderPassCreateInfo;
	memset(&xRenderPassCreateInfo, 0, sizeof(xRenderPassCreateInfo));
	xRenderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	xRenderPassCreateInfo.attachmentCount = 1;
	xRenderPassCreateInfo.pAttachments = &xColorAttachmentDesc;
	xRenderPassCreateInfo.subpassCount = 1;
	xRenderPassCreateInfo.pSubpasses = &xSubpassDesc;
	xRenderPassCreateInfo.dependencyCount = 1;
	xRenderPassCreateInfo.pDependencies = &xSubpassDependency;

	if (vkCreateRenderPass(s_xVulkan.xDevice, &xRenderPassCreateInfo, 0, &s_xVulkan.xRenderPass) != VK_SUCCESS) {
		printf("Failed creating render pass\n");
		return false;
	}

	return true;
}

static bool Vulkan_CreateShaderModules(const char* pcVertFilePath, const char* pcFragFilePath) {
	char* pcVertShaderBytes;
	char* pcFragShaderBytes;
	
	uint32_t nVertShaderSize;
	uint32_t nFragShaderSize;

	FileUtl_ReadBinary(&pcVertShaderBytes, &nVertShaderSize, pcVertFilePath);
	FileUtl_ReadBinary(&pcFragShaderBytes, &nFragShaderSize, pcFragFilePath);

	VkShaderModuleCreateInfo xVertCreateInfo;
	memset(&xVertCreateInfo, 0, sizeof(xVertCreateInfo));
	xVertCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	xVertCreateInfo.codeSize = nVertShaderSize;
	xVertCreateInfo.pCode = (const uint32_t*)pcVertShaderBytes;

	VkShaderModuleCreateInfo xFragCreateInfo;
	memset(&xFragCreateInfo, 0, sizeof(xFragCreateInfo));
	xFragCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	xFragCreateInfo.codeSize = nFragShaderSize;
	xFragCreateInfo.pCode = (const uint32_t*)pcFragShaderBytes;

	if (vkCreateShaderModule(s_xVulkan.xDevice, &xVertCreateInfo, 0, &s_xVulkan.xVertModule) != VK_SUCCESS) {
		printf("Failed creating vertex shader module\n");

		free(pcVertShaderBytes);

		return false;
	}

	free(pcVertShaderBytes);

	if (vkCreateShaderModule(s_xVulkan.xDevice, &xFragCreateInfo, 0, &s_xVulkan.xFragModule) != VK_SUCCESS) {
		printf("Failed creating fragment shader module\n");

		free(pcFragShaderBytes);

		return false;
	}

	free(pcFragShaderBytes);

	return true;
}

static bool Vulkan_DestroyShaderModules(void) {
	vkDestroyShaderModule(s_xVulkan.xDevice, s_xVulkan.xVertModule, 0);
	vkDestroyShaderModule(s_xVulkan.xDevice, s_xVulkan.xFragModule, 0);

	return true;
}

static bool Vulkan_CreateGraphicsPipeline(void) {
	VkPipelineShaderStageCreateInfo xVertShaderStageCreateInfo;
	memset(&xVertShaderStageCreateInfo, 0, sizeof(xVertShaderStageCreateInfo));
	xVertShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	xVertShaderStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	xVertShaderStageCreateInfo.module = s_xVulkan.xVertModule;
	xVertShaderStageCreateInfo.pName = "main";

	VkPipelineShaderStageCreateInfo xFragShaderStageCreateInfo;
	memset(&xFragShaderStageCreateInfo, 0, sizeof(xFragShaderStageCreateInfo));
	xFragShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	xFragShaderStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	xFragShaderStageCreateInfo.module = s_xVulkan.xFragModule;
	xFragShaderStageCreateInfo.pName = "main";

	VkPipelineShaderStageCreateInfo axShaderStages[] = { xVertShaderStageCreateInfo, xFragShaderStageCreateInfo };

	VkPipelineVertexInputStateCreateInfo xVertexInputCreateInfo;
	memset(&xVertexInputCreateInfo, 0, sizeof(xVertexInputCreateInfo));
	xVertexInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	xVertexInputCreateInfo.vertexBindingDescriptionCount = 0;
	xVertexInputCreateInfo.vertexAttributeDescriptionCount = 0;

	VkPipelineInputAssemblyStateCreateInfo xInputAssemblyCreateInfo;
	memset(&xInputAssemblyCreateInfo, 0, sizeof(xInputAssemblyCreateInfo));
	xInputAssemblyCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	xInputAssemblyCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	xInputAssemblyCreateInfo.primitiveRestartEnable = VK_FALSE;

	VkViewport xViewport;
	memset(&xViewport, 0, sizeof(xViewport));
	xViewport.x = 0.0F;
	xViewport.y = 0.0F;
	xViewport.width = (float)s_xVulkan.xSwapChainExtent.width;
	xViewport.height = (float)s_xVulkan.xSwapChainExtent.height;
	xViewport.minDepth = 0.0F;
	xViewport.maxDepth = 1.0F;

	VkRect2D xScissor;
	memset(&xScissor, 0, sizeof(xScissor));
	xScissor.offset.x = 0;
	xScissor.offset.y = 0;
	xScissor.extent.width = s_xVulkan.xSwapChainExtent.width;
	xScissor.extent.height = s_xVulkan.xSwapChainExtent.height;

	VkPipelineViewportStateCreateInfo xViewportStateCreateInfo;
	memset(&xViewportStateCreateInfo, 0, sizeof(xViewportStateCreateInfo));
	xViewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	xViewportStateCreateInfo.viewportCount = 1;
	xViewportStateCreateInfo.pViewports = &xViewport;
	xViewportStateCreateInfo.scissorCount = 1;
	xViewportStateCreateInfo.pScissors = &xScissor;

	VkPipelineRasterizationStateCreateInfo xRasterizerCreateInfo;
	memset(&xRasterizerCreateInfo, 0, sizeof(xRasterizerCreateInfo));
	xRasterizerCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	xRasterizerCreateInfo.depthClampEnable = VK_FALSE;
	xRasterizerCreateInfo.rasterizerDiscardEnable = VK_FALSE;
	xRasterizerCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
	xRasterizerCreateInfo.lineWidth = 1.0F;
	xRasterizerCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
	xRasterizerCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
	xRasterizerCreateInfo.depthBiasEnable = VK_FALSE;
	xRasterizerCreateInfo.depthBiasConstantFactor = 0.0F;
	xRasterizerCreateInfo.depthBiasClamp = 0.0F;
	xRasterizerCreateInfo.depthBiasSlopeFactor = 0.0F;

	VkPipelineMultisampleStateCreateInfo xMultisamplingCreateInfo;
	memset(&xMultisamplingCreateInfo, 0, sizeof(xMultisamplingCreateInfo));
	xMultisamplingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	xMultisamplingCreateInfo.sampleShadingEnable = VK_FALSE;
	xMultisamplingCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	xMultisamplingCreateInfo.minSampleShading = 1.0F;
	xMultisamplingCreateInfo.pSampleMask = 0;
	xMultisamplingCreateInfo.alphaToCoverageEnable = VK_FALSE;
	xMultisamplingCreateInfo.alphaToOneEnable = VK_FALSE;

	VkPipelineColorBlendAttachmentState xColorBlendAttachment;
	memset(&xColorBlendAttachment, 0, sizeof(xColorBlendAttachment));
	xColorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	xColorBlendAttachment.blendEnable = VK_FALSE;
	xColorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
	xColorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	xColorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	xColorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	xColorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	xColorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

	VkPipelineColorBlendStateCreateInfo xColorBlendCreateInfo;
	memset(&xColorBlendCreateInfo, 0, sizeof(xColorBlendCreateInfo));
	xColorBlendCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	xColorBlendCreateInfo.logicOpEnable = VK_FALSE;
	xColorBlendCreateInfo.logicOp = VK_LOGIC_OP_COPY;
	xColorBlendCreateInfo.attachmentCount = 1;
	xColorBlendCreateInfo.pAttachments = &xColorBlendAttachment;
	xColorBlendCreateInfo.blendConstants[0] = 0.0F;
	xColorBlendCreateInfo.blendConstants[1] = 0.0F;
	xColorBlendCreateInfo.blendConstants[2] = 0.0F;
	xColorBlendCreateInfo.blendConstants[3] = 0.0F;

	VkDynamicState axDynamicState[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

	VkPipelineDynamicStateCreateInfo xDynamicStateCreateInfo;
	memset(&xDynamicStateCreateInfo, 0, sizeof(xDynamicStateCreateInfo));
	xDynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	xDynamicStateCreateInfo.dynamicStateCount = ARRAY_LENGTH(axDynamicState);
	xDynamicStateCreateInfo.pDynamicStates = axDynamicState;

	VkPipelineLayoutCreateInfo xPipelineLayoutCreateInfo;
	memset(&xPipelineLayoutCreateInfo, 0, sizeof(xPipelineLayoutCreateInfo));
	xPipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	xPipelineLayoutCreateInfo.setLayoutCount = 0;
	xPipelineLayoutCreateInfo.pSetLayouts = 0;
	xPipelineLayoutCreateInfo.pushConstantRangeCount = 0;
	xPipelineLayoutCreateInfo.pPushConstantRanges = 0;

	if (vkCreatePipelineLayout(s_xVulkan.xDevice, &xPipelineLayoutCreateInfo, 0, &s_xVulkan.xPipelineLayout) != VK_SUCCESS) {
		printf("Failed creating pipeline layout\n");
		return false;
	}

	VkGraphicsPipelineCreateInfo xPipelineCreateInfo;
	memset(&xPipelineCreateInfo, 0, sizeof(xPipelineCreateInfo));
	xPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	xPipelineCreateInfo.stageCount = ARRAY_LENGTH(axShaderStages);
	xPipelineCreateInfo.pStages = axShaderStages;
	xPipelineCreateInfo.pVertexInputState = &xVertexInputCreateInfo;
	xPipelineCreateInfo.pInputAssemblyState = &xInputAssemblyCreateInfo;
	xPipelineCreateInfo.pViewportState = &xViewportStateCreateInfo;
	xPipelineCreateInfo.pRasterizationState = &xRasterizerCreateInfo;
	xPipelineCreateInfo.pMultisampleState = &xMultisamplingCreateInfo;
	xPipelineCreateInfo.pDepthStencilState = 0;
	xPipelineCreateInfo.pColorBlendState = &xColorBlendCreateInfo;
	xPipelineCreateInfo.pDynamicState = &xDynamicStateCreateInfo;
	xPipelineCreateInfo.layout = s_xVulkan.xPipelineLayout;
	xPipelineCreateInfo.renderPass = s_xVulkan.xRenderPass;
	xPipelineCreateInfo.subpass = 0;
	xPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;

	if (vkCreateGraphicsPipelines(s_xVulkan.xDevice, VK_NULL_HANDLE, 1, &xPipelineCreateInfo, 0, &s_xVulkan.xGraphicsPipeline) != VK_SUCCESS) {
		printf("Failed creating graphics pipeline\n");
		return false;
	}

	return true;
}

static bool Vulkan_CreateFrameBuffers(void) {
	s_xVulkan.pxFrameBuffers = (VkFramebuffer*)malloc(sizeof(VkFramebuffer) * s_xVulkan.nSwapChainImageCount);

	for (uint32_t i = 0; i < s_xVulkan.nSwapChainImageCount; ++i) {
    	VkImageView axAttachments[] = {
        	s_xVulkan.pxSwapChainImageViews[i],
    	};

    	VkFramebufferCreateInfo xFramebufferCreateInfo;
		memset(&xFramebufferCreateInfo, 0, sizeof(xFramebufferCreateInfo));
    	xFramebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    	xFramebufferCreateInfo.renderPass = s_xVulkan.xRenderPass;
    	xFramebufferCreateInfo.attachmentCount = ARRAY_LENGTH(axAttachments);
    	xFramebufferCreateInfo.pAttachments = axAttachments;
    	xFramebufferCreateInfo.width = s_xVulkan.xSwapChainExtent.width;
    	xFramebufferCreateInfo.height = s_xVulkan.xSwapChainExtent.height;
    	xFramebufferCreateInfo.layers = 1;

    	if (vkCreateFramebuffer(s_xVulkan.xDevice, &xFramebufferCreateInfo, 0, &s_xVulkan.pxFrameBuffers[i]) != VK_SUCCESS) {
        	printf("Failed creating framebuffer\n");
			return false;
    	}
	}

	return true;
}

static bool Vulkan_CreateCommandPool(int32_t nGraphicsQueueIndex) {
	VkCommandPoolCreateInfo xCommandPoolCreateInfo;
	memset(&xCommandPoolCreateInfo, 0, sizeof(xCommandPoolCreateInfo));
	xCommandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	xCommandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	xCommandPoolCreateInfo.queueFamilyIndex = nGraphicsQueueIndex;

	if (vkCreateCommandPool(s_xVulkan.xDevice, &xCommandPoolCreateInfo, 0, &s_xVulkan.xCommandPool) != VK_SUCCESS) {
	    printf("Failed creating command pool\n");
		return false;
	}

	return true;
}

static bool Vulkan_CreateCommandBuffer(void) {
	VkCommandBufferAllocateInfo xCommandBufferAllocCreateInfo;
	memset(&xCommandBufferAllocCreateInfo, 0, sizeof(xCommandBufferAllocCreateInfo));
	xCommandBufferAllocCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	xCommandBufferAllocCreateInfo.commandPool = s_xVulkan.xCommandPool;
	xCommandBufferAllocCreateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	xCommandBufferAllocCreateInfo.commandBufferCount = 1;

	if (vkAllocateCommandBuffers(s_xVulkan.xDevice, &xCommandBufferAllocCreateInfo, &s_xVulkan.xCommandBuffer) != VK_SUCCESS) {
    	printf("Failed allocating command buffers\n");
		return false;
	}

	return true;
}

static bool Vulkan_RecordCommandBuffer(uint32_t nImageIndex) {
	VkCommandBufferBeginInfo xCommandBufferBeginInfo;
	memset(&xCommandBufferBeginInfo, 0, sizeof(xCommandBufferBeginInfo));
	xCommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	xCommandBufferBeginInfo.flags = 0;
	xCommandBufferBeginInfo.pInheritanceInfo = 0;

	if (vkBeginCommandBuffer(s_xVulkan.xCommandBuffer, &xCommandBufferBeginInfo) != VK_SUCCESS) {
	    printf("Failed recording command buffer\n");
		return false;
	}

	VkClearValue xClearColor;
	memset(&xClearColor, 0, sizeof(xClearColor));
	xClearColor.color.float32[0] = 0.0F;
	xClearColor.color.float32[1] = 0.0F;
	xClearColor.color.float32[2] = 0.0F;
	xClearColor.color.float32[3] = 1.0F;

	VkRenderPassBeginInfo xRenderPassCreateInfo;
	memset(&xRenderPassCreateInfo, 0, sizeof(xRenderPassCreateInfo));
	xRenderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	xRenderPassCreateInfo.renderPass = s_xVulkan.xRenderPass;
	xRenderPassCreateInfo.framebuffer = s_xVulkan.pxFrameBuffers[nImageIndex];
	xRenderPassCreateInfo.renderArea.offset.x = 0;
	xRenderPassCreateInfo.renderArea.offset.y = 0;
	xRenderPassCreateInfo.renderArea.extent = s_xVulkan.xSwapChainExtent;
	xRenderPassCreateInfo.clearValueCount = 1;
	xRenderPassCreateInfo.pClearValues = &xClearColor;

	vkCmdBeginRenderPass(s_xVulkan.xCommandBuffer, &xRenderPassCreateInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(s_xVulkan.xCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, s_xVulkan.xGraphicsPipeline);

		VkViewport xViewport;
		memset(&xViewport, 0, sizeof(xViewport));
		xViewport.x = 0.0F;
		xViewport.y = 0.0F;
		xViewport.width = (float)s_xVulkan.xSwapChainExtent.width;
		xViewport.height = (float)s_xVulkan.xSwapChainExtent.height;
		xViewport.minDepth = 0.0F;
		xViewport.maxDepth = 1.0F;
		vkCmdSetViewport(s_xVulkan.xCommandBuffer, 0, 1, &xViewport);

		VkRect2D xScissor;
		memset(&xScissor, 0, sizeof(xScissor));
		xScissor.offset.x = 0;
		xScissor.offset.y = 0;
		xScissor.extent.width = s_xVulkan.xSwapChainExtent.width;
		xScissor.extent.height = s_xVulkan.xSwapChainExtent.height;
		vkCmdSetScissor(s_xVulkan.xCommandBuffer, 0, 1, &xScissor);            

		vkCmdDraw(s_xVulkan.xCommandBuffer, 3, 1, 0, 0);

	vkCmdEndRenderPass(s_xVulkan.xCommandBuffer);

	if (vkEndCommandBuffer(s_xVulkan.xCommandBuffer) != VK_SUCCESS) {
		printf("Failed recording command buffer\n");
		return false;
	}

	return true;
}

static bool Vulkan_CreatSyncObjects(void) {
	VkSemaphoreCreateInfo xSemaphoreCreateInfo;
	memset(&xSemaphoreCreateInfo, 0, sizeof(xSemaphoreCreateInfo));
    xSemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo xFenceCreateInfo;
	memset(&xFenceCreateInfo, 0, sizeof(xFenceCreateInfo));
	xFenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	xFenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	if (vkCreateSemaphore(s_xVulkan.xDevice, &xSemaphoreCreateInfo, 0, &s_xVulkan.xImageAvailableSemaphore) != VK_SUCCESS) {
		printf("Failed creating semaphore\n");
		return false;
	}

    if (vkCreateSemaphore(s_xVulkan.xDevice, &xSemaphoreCreateInfo, 0, &s_xVulkan.xRenderFinishedSemaphore) != VK_SUCCESS) {
		printf("Failed creating semaphore\n");
		return false;
	}

    if (vkCreateFence(s_xVulkan.xDevice, &xFenceCreateInfo, 0, &s_xVulkan.xInFlightFence) != VK_SUCCESS) {
		printf("Failed creating fence\n");
		return false;
	}

	return true;
}

struct xVulkan_t* Vulkan_Alloc(struct xWindow_t* pxWindow) {
	memset(&s_xVulkan, 0, sizeof(s_xVulkan));

	int32_t nGraphicsQueueIndex = -1;
	int32_t nPresentQueueIndex = -1;

	if (!Vulkan_CreateInstance()) {
		Vulkan_Cleanup();
		return 0;
	}

	if (!Vulkan_CreateSurface(pxWindow)) {
		Vulkan_Cleanup();
		return 0;
	}

	if (!Vulkan_FindPhysicalDevice()) {
		Vulkan_Cleanup();
		return 0;
	}

	if (!Vulkan_FindQueueFamilies(&nGraphicsQueueIndex, &nPresentQueueIndex)) {
		Vulkan_Cleanup();
		return 0;
	}

	if (!Vulkan_CheckPhysicalDeviceExtensions()) {
		Vulkan_Cleanup();
		return 0;
	}

	if (!Vulkan_CreateLogicalDevice(nGraphicsQueueIndex, nPresentQueueIndex)) {
		Vulkan_Cleanup();
		return 0;
	}

	if (!Vulkan_CheckSwapChainCapabilities()) {
		Vulkan_Cleanup();
		return 0;
	}

	if (!Vulkan_CreateSwapChain(pxWindow, nGraphicsQueueIndex, nPresentQueueIndex)) {
		Vulkan_Cleanup();
		return 0;
	}

	if (!Vulkan_CreateImageViews()) {
		Vulkan_Cleanup();
		return 0;
	}

	if (!Vulkan_CreateRenderPass()) {
		Vulkan_Cleanup();
		return 0;
	}

	if (!Vulkan_CreateShaderModules("../shaders/test.vert.spv", "../shaders/test.frag.spv")) {
		Vulkan_Cleanup();
		return 0;
	}

	if (!Vulkan_CreateGraphicsPipeline()) {
		Vulkan_Cleanup();
		return 0;
	}

	if (!Vulkan_CreateFrameBuffers()) {
		Vulkan_Cleanup();
		return 0;
	}

	if (!Vulkan_CreateCommandPool(nGraphicsQueueIndex)) {
		Vulkan_Cleanup();
		return 0;
	}

	if (!Vulkan_CreateCommandBuffer()) {
		Vulkan_Cleanup();
		return 0;
	}

	if (!Vulkan_CreatSyncObjects()) {
		Vulkan_Cleanup();
		return 0;
	}

	return &s_xVulkan;
}

void Vulkan_Free(struct xVulkan_t* pxVulkan) {
	UNUSED(pxVulkan);

	Vulkan_Cleanup();
}

bool Vulkan_Draw(struct xVulkan_t* pxVulkan) {
	vkResetFences(pxVulkan->xDevice, 1, &pxVulkan->xInFlightFence);

	uint32_t nImageIndex;
	vkAcquireNextImageKHR(pxVulkan->xDevice, pxVulkan->xSwapChain, UINT64_MAX, pxVulkan->xImageAvailableSemaphore, VK_NULL_HANDLE, &nImageIndex);

	vkResetCommandBuffer(pxVulkan->xCommandBuffer, 0);

	if (!Vulkan_RecordCommandBuffer(nImageIndex)) {
		return false;
	}

	VkSemaphore axWaitSemaphores[] = { pxVulkan->xImageAvailableSemaphore };
	VkSemaphore axSignalSemaphores[] = { pxVulkan->xRenderFinishedSemaphore };
	VkPipelineStageFlags axWaitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	VkSwapchainKHR axSwapChains[] = { pxVulkan->xSwapChain };

	VkSubmitInfo xSubmitInfo;
	memset(&xSubmitInfo, 0, sizeof(xSubmitInfo));
	xSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	xSubmitInfo.waitSemaphoreCount = ARRAY_LENGTH(axWaitSemaphores);
	xSubmitInfo.pWaitSemaphores = axWaitSemaphores;
	xSubmitInfo.pWaitDstStageMask = axWaitStages;
	xSubmitInfo.commandBufferCount = 1;
	xSubmitInfo.pCommandBuffers = &pxVulkan->xCommandBuffer;
	xSubmitInfo.signalSemaphoreCount = ARRAY_LENGTH(axSignalSemaphores);
	xSubmitInfo.pSignalSemaphores = axSignalSemaphores;

	if (vkQueueSubmit(pxVulkan->xGraphicsQueue, 1, &xSubmitInfo, pxVulkan->xInFlightFence) != VK_SUCCESS) {
		printf("Failed submitting draw command buffer\n");
		return false;
	}

	VkPresentInfoKHR xPresentInfo;
	memset(&xPresentInfo, 0, sizeof(xPresentInfo));
	xPresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	xPresentInfo.waitSemaphoreCount = ARRAY_LENGTH(axSignalSemaphores);
	xPresentInfo.pWaitSemaphores = axSignalSemaphores;
	xPresentInfo.swapchainCount = ARRAY_LENGTH(axSwapChains);
	xPresentInfo.pSwapchains = axSwapChains;
	xPresentInfo.pImageIndices = &nImageIndex;

	vkQueuePresentKHR(pxVulkan->xPresentQueue, &xPresentInfo);

	vkWaitForFences(pxVulkan->xDevice, 1, &pxVulkan->xInFlightFence, VK_TRUE, UINT64_MAX);

	return true;
}
