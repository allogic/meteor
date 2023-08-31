cmake_minimum_required(VERSION 3.12)

project(vulkan VERSION 0.0.1 LANGUAGES C)

include("../../cmake/common.cmake")
include("../../cmake/vulkan.cmake")

list(APPEND SOURCES "${PROJECT_DIR}/main.c")

list(APPEND SOURCES "${COMMON_DIR}/timer.c")

list(APPEND SOURCES "${DEBUG_DIR}/stacktrace.c")

list(APPEND SOURCES "${FILESYSTEM_DIR}/fileutil.c")

list(APPEND SOURCES "${MATH_DIR}/vector.c")
list(APPEND SOURCES "${MATH_DIR}/matrix.c")
list(APPEND SOURCES "${MATH_DIR}/orthographic.c")
list(APPEND SOURCES "${MATH_DIR}/view.c")

list(APPEND SOURCES "${PLATFORM_DIR}/nativewindow.c")

list(APPEND SOURCES "${VULKAN_DIR}/vkinstance.c")
list(APPEND SOURCES "${VULKAN_DIR}/vkswapchain.c")
list(APPEND SOURCES "${VULKAN_DIR}/vkshader.c")
list(APPEND SOURCES "${VULKAN_DIR}/vkrenderer.c")
list(APPEND SOURCES "${VULKAN_DIR}/vkbuffer.c")
list(APPEND SOURCES "${VULKAN_DIR}/vkbuffervariance.c")
list(APPEND SOURCES "${VULKAN_DIR}/vkimage.c")
list(APPEND SOURCES "${VULKAN_DIR}/vkimagevariance.c")
list(APPEND SOURCES "${VULKAN_DIR}/vkcommand.c")

add_executable(${PROJECT_NAME} ${SOURCES})
