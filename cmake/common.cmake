add_compile_options("-std=c99")

add_compile_options("-Wpedantic")
add_compile_options("-Wall")
add_compile_options("-Wextra")

add_compile_options("-Wno-visibility")
add_compile_options("-Wno-gnu-include-next")

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
set(CLANG_DIR ../llvm/clang/lib/Headers)

include_directories(${PROJECT_DIR})
include_directories(${LIBRARY_DIR})
include_directories(${CLANG_DIR})

add_compile_definitions("_CRT_SECURE_NO_WARNINGS")

add_compile_definitions("PROJECT_NAME=\"${PROJECT_NAME}\"")

add_compile_definitions("VER_MAJOR=\"${PROJECT_VERSION_MAJOR}\"")
add_compile_definitions("VER_MINOR=\"${PROJECT_VERSION_MINOR}\"")
add_compile_definitions("VER_PATCH=\"${PROJECT_VERSION_PATCH}\"")

add_compile_definitions("WIN32_CLASS_NAME=\"${PROJECT_NAME}_win32_class\"")
add_compile_definitions("WINDOW_NAME=\"${PROJECT_NAME} ${PROJECT_VERSION}\"")
