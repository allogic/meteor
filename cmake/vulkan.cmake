find_package(Vulkan REQUIRED)

include_directories(${Vulkan_INCLUDE_DIRS})

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
	add_link_options("-lUser32")

	add_link_options("-lvulkan-1")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	add_link_options("-lwayland-client")

	include_directories("/opt/vulkan/include")

	add_link_options("-lvulkan")

	list(APPEND SOURCES "${PLATFORM_DIR}/xdgshell.c")
endif()

add_compile_definitions("WIN32_CLASS_NAME=\"${PROJECT_NAME}_win32_class\"")
add_compile_definitions("WINDOW_NAME=\"${PROJECT_NAME} ${PROJECT_VERSION}\"")

add_compile_definitions("APPLICATION_NAME=\"${PROJECT_NAME}\"")
add_compile_definitions("ENGINE_NAME=\"${PROJECT_NAME}\"")