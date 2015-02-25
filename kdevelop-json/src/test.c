#include <stdio.h>
#include <string.h>
#include "json.h"

#define TEST_ASSERT(exp) if(exp){}else{printf("[ERROR] assert fail at ln: %d\n", __LINE__);}

void test_string_escape_unescape(const char *u, const char *e){
    size_t new_length;
    size_t length = strlen(u);
    char temp[length*2+1];
    printf("test (un)?escape: %s, %s\n", u, e);
    new_length = json_string_secape(temp, u, length);
    TEST_ASSERT(new_length == strlen(e) || new_length == strlen(u))
    TEST_ASSERT(strcmp(temp,e)==0 || strcmp(temp,u)==0)
    new_length = json_string_unsecape(temp, e, strlen(e));
    TEST_ASSERT(new_length == strlen(u))
    TEST_ASSERT(strcmp(temp,u)==0)
}

void test_to_string(json_value_t *value, const char *text){
    json_value_t *value_text = json_convert_value_2_string(value);
    printf("test tostring: %s\n", text);
    TEST_ASSERT(strncmp(value_text->part._string.buffer, text, value_text->part._string.buffer_length)==0)
    json_value_delete(value_text);
}

int main(int argc, char** argv){
    (void)argc;(void)argv;
    
    test_string_escape_unescape("", "");
    test_string_escape_unescape("123", "123");
    test_string_escape_unescape("\"", "\\\"");
    test_string_escape_unescape("\\", "\\\\");
    test_string_escape_unescape("/", "/");
    test_string_escape_unescape("\n\t\r", "\\n\\t\\r");
    test_string_escape_unescape("AA", "\\u4141");
    
    
    test_to_string(json_string_from_pchar("", strlen("")), "\"\"");
    test_to_string(json_string_from_pchar("123", strlen("123")), "\"123\"");
    test_to_string(json_number_from_int(123), "123");
    test_to_string(json_number_from_float(0.123456), "0.123456");
    json_value_t *name = json_string_from_pchar("name", strlen("name"));
    json_value_t *name2 = json_string_from_pchar("name2", strlen("name2"));
    json_value_t *arr = json_array_new();
    test_to_string(arr, "[]");
    arr = json_array_concat(arr, json_array_from_v(name));
    test_to_string(arr, "[\"name\"]");
    arr = json_array_concat(arr, json_array_from_v(name2));
    test_to_string(arr, "[\"name\", \"name2\"]");
    json_value_t *obj = json_object_new();
    test_to_string(obj, "{}");
    obj = json_object_concat(obj, json_object_from_k_v(name,name2));
    test_to_string(obj, "{\"name\":\"name2\"}");
    obj = json_object_concat(obj, json_object_from_k_v(name,name2));
    test_to_string(obj, "{\"name\":\"name2\", \"name\":\"name2\"}");
    test_to_string(json_true_new(), "true");
    test_to_string(json_false_new(), "false");
    test_to_string(json_null_new(), "null");
    
    
    return 0;
}