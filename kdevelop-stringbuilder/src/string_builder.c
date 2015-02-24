#include "string_builder.h"

string_builder_t *string_builder_new(){
    string_builder_t *ret = (string_builder_t*)malloc(sizeof(string_builder_t));
    ret->buffer_length = 0;
    ret->buffer = NULL;
    ret->cache_head = NULL;
    return ret;
}
string_builder_t *string_builder_from_pchar(const char* text){
    string_builder_t *ret = (string_builder_t*)malloc(sizeof(string_builder_t));
    ret->buffer_length = strlen(text);
    ret->buffer = (char*)malloc((ret->buffer_length+1) * sizeof(char));
    strncpy(ret->buffer, text, ret->buffer_length);
    ret->buffer[ret->buffer_length] = '\0';
    ret->cache_head = NULL;
    return ret;
}
void string_builder_delete(string_builder_t *string_builder){
    if(string_builder == NULL) {return;}
    string_builder_delete(string_builder->cache_head);
    free(string_builder->buffer);
    free(string_builder);
}
string_builder_t *string_builder_append(string_builder_t* string_builder, const char* text){
    string_builder_t *iter = string_builder;
    
    while(iter->cache_head != NULL){
        iter = iter->cache_head;
    }
    
    iter->cache_head = string_builder_from_pchar(text);
    
    return string_builder;
}
char string_builder_ch(string_builder_t *string_builder, size_t index){
    string_builder_t *iter;
    size_t length_counter;
    
    length_counter = 0;
    for(iter=string_builder; iter!=NULL; iter=iter->cache_head){
        length_counter += iter->buffer_length;
        if(index < length_counter){
            return iter->buffer[ index - (length_counter - iter->buffer_length) ];
        }
    }
    
    return '\0';
}

char *string_builder_c_str(string_builder_t *string_builder){
    size_t length_total;
    string_builder_t *iter;
    char *new_buffer;
    char *p;
    
    length_total = 0;
    for(iter=string_builder; iter!=NULL; iter=iter->cache_head){
        length_total += iter->buffer_length;
    }
    
    new_buffer = (char*)malloc((length_total+1) * sizeof(char));
    p = new_buffer;
    for(iter=string_builder; iter!=NULL; iter=iter->cache_head){
        strncpy(p, iter->buffer, iter->buffer_length);
        p = p + iter->buffer_length;
    }
    *p = '\0';
    
    string_builder_delete(string_builder->cache_head);
    
    string_builder->buffer = new_buffer;
    string_builder->buffer_length = length_total;
    string_builder->cache_head = NULL;
    
    return string_builder->buffer;
}
