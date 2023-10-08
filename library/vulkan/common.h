#ifndef METEOR_VULKAN_COMMON_H
#define METEOR_VULKAN_COMMON_H

#define MAX_DESCRIPTOR_TYPE (VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT + 1)

#ifdef DEBUG
#	define VK_CHECK(EXPR) { \
		VkResult result = (EXPR); \
		if (result != VK_SUCCESS) { \
			printf("At %s function %s failed with %X\n", __FILE__, #EXPR, result); \
		} \
	}
#else
#	define VK_CHECK(EXPR) (EXPR)
#endif

#endif
