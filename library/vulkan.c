#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"
#include "vulkan.h"
#include "window.h"

#include <vulkan/vulkan.h>

#ifdef OS_WINDOWS
#	include <windows.h>
#	include <vulkan/vulkan_win32.h>
#endif

#ifdef OS_LINUX
#	include <wayland-client.h>
#	include <vulkan/vulkan_wayland.h>
#endif

static VkInstance s_xInstance;

#ifdef DEBUG
static VkDebugUtilsMessengerEXT s_xDebugMessenger;
#endif

static VkSurfaceKHR s_xSurface;
static VkPhysicalDevice s_xPhysicalDevice;
static VkDevice s_xDevice;
static VkQueue s_xGraphicsQueue;
static VkQueue s_xPresentQueue;

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

bool Vulkan_Alloc(struct xWindow_t* pxWindow) {

///////////////////////////////////////////////////////////////////////////////
// Create Instance And Setup Debug Messenger
///////////////////////////////////////////////////////////////////////////////

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

	if (vkCreateInstance(&xInstanceCreateInfo, 0, &s_xInstance) != VK_SUCCESS) {
		printf("Failed creating vulkan instance\n");
		return false;
	}

#ifdef DEBUG
	if (CreateDebugUtilsMessengerEXT(s_xInstance, &xDebugCreateInfo, 0, &s_xDebugMessenger) != VK_SUCCESS) {
		printf("Failed setting up vulkan debug messenger\n");
		return false;
	}
#endif

///////////////////////////////////////////////////////////////////////////////
// Create Surface
///////////////////////////////////////////////////////////////////////////////

#ifdef OS_WINDOWS
	VkWin32SurfaceCreateInfoKHR xSurfaceCreateInfo;
	memset(&xSurfaceCreateInfo, 0, sizeof(xSurfaceCreateInfo));
	xSurfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	xSurfaceCreateInfo.hwnd = Window_GetWindowHandle(pxWindow);
	xSurfaceCreateInfo.hinstance = Window_GetModuleHandle(pxWindow);

	if (vkCreateWin32SurfaceKHR(s_xInstance, &xSurfaceCreateInfo, 0, &s_xSurface) != VK_SUCCESS) {
		printf("failed to create window surface\n");
		return false;
	}
#endif

///////////////////////////////////////////////////////////////////////////////
// Find Suitable Physical Device
///////////////////////////////////////////////////////////////////////////////

	uint32_t nPhysicalDeviceCount;
	vkEnumeratePhysicalDevices(s_xInstance, &nPhysicalDeviceCount, 0);
	if (nPhysicalDeviceCount == 0) {
		printf("Failed finding physical devices\n");
		return false;
	}

	VkPhysicalDevice* pxPhysicalDevices = (VkPhysicalDevice*)malloc(sizeof(VkPhysicalDevice) * nPhysicalDeviceCount);
	vkEnumeratePhysicalDevices(s_xInstance, &nPhysicalDeviceCount, pxPhysicalDevices);

	for (uint32_t i = 0; i < nPhysicalDeviceCount; ++i) {
		VkPhysicalDeviceProperties xPhysicalDeviceProperties;
		VkPhysicalDeviceFeatures xPhysicalDeviceFeatures;

		vkGetPhysicalDeviceProperties(pxPhysicalDevices[i], &xPhysicalDeviceProperties);
		vkGetPhysicalDeviceFeatures(pxPhysicalDevices[i], &xPhysicalDeviceFeatures);

		if (xPhysicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
			if (xPhysicalDeviceFeatures.geometryShader) {
				memcpy(&s_xPhysicalDevice, &pxPhysicalDevices[i], sizeof(s_xPhysicalDevice));
				break;
			}
		}
	}

	free(pxPhysicalDevices);

	if (s_xPhysicalDevice == 0) {
		printf("Failed finding suitable physical device\n");
		return false;
	}

///////////////////////////////////////////////////////////////////////////////
// Find Suitable Queue Families
///////////////////////////////////////////////////////////////////////////////

	uint32_t nQueueFamilyCount;
	vkGetPhysicalDeviceQueueFamilyProperties(s_xPhysicalDevice, &nQueueFamilyCount, 0);
	if (nQueueFamilyCount == 0) {
		printf("Failed finding queue family properties\n");
		return false;
	}

	VkQueueFamilyProperties* pxQueueFamilyProperties = (VkQueueFamilyProperties*)malloc(sizeof(VkQueueFamilyProperties) * nQueueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(s_xPhysicalDevice, &nQueueFamilyCount, pxQueueFamilyProperties);

	int32_t nGraphicsQueueIndex = -1;
	int32_t nPresentQueueIndex = -1;

	for (uint32_t i = 0; i < nQueueFamilyCount; ++i) {
		uint32_t nGraphicsSupport = 0;
		uint32_t nPresentSupport = 0;

		nGraphicsSupport = pxQueueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT;

		vkGetPhysicalDeviceSurfaceSupportKHR(s_xPhysicalDevice, i, s_xSurface, &nPresentSupport);

		if (nGraphicsSupport && (nGraphicsQueueIndex == -1)) {
			nGraphicsQueueIndex = i;
		} else if (nPresentSupport && (nPresentQueueIndex == -1)) {
			nPresentQueueIndex = i;
		}

		if ((nGraphicsQueueIndex != -1) && (nPresentQueueIndex != -1)) {
			break;
		}
	}

	if (nGraphicsQueueIndex == -1) {
		printf("Failed to find graphics family\n");
		return false;
	}

	if (nPresentQueueIndex == -1) {
		printf("Failed to find present support\n");
		return false;
	}

	free(pxQueueFamilyProperties);

///////////////////////////////////////////////////////////////////////////////
// Check Physical Device Extensions
///////////////////////////////////////////////////////////////////////////////

	uint32_t nAvailableDeviceExtensionCount;
	vkEnumerateDeviceExtensionProperties(s_xPhysicalDevice, 0, &nAvailableDeviceExtensionCount, 0);

	VkExtensionProperties* pxAvailableDeviceExtensions = (VkExtensionProperties*)malloc(sizeof(VkExtensionProperties) * nAvailableDeviceExtensionCount);
	vkEnumerateDeviceExtensionProperties(s_xPhysicalDevice, 0, &nAvailableDeviceExtensionCount, pxAvailableDeviceExtensions);

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

#ifdef DEBUG
			if (j == (nAvailableDeviceExtensionCount - 1)) {
				printf("Missing %s\n", s_apDeviceExtensions[i]);
			}
#endif
		}

		if (!nDeviceExtensionsAvailable) {
			nRequiredDeviceExtensionsAvailable = false;
			break;
		}
	}

	if (!nRequiredDeviceExtensionsAvailable) {
		printf("Failed to find required device extensions\n");
		return false;
	}

	free(pxAvailableDeviceExtensions);

///////////////////////////////////////////////////////////////////////////////
// Check Swap Chain Capabilities
///////////////////////////////////////////////////////////////////////////////

	VkSurfaceCapabilitiesKHR xSurfaceCapabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(s_xPhysicalDevice, s_xSurface, &xSurfaceCapabilities);

	uint32_t nSurfaceFormatCount;
	uint32_t nPresentModeCount;

	vkGetPhysicalDeviceSurfaceFormatsKHR(s_xPhysicalDevice, s_xSurface, &nSurfaceFormatCount, 0);
	vkGetPhysicalDeviceSurfacePresentModesKHR(s_xPhysicalDevice, s_xSurface, &nPresentModeCount, 0);

	VkSurfaceFormatKHR* pxSurfaceFormats = (VkSurfaceFormatKHR*)malloc(sizeof(VkSurfaceFormatKHR) * nSurfaceFormatCount);
	VkPresentModeKHR* pxPresentModes = (VkPresentModeKHR*)malloc(sizeof(VkPresentModeKHR) * nPresentModeCount);

	vkGetPhysicalDeviceSurfaceFormatsKHR(s_xPhysicalDevice, s_xSurface, &nSurfaceFormatCount, pxSurfaceFormats);
	vkGetPhysicalDeviceSurfacePresentModesKHR(s_xPhysicalDevice, s_xSurface, &nPresentModeCount, pxPresentModes);

	uint32_t nSwapChainAdequate = (nSurfaceFormatCount > 0) && (nPresentModeCount > 0);

	if (nSwapChainAdequate == 0) {
		printf("Failed swap chain capabilities check\n");
		return false;
	}

	bool bPreferedSurfaceFormatAvailable;
	bool bPreferedPresentModeAvailable;

	VkSurfaceFormatKHR xPreferedSurfaceFormat;
	VkPresentModeKHR xPreferedPresentMode;

	for (uint32_t i = 0; i < nSurfaceFormatCount; ++i) {
		if ((pxSurfaceFormats[i].format == VK_FORMAT_B8G8R8A8_SRGB) && (pxSurfaceFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)) {
			memcpy(&xPreferedSurfaceFormat, &pxSurfaceFormats[i], sizeof(xPreferedSurfaceFormat));
			bPreferedSurfaceFormatAvailable = true;
		}
	}

	for (uint32_t i = 0; i < nPresentModeCount; ++i) {
		if (pxPresentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
			memcpy(&xPreferedPresentMode, &pxPresentModes[i], sizeof(xPreferedPresentMode));
			bPreferedPresentModeAvailable = true;
		}
	}

	if (!bPreferedSurfaceFormatAvailable) {
		printf("Failed finding prefered surface format\n");
		return false;
	}

	if (!bPreferedPresentModeAvailable) {
		printf("Failed finding prefered present mode\n");
		return false;
	}

	VkExtent2D xSwapChainExtent;
	memset(&xSwapChainExtent, 0, sizeof(xSwapChainExtent));
	xSwapChainExtent.width = Window_GetWidth(pxWindow);
	xSwapChainExtent.height = Window_GetHeight(pxWindow);

	free(pxSurfaceFormats);
	free(pxPresentModes);

///////////////////////////////////////////////////////////////////////////////
// Create Swap Chain
///////////////////////////////////////////////////////////////////////////////

	uint32_t nImageCount = xSurfaceCapabilities.minImageCount + 1;

	VkSwapchainCreateInfoKHR xSwapChaincreateInfo;
	memset(&xSwapChaincreateInfo, 0, sizeof(xSwapChaincreateInfo));
	xSwapChaincreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	xSwapChaincreateInfo.surface = s_xSurface;
	xSwapChaincreateInfo.minImageCount = nImageCount;
	xSwapChaincreateInfo.imageFormat = xPreferedSurfaceFormat.format;
	xSwapChaincreateInfo.imageColorSpace = xPreferedSurfaceFormat.colorSpace;
	xSwapChaincreateInfo.imageExtent = xSwapChainExtent;
	xSwapChaincreateInfo.imageArrayLayers = 1;
	xSwapChaincreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

///////////////////////////////////////////////////////////////////////////////
// Create Logical Device
///////////////////////////////////////////////////////////////////////////////

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

	if (vkCreateDevice(s_xPhysicalDevice, &xDeviceCreateInfo, 0, &s_xDevice) != VK_SUCCESS) {
		printf("Failed to create logical device");
		return false;
	}

	vkGetDeviceQueue(s_xDevice, nGraphicsQueueIndex, 0, &s_xGraphicsQueue);
	vkGetDeviceQueue(s_xDevice, nPresentQueueIndex, 0, &s_xPresentQueue);

	return true;
}

void Vulkan_Free(void) {
	vkDestroyDevice(s_xDevice, 0);

	vkDestroySurfaceKHR(s_xInstance, s_xSurface, 0);

#ifdef DEBUG
	DestroyDebugUtilsMessengerEXT(s_xInstance, s_xDebugMessenger, 0);
#endif

	vkDestroyInstance(s_xInstance, 0);
}
