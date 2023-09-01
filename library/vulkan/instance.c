#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <common/macros.h>

#include <platform/nativewindow.h>

#include <vulkan/instance.h>

#ifdef OS_WINDOWS
#	include <windows.h>
#	include <vulkan/vulkan_win32.h>
#endif

#ifdef OS_LINUX
#	include <wayland-client.h>
#	include <vulkan/vulkan_wayland.h>
#endif

struct xInstance_t {
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

static void Instance_CreateInstance(struct xInstance_t* pxInstance) {
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

	VK_CHECK(vkCreateInstance(&xInstanceCreateInfo, 0, &pxInstance->xInstance));

#ifdef DEBUG
	CreateDebugUtilsMessengerEXT(pxInstance->xInstance, &xDebugCreateInfo, 0, &pxInstance->xDebugMessenger);
#endif
}

static void Instance_CreateSurface(struct xInstance_t* pxInstance) {
#ifdef OS_WINDOWS
	VkWin32SurfaceCreateInfoKHR xSurfaceCreateInfo;
	memset(&xSurfaceCreateInfo, 0, sizeof(xSurfaceCreateInfo));
	xSurfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	xSurfaceCreateInfo.hwnd = NativeWindow_GetWindowHandle();
	xSurfaceCreateInfo.hinstance = NativeWindow_GetModuleHandle();

	VK_CHECK(vkCreateWin32SurfaceKHR(pxInstance->xInstance, &xSurfaceCreateInfo, 0, &pxInstance->xSurface));
#endif

#ifdef OS_LINUX
	VkWaylandSurfaceCreateInfoKHR xSurfaceCreateInfo;
	memset(&xSurfaceCreateInfo, 0, sizeof(xSurfaceCreateInfo));
	xSurfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR;
	xSurfaceCreateInfo.display = NativeWindow_GetDisplayHandle();
	xSurfaceCreateInfo.surface = NativeWindow_GetSurfaceHandle();

	VK_CHECK(vkCreateWaylandSurfaceKHR(pxInstance->xInstance, &xSurfaceCreateInfo, 0, &pxInstance->xSurface));
#endif
}

static void Instance_FindPhysicalDevice(struct xInstance_t* pxInstance) {
	uint32_t nPhysicalDeviceCount;
	VK_CHECK(vkEnumeratePhysicalDevices(pxInstance->xInstance, &nPhysicalDeviceCount, 0));

	VkPhysicalDevice* pxPhysicalDevices = (VkPhysicalDevice*)malloc(sizeof(VkPhysicalDevice) * nPhysicalDeviceCount);
	VK_CHECK(vkEnumeratePhysicalDevices(pxInstance->xInstance, &nPhysicalDeviceCount, pxPhysicalDevices));

	for (uint32_t i = 0; i < nPhysicalDeviceCount; ++i) {
		VkPhysicalDeviceProperties xPhysicalDeviceProperties;
		VkPhysicalDeviceFeatures xPhysicalDeviceFeatures;

		vkGetPhysicalDeviceProperties(pxPhysicalDevices[i], &xPhysicalDeviceProperties);
		vkGetPhysicalDeviceFeatures(pxPhysicalDevices[i], &xPhysicalDeviceFeatures);

		if (xPhysicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
			if (xPhysicalDeviceFeatures.geometryShader) {
				memcpy(&pxInstance->xPhysicalDevice, &pxPhysicalDevices[i], sizeof(pxInstance->xPhysicalDevice));
				break;
			}
		}
	}

	free(pxPhysicalDevices);
}

static void Instance_FindQueueFamilies(struct xInstance_t* pxInstance) {
	uint32_t nQueueFamilyCount;
	vkGetPhysicalDeviceQueueFamilyProperties(pxInstance->xPhysicalDevice, &nQueueFamilyCount, 0);

	VkQueueFamilyProperties* pxQueueFamilyProperties = (VkQueueFamilyProperties*)malloc(sizeof(VkQueueFamilyProperties) * nQueueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(pxInstance->xPhysicalDevice, &nQueueFamilyCount, pxQueueFamilyProperties);

	for (uint32_t i = 0; i < nQueueFamilyCount; ++i) {
		uint32_t nGraphicsSupport = 0;
		uint32_t nPresentSupport = 0;

		nGraphicsSupport = pxQueueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT;

		VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(pxInstance->xPhysicalDevice, i, pxInstance->xSurface, &nPresentSupport));

		if (nGraphicsSupport && (pxInstance->nGraphicsQueueIndex == -1)) {
			pxInstance->nGraphicsQueueIndex = i;
		} else if (nPresentSupport && (pxInstance->nPresentQueueIndex == -1)) {
			pxInstance->nPresentQueueIndex = i;
		}

		if ((pxInstance->nGraphicsQueueIndex != -1) && (pxInstance->nPresentQueueIndex != -1)) {
			break;
		}
	}

	free(pxQueueFamilyProperties);
}

static void Instance_CheckPhysicalDeviceExtensions(struct xInstance_t* pxInstance) {
	uint32_t nAvailableDeviceExtensionCount;
	VK_CHECK(vkEnumerateDeviceExtensionProperties(pxInstance->xPhysicalDevice, 0, &nAvailableDeviceExtensionCount, 0));

	VkExtensionProperties* pxAvailableDeviceExtensions = (VkExtensionProperties*)malloc(sizeof(VkExtensionProperties) * nAvailableDeviceExtensionCount);
	VK_CHECK(vkEnumerateDeviceExtensionProperties(pxInstance->xPhysicalDevice, 0, &nAvailableDeviceExtensionCount, pxAvailableDeviceExtensions));

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

static void Instance_CreateLogicalDevice(struct xInstance_t* pxInstance) {
#ifdef DEBUG
	printf("GraphicsQueueIndex %d\n", pxInstance->nGraphicsQueueIndex);
	printf("PresentQueueIndex %d\n", pxInstance->nPresentQueueIndex);
#endif

	float fQueuePriority = 1.0f;

	VkDeviceQueueCreateInfo xQueueCreateInfos[2];
	memset(&xQueueCreateInfos, 0, sizeof(xQueueCreateInfos));

	xQueueCreateInfos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	xQueueCreateInfos[0].queueFamilyIndex = pxInstance->nGraphicsQueueIndex;
	xQueueCreateInfos[0].queueCount = 1;
	xQueueCreateInfos[0].pQueuePriorities = &fQueuePriority;

	xQueueCreateInfos[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	xQueueCreateInfos[1].queueFamilyIndex = pxInstance->nPresentQueueIndex;
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

	VK_CHECK(vkCreateDevice(pxInstance->xPhysicalDevice, &xDeviceCreateInfo, 0, &pxInstance->xDevice));

	vkGetDeviceQueue(pxInstance->xDevice, pxInstance->nGraphicsQueueIndex, 0, &pxInstance->xGraphicsQueue);
	vkGetDeviceQueue(pxInstance->xDevice, pxInstance->nPresentQueueIndex, 0, &pxInstance->xPresentQueue);
}

static void Instance_CheckSurfaceCapabilities(struct xInstance_t* pxInstance) {
	uint32_t nSurfaceFormatCount;
	uint32_t nPresentModeCount;

	VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(pxInstance->xPhysicalDevice, pxInstance->xSurface, &nSurfaceFormatCount, 0));
	VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(pxInstance->xPhysicalDevice, pxInstance->xSurface, &nPresentModeCount, 0));

	VkSurfaceFormatKHR* pxSurfaceFormats = (VkSurfaceFormatKHR*)malloc(sizeof(VkSurfaceFormatKHR) * nSurfaceFormatCount);
	VkPresentModeKHR* pxPresentModes = (VkPresentModeKHR*)malloc(sizeof(VkPresentModeKHR) * nPresentModeCount);

	VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(pxInstance->xPhysicalDevice, pxInstance->xSurface, &nSurfaceFormatCount, pxSurfaceFormats));
	VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(pxInstance->xPhysicalDevice, pxInstance->xSurface, &nPresentModeCount, pxPresentModes));

	for (uint32_t i = 0; i < nSurfaceFormatCount; ++i) {
		if ((pxSurfaceFormats[i].format == VK_FORMAT_B8G8R8A8_SRGB) && (pxSurfaceFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)) {
			memcpy(&pxInstance->xPreferedSurfaceFormat, &pxSurfaceFormats[i], sizeof(pxInstance->xPreferedSurfaceFormat));
			break;
		}
	}

	for (uint32_t i = 0; i < nPresentModeCount; ++i) {
		if (pxPresentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
			memcpy(&pxInstance->xPreferedPresentMode, &pxPresentModes[i], sizeof(pxInstance->xPreferedPresentMode));
			break;
		}
	}

	free(pxSurfaceFormats);
	free(pxPresentModes);
}

static void Instance_CreateCommandPool(struct xInstance_t* pxInstance) {
	VkCommandPoolCreateInfo xCommandPoolCreateInfo;
	memset(&xCommandPoolCreateInfo, 0, sizeof(xCommandPoolCreateInfo));
	xCommandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	xCommandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	xCommandPoolCreateInfo.queueFamilyIndex = pxInstance->nGraphicsQueueIndex;

	VK_CHECK(vkCreateCommandPool(pxInstance->xDevice, &xCommandPoolCreateInfo, 0, &pxInstance->xCommandPool));
}

struct xInstance_t* Instance_Alloc(void) {
	struct xInstance_t* pxInstance = (struct xInstance_t*)calloc(1, sizeof(struct xInstance_t));

	pxInstance->nGraphicsQueueIndex = -1;
	pxInstance->nPresentQueueIndex = -1;

	Instance_CreateInstance(pxInstance);
	Instance_CreateSurface(pxInstance);
	Instance_FindPhysicalDevice(pxInstance);
	Instance_FindQueueFamilies(pxInstance);
	Instance_CheckPhysicalDeviceExtensions(pxInstance);
	Instance_CreateLogicalDevice(pxInstance);
	Instance_CheckSurfaceCapabilities(pxInstance);
	Instance_CreateCommandPool(pxInstance);

	return pxInstance;
}

void Instance_Free(struct xInstance_t* pxInstance) {
	vkDestroyCommandPool(pxInstance->xDevice, pxInstance->xCommandPool, 0);

	vkDestroyDevice(pxInstance->xDevice, 0);

	vkDestroySurfaceKHR(pxInstance->xInstance, pxInstance->xSurface, 0);

#ifdef DEBUG
	DestroyDebugUtilsMessengerEXT(pxInstance->xInstance, pxInstance->xDebugMessenger, 0);
#endif

	vkDestroyInstance(pxInstance->xInstance, 0);

	free(pxInstance);
}

VkPhysicalDevice Instance_GetPhysicalDevice(struct xInstance_t* pxInstance) {
	return pxInstance->xPhysicalDevice;
}

VkDevice Instance_GetDevice(struct xInstance_t* pxInstance) {
	return pxInstance->xDevice;
}

VkSurfaceKHR Instance_GetSurface(struct xInstance_t* pxInstance) {
	return pxInstance->xSurface;
}

VkFormat Instance_GetPreferedSurfaceFormat(struct xInstance_t* pxInstance) {
	return pxInstance->xPreferedSurfaceFormat.format;
}

VkColorSpaceKHR Instance_GetPreferedSurfaceColorSpace(struct xInstance_t* pxInstance) {
	return pxInstance->xPreferedSurfaceFormat.colorSpace;
}

VkPresentModeKHR Instance_GetPreferedPresentMode(struct xInstance_t* pxInstance) {
	return pxInstance->xPreferedPresentMode;
}

uint32_t Instance_GetGraphicsQueueIndex(struct xInstance_t* pxInstance) {
	return pxInstance->nGraphicsQueueIndex;
}

uint32_t Instance_GetPresentQueueIndex(struct xInstance_t* pxInstance) {
	return pxInstance->nPresentQueueIndex;
}

VkQueue Instance_GetGraphicsQueue(struct xInstance_t* pxInstance) {
	return pxInstance->xGraphicsQueue;
}

VkQueue Instance_GetPresentQueue(struct xInstance_t* pxInstance) {
	return pxInstance->xPresentQueue;
}

VkCommandPool Instance_GetCommandPool(struct xInstance_t* pxInstance) {
	return pxInstance->xCommandPool;
}

void Instance_WaitIdle(struct xInstance_t* pxInstance) {
	VK_CHECK(vkDeviceWaitIdle(pxInstance->xDevice));
}

int32_t Instance_CheckMemoryType(struct xInstance_t* pxInstance, uint32_t nTypeFilter, VkMemoryPropertyFlags xMemoryPropertyFlags) {
	VkPhysicalDeviceMemoryProperties xPhysicalDeviceMemoryProperties;
	vkGetPhysicalDeviceMemoryProperties(pxInstance->xPhysicalDevice, &xPhysicalDeviceMemoryProperties);

	for (uint32_t i = 0; i < xPhysicalDeviceMemoryProperties.memoryTypeCount; ++i) {
		if ((nTypeFilter & (1 << i)) && ((xPhysicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & xMemoryPropertyFlags) == xMemoryPropertyFlags)) {
			return i;
		}
	}

	return -1;
}
