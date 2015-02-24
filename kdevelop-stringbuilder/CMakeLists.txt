cmake_minimum_required(VERSION 2.8)

project(kdevelop-stringbuilder)

set(CMAKE_C_FLAGS "-Wall -Wextra -Wformat=2 -Wstrict-aliasing=2 -Wcast-align -Wwrite-strings -Wformat-nonliteral -Wconversion -Wfloat-equal -Wpointer-arith -Wswitch-enum")

SET(SOURCES src/string_builder.c)

add_executable(string-builder-test ${SOURCES} src/test.c)
