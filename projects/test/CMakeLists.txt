cmake_minimum_required(VERSION 3.12)

project(test VERSION 0.0.1 LANGUAGES C)

include("../../cmake/common.cmake")

list(APPEND SOURCES "${PROJECT_DIR}/main.c")

list(APPEND SOURCES "${LIBRARY_DIR}/stacktrace.c")
list(APPEND SOURCES "${LIBRARY_DIR}/list.c")
list(APPEND SOURCES "${LIBRARY_DIR}/filelist.c")
list(APPEND SOURCES "${LIBRARY_DIR}/fileutl.c")
list(APPEND SOURCES "${LIBRARY_DIR}/strutl.c")

add_executable(${PROJECT_NAME} ${SOURCES})
