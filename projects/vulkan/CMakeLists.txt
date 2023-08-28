cmake_minimum_required(VERSION 3.12)

project(vulkan VERSION 0.0.1 LANGUAGES C)

include("../../cmake/common.cmake")
include("../../cmake/vulkan.cmake")

list(APPEND SOURCES "${PROJECT_DIR}/main.c")

list(APPEND SOURCES "${LIBRARY_DIR}/stacktrace.c")
list(APPEND SOURCES "${LIBRARY_DIR}/fileutl.c")

list(APPEND SOURCES "${PLATFORM_DIR}/nativewindow.c")

list(APPEND SOURCES "${VULKAN_DIR}/vkinstance.c")
list(APPEND SOURCES "${VULKAN_DIR}/vkswapchain.c")
list(APPEND SOURCES "${VULKAN_DIR}/vkshader.c")
list(APPEND SOURCES "${VULKAN_DIR}/vkrenderer.c")
list(APPEND SOURCES "${VULKAN_DIR}/vkbuffer.c")

add_executable(${PROJECT_NAME} ${SOURCES})
