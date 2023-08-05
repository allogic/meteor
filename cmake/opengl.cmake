if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
	add_link_options("-lUser32")
	add_link_options("-lGdi32")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	add_link_options("-lwayland-client")
	add_link_options("-lwayland-egl")
	add_link_options("-lEGL")

	list(APPEND SOURCES "${PLATFORM_DIR}/xdgshell.c")
endif()

list(APPEND SOURCES "${PLATFORM_DIR}/glad.c")