cmake_minimum_required(VERSION 2.8)
project(kdevelop-json)

SET(CMAKE_C_FLAGS "-g -Wall -Wextra -Wformat=2 -Wstrict-aliasing=2 -Wcast-align -Wwrite-strings -Wformat-nonliteral -Wconversion -Wfloat-equal -Wpointer-arith -Wswitch-enum" )

SET(SOURCES src/json.c)


message(STATUS "copy newest string_builder")
SET(STRING_BUILDER_SRC_DIR "${PROJECT_SOURCE_DIR}/../kdevelop-stringbuilder/src")
exec_program(mkdir ARGS "-p" "${PROJECT_SOURCE_DIR}/src/remote")
exec_program(cp ARGS "${STRING_BUILDER_SRC_DIR}/string_builder.*" "${PROJECT_SOURCE_DIR}/src/remote")


AUX_SOURCE_DIRECTORY("${PROJECT_SOURCE_DIR}/src/remote" REMOTE_SOURCES)

add_executable(kdevelop-json-test src/test.c ${SOURCES} ${REMOTE_SOURCES})

