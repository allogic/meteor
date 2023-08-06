add_compile_options("-std=c99")

add_compile_options("-Wpedantic")
add_compile_options("-Wall")
add_compile_options("-Wextra")

add_compile_options("-Wno-visibility")

if(CMAKE_BUILD_TYPE STREQUAL "debug")
	add_compile_options("-g")
	add_compile_options("-O0")

	add_compile_definitions("DEBUG")
elseif(CMAKE_BUILD_TYPE STREQUAL "release")
	add_compile_options("-O2")
	add_compile_options("-flto")
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
	add_compile_definitions("OS_WINDOWS")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	add_compile_definitions("OS_LINUX")
endif()

set(PROJECT_DIR .)
set(LIBRARY_DIR ../library)
set(PLATFORM_DIR ../library/platform)

include_directories(${PROJECT_DIR})
include_directories(${LIBRARY_DIR})
include_directories(${PLATFORM_DIR})

add_compile_definitions("_CRT_SECURE_NO_WARNINGS")
