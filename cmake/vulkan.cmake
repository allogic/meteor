if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
	add_link_options("-lUser32")
	add_link_options("-lGdi32") # TODO

	include_directories("C:/vulkan/include")

	add_link_options("-lC:/vulkan/lib/vulkan1")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	add_link_options("-lwayland-client")

	include_directories("/opt/vulkan/include")

	add_link_options("-L/opt/vulkan/lib")

	add_link_options("-lvku")
	add_link_options("-lvolk")
	add_link_options("-lVkLayer_utils")
	add_link_options("-lVulkanLayerUtils")

	list(APPEND SOURCES "${PLATFORM_DIR}/xdgshell.c")
endif()

add_compile_definitions("WIN32_CLASS_NAME=\"${PROJECT_NAME}_win32_class\"")
add_compile_definitions("WINDOW_NAME=\"${PROJECT_NAME} ${PROJECT_VERSION}\"")

add_compile_definitions("APPLICATION_NAME=\"${PROJECT_NAME}\"")
add_compile_definitions("ENGINE_NAME=\"${PROJECT_NAME}\"")