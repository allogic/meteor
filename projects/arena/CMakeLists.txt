cmake_minimum_required(VERSION 3.12)

project(arena VERSION 0.0.1 LANGUAGES C)

include("../../cmake/common.cmake")

list(APPEND SOURCES "${PROJECT_DIR}/main.c")

list(APPEND SOURCES "${CONTAINER_DIR}/list.c")
list(APPEND SOURCES "${CONTAINER_DIR}/vector.c")

list(APPEND SOURCES "${DEBUG_DIR}/stacktrace.c")

add_executable(${PROJECT_NAME} ${SOURCES})
