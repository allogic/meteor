cmake_minimum_required(VERSION 3.12)

project(game VERSION 0.0.1 LANGUAGES C)

include("../../cmake/common.cmake")
include("../../cmake/vulkan.cmake")

list(APPEND SOURCES "${PROJECT_DIR}/main.c")
list(APPEND SOURCES "${PROJECT_DIR}/renderer.c")
list(APPEND SOURCES "${PROJECT_DIR}/entity.c")
list(APPEND SOURCES "${PROJECT_DIR}/scene.c")
list(APPEND SOURCES "${PROJECT_DIR}/assets.c")

list(APPEND SOURCES "${COMMON_DIR}/timer.c")

list(APPEND SOURCES "${DEBUG_DIR}/stacktrace.c")

list(APPEND SOURCES "${FILESYSTEM_DIR}/fileutil.c")

list(APPEND SOURCES "${STANDARD_DIR}/list.c")

list(APPEND SOURCES "${MATH_DIR}/vector.c")
list(APPEND SOURCES "${MATH_DIR}/matrix.c")
list(APPEND SOURCES "${MATH_DIR}/orthographic.c")
list(APPEND SOURCES "${MATH_DIR}/view.c")

list(APPEND SOURCES "${PLATFORM_DIR}/nativewindow.c")

list(APPEND SOURCES "${RANDOM_DIR}/xorshift128.c")

list(APPEND SOURCES "${VULKAN_DIR}/instance.c")
list(APPEND SOURCES "${VULKAN_DIR}/swapchain.c")
list(APPEND SOURCES "${VULKAN_DIR}/shader.c")
list(APPEND SOURCES "${VULKAN_DIR}/buffer.c")
list(APPEND SOURCES "${VULKAN_DIR}/buffervariance.c")
list(APPEND SOURCES "${VULKAN_DIR}/image.c")
list(APPEND SOURCES "${VULKAN_DIR}/imagevariance.c")
list(APPEND SOURCES "${VULKAN_DIR}/command.c")
list(APPEND SOURCES "${VULKAN_DIR}/computepipeline.c")
list(APPEND SOURCES "${VULKAN_DIR}/graphicpipeline.c")
list(APPEND SOURCES "${VULKAN_DIR}/vertex.c")

add_executable(${PROJECT_NAME} ${SOURCES})
