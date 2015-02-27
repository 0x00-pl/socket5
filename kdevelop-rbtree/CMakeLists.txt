cmake_minimum_required(VERSION 2.8)
project(kdevelop-rbtree)


SET(MY_SOURCES src/rb_tree.c)
SET(LIB_NAME "plrbt")


set(PL_INCLUDE "${PROJECT_SOURCE_DIR}/../pl-include")
set(PL_LIB "${PROJECT_SOURCE_DIR}/../pl-lib")
set(LIBRARY_OUTPUT_PATH ${PL_LIB})
include_directories(${PL_INCLUDE})
link_directories(${PL_LIB})


add_definitions(-Wall)
add_definitions(-Wextra)
add_definitions(-Wformat=2)
add_definitions(-Wstrict-aliasing=2)
add_definitions(-Wcast-align)
add_definitions(-Wwrite-strings)
add_definitions(-Wformat-nonliteral)
add_definitions(-Wconversion)
add_definitions(-Wfloat-equal)
add_definitions(-Wpointer-arith)
add_definitions(-Wswitch-enum)

add_executable(${LIB_NAME}_test ${MY_SOURCES} src/test.c)
add_library(${LIB_NAME}_shared SHARED ${MY_SOURCES})
add_library(${LIB_NAME}_static STATIC ${MY_SOURCES})
set_target_properties(${LIB_NAME}_shared PROPERTIES OUTPUT_NAME ${LIB_NAME})
set_target_properties(${LIB_NAME}_static PROPERTIES OUTPUT_NAME ${LIB_NAME})
