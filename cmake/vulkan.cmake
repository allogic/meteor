find_package(Vulkan REQUIRED)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
	add_link_options("-lUser32")
	add_link_options("-lGdi32") # TODO
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	add_link_options("-lwayland-client")

	list(APPEND SOURCES "${PLATFORM_DIR}/xdgshell.c")
endif()

include_directories(${Vulkan_INCLUDE_DIRS})

add_link_options("-l${Vulkan_LIBRARIES}")

add_compile_definitions("WIN32_CLASS_NAME=\"${PROJECT_NAME}_win32_class\"")
add_compile_definitions("WINDOW_NAME=\"${PROJECT_NAME} ${PROJECT_VERSION}\"")

add_compile_definitions("APPLICATION_NAME=\"${PROJECT_NAME}\"")
add_compile_definitions("ENGINE_NAME=\"${PROJECT_NAME}\"")