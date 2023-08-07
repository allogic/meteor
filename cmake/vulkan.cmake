set(VULKAN_INCLUDE_DIR "C:/VulkanSDK/1.3.250.1/Include")
set(VULKAN_LIBRARY_DIR "C:/VulkanSDK/1.3.250.1/Lib")

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
	add_link_options("-lUser32")
	add_link_options("-lGdi32")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	add_link_options("-lwayland-client")
endif()

add_link_options("-l${VULKAN_LIBRARY_DIR}/vulkan-1.lib")

include_directories(${VULKAN_INCLUDE_DIR})
