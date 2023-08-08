find_package(Vulkan REQUIRED FATAL_ERROR)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
	add_link_options("-lUser32")
	add_link_options("-lGdi32") # TODO
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	add_link_options("-lwayland-client")

	list(APPEND SOURCES "${PLATFORM_DIR}/xdgshell.c")
endif()

message("FUUUUUUUUUUU ${Vulkan_LIBRARIES}")
message("FUUUUUUUUUUU ${Vulkan_LIBRARY}")
message("FUUUUUUUUUUU ${Vulkan_INCLUDE_DIRS}")

include_directories(${Vulkan_INCLUDE_DIRS})
