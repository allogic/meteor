#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "hardfault.h"
#include "macros.h"
#include "window.h"
#include "vulkan.h"
#include "strutl.h"

int32_t main(void) {

#ifdef DEBUG
	Hardfault_Alloc();
#endif

	struct xWindow_t* pxWindow = Window_Alloc(WINDOW_NAME, 800, 600);

	if (pxWindow) {

		if (Vulkan_Alloc(pxWindow)) {

			while (Window_ShouldNotClose(pxWindow)) {
				Window_PollEvents(pxWindow);
				Window_SwapBuffers(pxWindow);
			}

			Vulkan_Free();

		}

		Window_Free(pxWindow);
	}

#ifdef DEBUG
	Hardfault_Free();
#endif

	return 0;
}
