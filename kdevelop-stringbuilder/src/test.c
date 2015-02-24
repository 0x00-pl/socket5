#include <stdio.h>
#include "string_builder.h"

#define TEST_ASSERT(exp) if(exp){}else{printf("unpass ln: %d\n", __LINE__);}

int main(int argc, char **argv) {
    (void)argc,(void)argv;
    
    string_builder_t *sb = string_builder_new();
    string_builder_append(sb, "0");
    string_builder_append(sb, "1");
    string_builder_append(sb, "2");
    string_builder_append(sb, "3");
    string_builder_append(sb, "4");
    
    TEST_ASSERT(string_builder_ch(sb, 0) == '0')
    TEST_ASSERT(string_builder_ch(sb, 1) == '1')
    TEST_ASSERT(string_builder_ch(sb, 2) == '2')
    TEST_ASSERT(string_builder_ch(sb, 3) == '3')
    TEST_ASSERT(string_builder_ch(sb, 4) == '4')
    
    string_builder_c_str(sb);
    
    TEST_ASSERT(string_builder_ch(sb, 0) == '0')
    TEST_ASSERT(string_builder_ch(sb, 1) == '1')
    TEST_ASSERT(string_builder_ch(sb, 2) == '2')
    TEST_ASSERT(string_builder_ch(sb, 3) == '3')
    TEST_ASSERT(string_builder_ch(sb, 4) == '4')
    
    string_builder_delete(sb);
    
    return 0;
}
