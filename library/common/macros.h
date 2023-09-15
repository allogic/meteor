#ifndef METEOR_COMMON_MACROS_H
#define METEOR_COMMON_MACROS_H

#define UNUSED(X) ((void)X)

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

#define ARRAY_LENGTH(X) (sizeof(X) / sizeof(X[0]))

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
