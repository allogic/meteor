#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <macros.h>

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
};

static struct xVulkan_t s_xVulkan;

static const char* s_apInstanceExtensions[] = {
	"VK_KHR_surface",
#ifdef DEBUG
	"VK_EXT_debug_utils",
#endif
#ifdef OS_WINDOWS
	"VK_KHR_win32_surface",
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
	if (s_xVulkan.xDevice) {
		vkDestroyDevice(s_xVulkan.xDevice, 0);
	}

	if (s_xVulkan.xInstance && s_xVulkan.xSurface) {
		vkDestroySurfaceKHR(s_xVulkan.xInstance, s_xVulkan.xSurface, 0);
	}

#ifdef DEBUG
	if (s_xVulkan.xInstance && s_xVulkan.xDebugMessenger) {
		DestroyDebugUtilsMessengerEXT(s_xVulkan.xInstance, s_xVulkan.xDebugMessenger, 0);
	}
#endif

	if (s_xVulkan.xInstance) {
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
	xInstanceCreateInfo.enabledExtensionCount = ARRAY_LENGTH(s_apInstanceExtensions);
	xInstanceCreateInfo.ppEnabledExtensionNames = s_apInstanceExtensions;

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
		printf("failed to create window surface\n");
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

static bool Vulkan_CheckSwapChainCapabilities(void) {
	uint32_t nSurfaceFormatCount;
	uint32_t nPresentModeCount;

	vkGetPhysicalDeviceSurfaceFormatsKHR(s_xVulkan.xPhysicalDevice, s_xVulkan.xSurface, &nSurfaceFormatCount, 0);
	vkGetPhysicalDeviceSurfacePresentModesKHR(s_xVulkan.xPhysicalDevice, s_xVulkan.xSurface, &nPresentModeCount, 0);

	VkSurfaceFormatKHR* pxSurfaceFormats = (VkSurfaceFormatKHR*)malloc(sizeof(VkSurfaceFormatKHR) * nSurfaceFormatCount);
	VkPresentModeKHR* pxPresentModes = (VkPresentModeKHR*)malloc(sizeof(VkPresentModeKHR) * nPresentModeCount);

	vkGetPhysicalDeviceSurfaceFormatsKHR(s_xVulkan.xPhysicalDevice, s_xVulkan.xSurface, &nSurfaceFormatCount, pxSurfaceFormats);
	vkGetPhysicalDeviceSurfacePresentModesKHR(s_xVulkan.xPhysicalDevice, s_xVulkan.xSurface, &nPresentModeCount, pxPresentModes);

	uint32_t nSwapChainAdequate = (nSurfaceFormatCount > 0) && (nPresentModeCount > 0);

	if (nSwapChainAdequate == 0) {
		if (pxSurfaceFormats) {
			free(pxSurfaceFormats);
		}

		if (pxPresentModes) {
			free(pxPresentModes);
		}

		printf("Failed swap chain capabilities check\n");
		return false;
	}

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

static bool Vulkan_CreateSwapChain(struct xWindow_t* pxWindow) {
	VkExtent2D xSwapChainExtent;
	memset(&xSwapChainExtent, 0, sizeof(xSwapChainExtent));
	xSwapChainExtent.width = Window_GetWidth(pxWindow);
	xSwapChainExtent.height = Window_GetHeight(pxWindow);

	VkSurfaceCapabilitiesKHR xSurfaceCapabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(s_xVulkan.xPhysicalDevice, s_xVulkan.xSurface, &xSurfaceCapabilities);

	uint32_t nImageCount = xSurfaceCapabilities.minImageCount + 1;

	VkSwapchainCreateInfoKHR xSwapChaincreateInfo;
	memset(&xSwapChaincreateInfo, 0, sizeof(xSwapChaincreateInfo));
	xSwapChaincreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	xSwapChaincreateInfo.surface = s_xVulkan.xSurface;
	xSwapChaincreateInfo.minImageCount = nImageCount;
	xSwapChaincreateInfo.imageFormat = s_xVulkan.xPreferedSurfaceFormat.format;
	xSwapChaincreateInfo.imageColorSpace = s_xVulkan.xPreferedSurfaceFormat.colorSpace;
	xSwapChaincreateInfo.imageExtent = xSwapChainExtent;
	xSwapChaincreateInfo.imageArrayLayers = 1;
	xSwapChaincreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	// TODO: Finish tha swap freagin chain..

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

struct xVulkan_t* Vulkan_Alloc(struct xWindow_t* pxWindow) {
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

	if (!Vulkan_CheckSwapChainCapabilities()) {
		Vulkan_Cleanup();
		return 0;
	}

	if (!Vulkan_CreateSwapChain(pxWindow)) {
		Vulkan_Cleanup();
		return 0;
	}

	if (!Vulkan_CreateLogicalDevice(nGraphicsQueueIndex, nPresentQueueIndex)) {
		Vulkan_Cleanup();
		return 0;
	}

	return &s_xVulkan;
}

void Vulkan_Free(struct xVulkan_t* pxVulkan) {
	UNUSED(pxVulkan);

	Vulkan_Cleanup();
}
