project(kdevelop-rbtree)

SET(CMAKE_C_FLAGS "-g -Wall -Wextra -Wformat=2 -Wstrict-aliasing=2 -Wcast-align -Wwrite-strings -Wformat-nonliteral -Wconversion -Wfloat-equal -Wpointer-arith -Wswitch-enum" )

SET(SOURCE src/test.c src/rb_tree.c)

add_executable(kdevelop-rbtree-test ${SOURCE})

