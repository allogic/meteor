#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <hardfault.h>
#include <macros.h>
#include <strutl.h>

#include <platform/window.h>

#include <graphics/vulkan.h>

int32_t main(void) {

#ifdef DEBUG
	Hardfault_Alloc();
#endif

	struct xWindow_t* pxWindow = Window_Alloc(WINDOW_NAME, 800, 600);
	if (pxWindow) {

		struct xVulkan_t* pxVulkan = Vulkan_Alloc(pxWindow);
		if (pxVulkan) {

			while (Window_ShouldNotClose(pxWindow)) {
				Window_PollEvents(pxWindow);
				Window_SwapBuffers(pxWindow);
			}

			Vulkan_Free(pxVulkan);
		}

		Window_Free(pxWindow);
	}

#ifdef DEBUG
	Hardfault_Free();
#endif

	return 0;
}
