cmake_minimum_required(VERSION 3.12)

project(viewer VERSION 0.0.1 LANGUAGES C)

include("../cmake/common.cmake")
include("../cmake/version.cmake")
include("../cmake/opengl.cmake")

list(APPEND SOURCES "${PROJECT_DIR}/main.c")

list(APPEND SOURCES "${LIBRARY_DIR}/debug.c")
list(APPEND SOURCES "${LIBRARY_DIR}/window.c")
list(APPEND SOURCES "${LIBRARY_DIR}/list.c")
list(APPEND SOURCES "${LIBRARY_DIR}/filelist.c")
list(APPEND SOURCES "${LIBRARY_DIR}/fileutl.c")
list(APPEND SOURCES "${LIBRARY_DIR}/strutl.c")

list(APPEND SOURCES "${OPENGL_DIR}/shader.c")

add_executable(${PROJECT_NAME} ${SOURCES})
