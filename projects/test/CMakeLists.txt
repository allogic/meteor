cmake_minimum_required(VERSION 3.12)

project(test VERSION 0.0.1 LANGUAGES C)

include("../../cmake/common.cmake")

list(APPEND SOURCES "${PROJECT_DIR}/main.c")

list(APPEND SOURCES "${COMMON_DIR}/stringutil.c")

list(APPEND SOURCES "${CONTAINER_DIR}/list.c")
list(APPEND SOURCES "${CONTAINER_DIR}/vector.c")
list(APPEND SOURCES "${CONTAINER_DIR}/queue.c")
list(APPEND SOURCES "${CONTAINER_DIR}/set.c")

list(APPEND SOURCES "${DEBUG_DIR}/stacktrace.c")

list(APPEND SOURCES "${FILESYSTEM_DIR}/filelist.c")
list(APPEND SOURCES "${FILESYSTEM_DIR}/fileutil.c")
list(APPEND SOURCES "${FILESYSTEM_DIR}/path.c")

add_executable(${PROJECT_NAME} ${SOURCES})
