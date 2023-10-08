if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
	add_compile_options("-std=c99")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	add_compile_options("-std=gnu99")
endif()

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
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
	add_compile_definitions("OS_WINDOWS")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	add_compile_definitions("OS_LINUX")

	add_link_options("-lm")
	add_link_options("-lrt")
endif()

set(PROJECT_DIR ".")
set(PARENT_DIR "..")
set(LIBRARY_DIR "../../library")
set(COMMON_DIR "../../library/common")
set(DEBUG_DIR "../../library/debug")
set(ECS_DIR "../../library/ecs")
set(FILESYSTEM_DIR "../../library/filesystem")
set(MATH_DIR "../../library/math")
set(CONTAINER_DIR "../../library/container")
set(PLATFORM_DIR "../../library/platform")
set(RANDOM_DIR "../../library/random")
set(VULKAN_DIR "../../library/vulkan")

include_directories(${PROJECT_DIR})
include_directories(${PARENT_DIR})
include_directories(${LIBRARY_DIR})

add_compile_definitions("_CRT_SECURE_NO_WARNINGS")

add_compile_definitions("PROJECT_NAME=\"${PROJECT_NAME}\"")

add_compile_definitions("VER_MAJOR=\"${PROJECT_VERSION_MAJOR}\"")
add_compile_definitions("VER_MINOR=\"${PROJECT_VERSION_MINOR}\"")
add_compile_definitions("VER_PATCH=\"${PROJECT_VERSION_PATCH}\"")
