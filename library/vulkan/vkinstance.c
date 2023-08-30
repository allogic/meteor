#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <common/macros.h>

#include <platform/nativewindow.h>

#include <vulkan/vkinstance.h>

#ifdef OS_WINDOWS
#	include <windows.h>
#	include <vulkan/vulkan_win32.h>
#endif

#ifdef OS_LINUX
#	include <wayland-client.h>
#	include <vulkan/vulkan_wayland.h>
#endif

struct xVkInstance_t {
	VkInstance xInstance;
#ifdef DEBUG
	VkDebugUtilsMessengerEXT xDebugMessenger;
#endif
	VkSurfaceKHR xSurface;
	VkPhysicalDevice xPhysicalDevice;
	VkSurfaceFormatKHR xPreferedSurfaceFormat;
	VkPresentModeKHR xPreferedPresentMode;
	VkDevice xDevice;
	int32_t nGraphicsQueueIndex;
	int32_t nPresentQueueIndex;
	VkQueue xGraphicsQueue;
	VkQueue xPresentQueue;
	VkCommandPool xCommandPool;
};

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

static void VkInstance_CreateInstance(struct xVkInstance_t* pxVkInstance) {
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

	VK_CHECK(vkCreateInstance(&xInstanceCreateInfo, 0, &pxVkInstance->xInstance));

#ifdef DEBUG
	CreateDebugUtilsMessengerEXT(pxVkInstance->xInstance, &xDebugCreateInfo, 0, &pxVkInstance->xDebugMessenger);
#endif
}

static void VkInstance_CreateSurface(struct xVkInstance_t* pxVkInstance) {
#ifdef OS_WINDOWS
	VkWin32SurfaceCreateInfoKHR xSurfaceCreateInfo;
	memset(&xSurfaceCreateInfo, 0, sizeof(xSurfaceCreateInfo));
	xSurfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	xSurfaceCreateInfo.hwnd = NativeWindow_GetWindowHandle();
	xSurfaceCreateInfo.hinstance = NativeWindow_GetModuleHandle();

	VK_CHECK(vkCreateWin32SurfaceKHR(pxVkInstance->xInstance, &xSurfaceCreateInfo, 0, &pxVkInstance->xSurface));
#endif

#ifdef OS_LINUX
	VkWaylandSurfaceCreateInfoKHR xSurfaceCreateInfo;
	memset(&xSurfaceCreateInfo, 0, sizeof(xSurfaceCreateInfo));
	xSurfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR;
	xSurfaceCreateInfo.display = NativeWindow_GetDisplayHandle();
	xSurfaceCreateInfo.surface = NativeWindow_GetSurfaceHandle();

	VK_CHECK(vkCreateWaylandSurfaceKHR(pxVkInstance->xInstance, &xSurfaceCreateInfo, 0, &pxVkInstance->xSurface));
#endif
}

static void VkInstance_FindPhysicalDevice(struct xVkInstance_t* pxVkInstance) {
	uint32_t nPhysicalDeviceCount;
	VK_CHECK(vkEnumeratePhysicalDevices(pxVkInstance->xInstance, &nPhysicalDeviceCount, 0));

	VkPhysicalDevice* pxPhysicalDevices = (VkPhysicalDevice*)malloc(sizeof(VkPhysicalDevice) * nPhysicalDeviceCount);
	VK_CHECK(vkEnumeratePhysicalDevices(pxVkInstance->xInstance, &nPhysicalDeviceCount, pxPhysicalDevices));

	for (uint32_t i = 0; i < nPhysicalDeviceCount; ++i) {
		VkPhysicalDeviceProperties xPhysicalDeviceProperties;
		VkPhysicalDeviceFeatures xPhysicalDeviceFeatures;

		vkGetPhysicalDeviceProperties(pxPhysicalDevices[i], &xPhysicalDeviceProperties);
		vkGetPhysicalDeviceFeatures(pxPhysicalDevices[i], &xPhysicalDeviceFeatures);

		if (xPhysicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
			if (xPhysicalDeviceFeatures.geometryShader) {
				memcpy(&pxVkInstance->xPhysicalDevice, &pxPhysicalDevices[i], sizeof(pxVkInstance->xPhysicalDevice));
				break;
			}
		}
	}

	free(pxPhysicalDevices);
}

static void VkInstance_FindQueueFamilies(struct xVkInstance_t* pxVkInstance) {
	uint32_t nQueueFamilyCount;
	vkGetPhysicalDeviceQueueFamilyProperties(pxVkInstance->xPhysicalDevice, &nQueueFamilyCount, 0);

	VkQueueFamilyProperties* pxQueueFamilyProperties = (VkQueueFamilyProperties*)malloc(sizeof(VkQueueFamilyProperties) * nQueueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(pxVkInstance->xPhysicalDevice, &nQueueFamilyCount, pxQueueFamilyProperties);

	for (uint32_t i = 0; i < nQueueFamilyCount; ++i) {
		uint32_t nGraphicsSupport = 0;
		uint32_t nPresentSupport = 0;

		nGraphicsSupport = pxQueueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT;

		VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(pxVkInstance->xPhysicalDevice, i, pxVkInstance->xSurface, &nPresentSupport));

		if (nGraphicsSupport && (pxVkInstance->nGraphicsQueueIndex == -1)) {
			pxVkInstance->nGraphicsQueueIndex = i;
		} else if (nPresentSupport && (pxVkInstance->nPresentQueueIndex == -1)) {
			pxVkInstance->nPresentQueueIndex = i;
		}

		if ((pxVkInstance->nGraphicsQueueIndex != -1) && (pxVkInstance->nPresentQueueIndex != -1)) {
			break;
		}
	}

	free(pxQueueFamilyProperties);
}

static void VkInstance_CheckPhysicalDeviceExtensions(struct xVkInstance_t* pxVkInstance) {
	uint32_t nAvailableDeviceExtensionCount;
	VK_CHECK(vkEnumerateDeviceExtensionProperties(pxVkInstance->xPhysicalDevice, 0, &nAvailableDeviceExtensionCount, 0));

	VkExtensionProperties* pxAvailableDeviceExtensions = (VkExtensionProperties*)malloc(sizeof(VkExtensionProperties) * nAvailableDeviceExtensionCount);
	VK_CHECK(vkEnumerateDeviceExtensionProperties(pxVkInstance->xPhysicalDevice, 0, &nAvailableDeviceExtensionCount, pxAvailableDeviceExtensions));

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

			break;
		}
	}

	free(pxAvailableDeviceExtensions);
}

static void VkInstance_CreateLogicalDevice(struct xVkInstance_t* pxVkInstance) {
#ifdef DEBUG
	printf("GraphicsQueueIndex %d\n", pxVkInstance->nGraphicsQueueIndex);
	printf("PresentQueueIndex %d\n", pxVkInstance->nPresentQueueIndex);
#endif

	float fQueuePriority = 1.0f;

	VkDeviceQueueCreateInfo xQueueCreateInfos[2];
	memset(&xQueueCreateInfos, 0, sizeof(xQueueCreateInfos));

	xQueueCreateInfos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	xQueueCreateInfos[0].queueFamilyIndex = pxVkInstance->nGraphicsQueueIndex;
	xQueueCreateInfos[0].queueCount = 1;
	xQueueCreateInfos[0].pQueuePriorities = &fQueuePriority;

	xQueueCreateInfos[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	xQueueCreateInfos[1].queueFamilyIndex = pxVkInstance->nPresentQueueIndex;
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

	VK_CHECK(vkCreateDevice(pxVkInstance->xPhysicalDevice, &xDeviceCreateInfo, 0, &pxVkInstance->xDevice));

	vkGetDeviceQueue(pxVkInstance->xDevice, pxVkInstance->nGraphicsQueueIndex, 0, &pxVkInstance->xGraphicsQueue);
	vkGetDeviceQueue(pxVkInstance->xDevice, pxVkInstance->nPresentQueueIndex, 0, &pxVkInstance->xPresentQueue);
}

static void VkInstance_CheckSurfaceCapabilities(struct xVkInstance_t* pxVkInstance) {
	uint32_t nSurfaceFormatCount;
	uint32_t nPresentModeCount;

	VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(pxVkInstance->xPhysicalDevice, pxVkInstance->xSurface, &nSurfaceFormatCount, 0));
	VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(pxVkInstance->xPhysicalDevice, pxVkInstance->xSurface, &nPresentModeCount, 0));

	VkSurfaceFormatKHR* pxSurfaceFormats = (VkSurfaceFormatKHR*)malloc(sizeof(VkSurfaceFormatKHR) * nSurfaceFormatCount);
	VkPresentModeKHR* pxPresentModes = (VkPresentModeKHR*)malloc(sizeof(VkPresentModeKHR) * nPresentModeCount);

	VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(pxVkInstance->xPhysicalDevice, pxVkInstance->xSurface, &nSurfaceFormatCount, pxSurfaceFormats));
	VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(pxVkInstance->xPhysicalDevice, pxVkInstance->xSurface, &nPresentModeCount, pxPresentModes));

	for (uint32_t i = 0; i < nSurfaceFormatCount; ++i) {
		if ((pxSurfaceFormats[i].format == VK_FORMAT_B8G8R8A8_SRGB) && (pxSurfaceFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)) {
			memcpy(&pxVkInstance->xPreferedSurfaceFormat, &pxSurfaceFormats[i], sizeof(pxVkInstance->xPreferedSurfaceFormat));
			break;
		}
	}

	for (uint32_t i = 0; i < nPresentModeCount; ++i) {
		if (pxPresentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
			memcpy(&pxVkInstance->xPreferedPresentMode, &pxPresentModes[i], sizeof(pxVkInstance->xPreferedPresentMode));
			break;
		}
	}

	free(pxSurfaceFormats);
	free(pxPresentModes);
}

static void VkInstance_CreateCommandPool(struct xVkInstance_t* pxVkInstance) {
	VkCommandPoolCreateInfo xCommandPoolCreateInfo;
	memset(&xCommandPoolCreateInfo, 0, sizeof(xCommandPoolCreateInfo));
	xCommandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	xCommandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	xCommandPoolCreateInfo.queueFamilyIndex = pxVkInstance->nGraphicsQueueIndex;

	VK_CHECK(vkCreateCommandPool(pxVkInstance->xDevice, &xCommandPoolCreateInfo, 0, &pxVkInstance->xCommandPool));
}

struct xVkInstance_t* VkInstance_Alloc(void) {
	struct xVkInstance_t* pxVkInstance = (struct xVkInstance_t*)calloc(1, sizeof(struct xVkInstance_t));

	pxVkInstance->nGraphicsQueueIndex = -1;
	pxVkInstance->nPresentQueueIndex = -1;

	VkInstance_CreateInstance(pxVkInstance);
	VkInstance_CreateSurface(pxVkInstance);
	VkInstance_FindPhysicalDevice(pxVkInstance);
	VkInstance_FindQueueFamilies(pxVkInstance);
	VkInstance_CheckPhysicalDeviceExtensions(pxVkInstance);
	VkInstance_CreateLogicalDevice(pxVkInstance);
	VkInstance_CheckSurfaceCapabilities(pxVkInstance);
	VkInstance_CreateCommandPool(pxVkInstance);

	return pxVkInstance;
}

void VkInstance_Free(struct xVkInstance_t* pxVkInstance) {
	vkDestroyCommandPool(pxVkInstance->xDevice, pxVkInstance->xCommandPool, 0);

	vkDestroyDevice(pxVkInstance->xDevice, 0);

	vkDestroySurfaceKHR(pxVkInstance->xInstance, pxVkInstance->xSurface, 0);

#ifdef DEBUG
	DestroyDebugUtilsMessengerEXT(pxVkInstance->xInstance, pxVkInstance->xDebugMessenger, 0);
#endif

	vkDestroyInstance(pxVkInstance->xInstance, 0);

	free(pxVkInstance);
}

VkPhysicalDevice VkInstance_GetPhysicalDevice(struct xVkInstance_t* pxVkInstance) {
	return pxVkInstance->xPhysicalDevice;
}

VkDevice VkInstance_GetDevice(struct xVkInstance_t* pxVkInstance) {
	return pxVkInstance->xDevice;
}

VkSurfaceKHR VkInstance_GetSurface(struct xVkInstance_t* pxVkInstance) {
	return pxVkInstance->xSurface;
}

VkFormat VkInstance_GetPreferedSurfaceFormat(struct xVkInstance_t* pxVkInstance) {
	return pxVkInstance->xPreferedSurfaceFormat.format;
}

VkColorSpaceKHR VkInstance_GetPreferedSurfaceColorSpace(struct xVkInstance_t* pxVkInstance) {
	return pxVkInstance->xPreferedSurfaceFormat.colorSpace;
}

VkPresentModeKHR VkInstance_GetPreferedPresentMode(struct xVkInstance_t* pxVkInstance) {
	return pxVkInstance->xPreferedPresentMode;
}

uint32_t VkInstance_GetGraphicsQueueIndex(struct xVkInstance_t* pxVkInstance) {
	return pxVkInstance->nGraphicsQueueIndex;
}

uint32_t VkInstance_GetPresentQueueIndex(struct xVkInstance_t* pxVkInstance) {
	return pxVkInstance->nPresentQueueIndex;
}

VkQueue VkInstance_GetGraphicsQueue(struct xVkInstance_t* pxVkInstance) {
	return pxVkInstance->xGraphicsQueue;
}

VkQueue VkInstance_GetPresentQueue(struct xVkInstance_t* pxVkInstance) {
	return pxVkInstance->xPresentQueue;
}

VkCommandPool VkInstance_GetCommandPool(struct xVkInstance_t* pxVkInstance) {
	return pxVkInstance->xCommandPool;
}

void VkInstance_WaitIdle(struct xVkInstance_t* pxVkInstance) {
	VK_CHECK(vkDeviceWaitIdle(pxVkInstance->xDevice));
}
