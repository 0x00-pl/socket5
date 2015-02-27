#ifndef _STRING_BUILDER_H_
#define _STRING_BUILDER_H_

#include <stdlib.h>
#include <string.h>


typedef struct string_builder_decl{
    char *buffer;
    size_t buffer_length;
    struct string_builder_decl *cache_head;
} string_builder_t;


string_builder_t *string_builder_new();
string_builder_t *string_builder_from_pchar(const char* text);
void string_builder_delete(string_builder_t *string_builder);
string_builder_t *string_builder_append(string_builder_t *string_builder, const char* text);
char string_builder_ch(string_builder_t *string_builder, size_t index);
char *string_builder_c_str(string_builder_t *string_builder);


#endif