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

static VkPhysicalDevice s_xPhysicalDevice;
static VkDevice s_xDevice;
static VkQueue s_xGraphicsQueue;
static VkSurfaceKHR s_xSurface;

static const char* s_apExtensions[] = {
	"VK_KHR_surface",
#ifdef DEBUG
	"VK_EXT_debug_utils",
#endif
#ifdef OS_WINDOWS
	"VK_KHR_win32_surface",
#endif
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
	VkApplicationInfo xAppInfo;
	memset(&xAppInfo, 0, sizeof(xAppInfo));
	xAppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	xAppInfo.pApplicationName = "Hello Triangle";
	xAppInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	xAppInfo.pEngineName = "No Engine";
	xAppInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	xAppInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo xInstanceCreateInfo;
	memset(&xInstanceCreateInfo, 0, sizeof(xInstanceCreateInfo));
	xInstanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	xInstanceCreateInfo.pApplicationInfo = &xAppInfo;
	xInstanceCreateInfo.enabledExtensionCount = ARRAY_LENGTH(s_apExtensions);
	xInstanceCreateInfo.ppEnabledExtensionNames = s_apExtensions;
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

	uint32_t nQueueFamilyCount;
	vkGetPhysicalDeviceQueueFamilyProperties(s_xPhysicalDevice, &nQueueFamilyCount, 0);
	if (nQueueFamilyCount == 0) {
		printf("Failed finding queue family properties\n");
		return false;
	}

	VkQueueFamilyProperties* pxQueueFamilyProperties = (VkQueueFamilyProperties*)malloc(sizeof(VkQueueFamilyProperties) * nQueueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(s_xPhysicalDevice, &nQueueFamilyCount, pxQueueFamilyProperties);

	int32_t nGraphicsFamily = -1;

	for (uint32_t i = 0; i < nQueueFamilyCount; ++i) {
		if (pxQueueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			nGraphicsFamily = i;
			break;
		}
	}

	if (nGraphicsFamily == -1) {
		printf("Failed to find graphics family\n");
		return false;
	}

	free(pxQueueFamilyProperties);

	VkDeviceQueueCreateInfo xQueueCreateInfo;
	memset(&xQueueCreateInfo, 0, sizeof(xQueueCreateInfo));
	xQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	xQueueCreateInfo.queueFamilyIndex = nGraphicsFamily;
	xQueueCreateInfo.queueCount = 1;

	float fQueuePriority = 1.0f;
	xQueueCreateInfo.pQueuePriorities = &fQueuePriority;

	VkPhysicalDeviceFeatures xPhysicalDeviceFeatures;
	memset(&xPhysicalDeviceFeatures, 0, sizeof(xPhysicalDeviceFeatures));

	VkDeviceCreateInfo xDeviceCreateInfo;
	memset(&xDeviceCreateInfo, 0, sizeof(xDeviceCreateInfo));
	xDeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	xDeviceCreateInfo.pQueueCreateInfos = &xQueueCreateInfo;
	xDeviceCreateInfo.queueCreateInfoCount = 1;
	xDeviceCreateInfo.pEnabledFeatures = &xPhysicalDeviceFeatures;
	xDeviceCreateInfo.enabledExtensionCount = 0;
#ifdef DEBUG
	xDeviceCreateInfo.enabledLayerCount = ARRAY_LENGTH(s_apValidationLayers);
	xDeviceCreateInfo.ppEnabledLayerNames = s_apValidationLayers;
#endif

	if (vkCreateDevice(s_xPhysicalDevice, &xDeviceCreateInfo, 0, &s_xDevice) != VK_SUCCESS) {
		printf("Failed to create logical device");
		return false;
	}

	vkGetDeviceQueue(s_xDevice, nGraphicsFamily, 0, &s_xGraphicsQueue);

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

	return true;
}

void Vulkan_Free(void) {
	vkDestroyDevice(s_xDevice, 0);

#ifdef DEBUG
	DestroyDebugUtilsMessengerEXT(s_xInstance, s_xDebugMessenger, 0);
#endif

	vkDestroyInstance(s_xInstance, 0);
}
