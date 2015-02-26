#include "json.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#include "remote/string_builder.h"


// string
json_value_t *json_string_new(){
    json_value_t *ret = json_value_new();
    ret->type = json_value_string_e;
    ret->part._string.buffer = NULL;
    ret->part._string.buffer_length = 0;
    return ret;
}

json_value_t *json_string_from_pchar(const char *text, size_t length){
    json_value_t *ret = json_value_new();
    ret->type = json_value_string_e;
    ret->part._string.buffer = (char*)malloc(length+1);
    strncpy(ret->part._string.buffer, text, length);
    ret->part._string.buffer[length] = '\0';
    ret->part._string.buffer_length = length;
    return ret;
}

void json_string_delete(json_value_t *json_string){
    free(json_string->part._string.buffer);
    free(json_string);
}

int json_string_eq(json_value_t *string_a, json_value_t *string_b){
    return strcmp(string_a->part._string.buffer, string_b->part._string.buffer) == 0;
}

// number
json_value_t *json_number_new(){
    json_value_t *ret = json_value_new();
    ret->type = json_value_number_e;
    ret->part._number.part_float = 0;
    ret->part._number.part_int = 0;
    return ret;
}

static void json_float_set_unuse_(json_float_t *f){
    if(sizeof(json_float_t) == sizeof(uint32_t)){
        *(uint32_t*)(f) = 0;
    }else if(sizeof(json_float_t) == sizeof(uint64_t)){
        *(uint64_t*)(f) = 0;
    }else{
        printf("?\n");
        exit(-1);
    }
}

static int json_float_is_unuse_(json_float_t *f){
    if(sizeof(json_float_t) == sizeof(uint32_t)){
        return *(uint32_t*)(f) == 0;
    }else if(sizeof(json_float_t) == sizeof(uint64_t)){
        return *(uint64_t*)(f) == 0;
    }else{
        printf("wtf?\n");
        exit(-1);
        return 0;
    }
}

json_value_t *json_number_from_int(json_int_t v){
    json_value_t *ret = json_number_new();
    return json_number_set_int(ret, v);
}

json_value_t *json_number_from_float(json_float_t v){
    json_value_t *ret = json_number_new();
    return json_number_set_float(ret, v);
}

void json_number_delete(json_value_t *json_number){
    free(json_number);
}

json_value_t *json_number_set_int(json_value_t *json_number, json_int_t v){
    json_float_set_unuse_(&json_number->part._number.part_float);
    json_number->part._number.part_int = v;
    return json_number;
}

json_value_t *json_number_set_float(json_value_t *json_number, json_float_t v){
    json_number->part._number.part_float = v;
    json_number->part._number.part_int = (json_int_t)v;
    return json_number;
}

json_int_t json_number_get_int(json_value_t *json_number){
    return json_number->part._number.part_int;
}

json_float_t json_number_get_float(json_value_t *json_number){
    if(json_float_is_unuse_(&json_number->part._number.part_float)){
        return (json_float_t)json_number->part._number.part_int;
    }else{
        return json_number->part._number.part_float;
    }
}

// object
json_value_t *json_object_new(){
    json_value_t *ret = json_value_new();
    ret->type = json_value_object_e;
    ret->part._object.name = NULL;
    ret->part._object.value = NULL;
    ret->part._object.next = NULL;
    return ret;
}

json_value_t *json_object_from_k_v(json_value_t *k, json_value_t *v){
    json_value_t *ret = json_value_new();
    ret->type = json_value_object_e;
    ret->part._object.name = k;
    ret->part._object.value = v;
    ret->part._object.next = NULL;
    return ret;
}

void json_object_delete(json_value_t *json_object){
    if(json_object == NULL) {return;}
    json_value_delete(json_object->part._object.name);
    json_value_delete(json_object->part._object.value);
    json_object_delete(json_object->part._object.next);
    free(json_object);
}

// notice: object_a is unsafe when after function call
json_value_t *json_object_concat(json_value_t *object_a, json_value_t *object_b){
    json_value_t *last_objet;
    if(object_b->part._object.value == NULL){
        json_object_delete(object_b);
        return object_a;
    }
    if(object_a->part._object.value == NULL){
        json_object_delete(object_a);
        return object_b;
    }
    for(last_objet=object_a; last_objet->part._object.next!=NULL; last_objet=last_objet->part._object.next) {/* do nothing */}
    last_objet->part._object.next = object_b;
    return object_a;
}

json_value_t *json_object_at(json_value_t *json_object, size_t n){
    if(json_object->part._object.value == NULL){
        return json_null_new();
    }
    while(n --> 0 || json_object!=NULL){
        json_object = json_object->part._object.next;
    }
    return json_object;
}

json_value_t *json_object_find(json_value_t *json_object, json_value_t *json_string){
    if(json_object->part._object.value == NULL){
        return json_null_new();
    }
    while(json_object!=NULL){
        if(json_string_eq(json_object->part._object.name, json_string)){
            return json_object;
        }
        json_object = json_object->part._object.next;
    }
    return json_object;
}

// array
json_value_t *json_array_new(){
    json_value_t *ret = json_value_new();
    ret->type = json_value_array_e;
    ret->part._array.value = NULL;
    ret->part._array.next = NULL;
    return ret;
}

json_value_t *json_array_from_v(json_value_t *v){
    json_value_t *ret = json_value_new();
    ret->type = json_value_array_e;
    ret->part._array.value = v;
    ret->part._array.next = NULL;
    return ret;
}

void json_array_delete(json_value_t *json_array){
    if(json_array == NULL) {return;}
    json_value_delete(json_array->part._array.value);
    json_array_delete(json_array->part._array.next);
    free(json_array);
}

json_value_t *json_array_concat(json_value_t *array_a, json_value_t *array_b){
    json_value_t *last_array;
    if(array_b->part._array.value == NULL){
        json_array_delete(array_b);
        return array_a;
    }
    if(array_a->part._array.value == NULL){
        json_array_delete(array_a);
        return array_b;
    }
    for(last_array=array_a; last_array->part._array.next!=NULL; last_array=last_array->part._array.next) {/* do nothing */}
    last_array->part._array.next = array_b;
    return array_a;
}

json_value_t *json_array_at(json_value_t *json_array, size_t n){
    if(json_array->part._array.value == NULL){
        return json_null_new();
    }
    while(n --> 0 || json_array!=0){
        json_array = json_array->part._object.next;
    }
    return json_array;
}


// true,false,null
json_value_t *json_true_new(){
    static json_value_t *ret = NULL;
    if(ret == NULL){
        ret = json_value_new();
        ret->type = json_value_true_e;
    }
    return ret;
}

json_value_t *json_false_new(){
    static json_value_t *ret = NULL;
    if(ret == NULL){
        ret = json_value_new();
        ret->type = json_value_false_e;
    }
    return ret;
}

json_value_t *json_null_new(){
    static json_value_t *ret = NULL;
    if(ret == NULL){
        ret = json_value_new();
        ret->type = json_value_null_e;
    }
    return ret;
}

void json_true_delete(json_value_t *value){
    (void)value;
//     free(value);
}

void json_false_delete(json_value_t *value){
    (void)value;
//     free(value);
}

void json_null_delete(json_value_t *value){
    (void)value;
//     free(value);
}

// value
json_value_t *json_value_new(){
    json_value_t *ret = (json_value_t*)malloc(sizeof(json_value_t));
    ret->type = json_value_null_e;
    return ret;
}

void json_value_delete(json_value_t *value){
    if(value == NULL) {return;}
    switch(value->type){
        case json_value_string_e:
            json_string_delete(value);
            break;
        case json_value_number_e:
            json_number_delete(value);
            break;
        case json_value_object_e:
            json_object_delete(value);
            break;
        case json_value_array_e:
            json_array_delete(value);
            break;
        case json_value_true_e:
            json_true_delete(value);
            break;
        case json_value_false_e:
            json_false_delete(value);
            break;
        case json_value_null_e:
            json_null_delete(value);
            break;
        default:
            printf("wtf?\n");
            break;
    }
}


// convert

size_t json_string_secape(char *dest, const char *text, size_t length){
    size_t i = 0;
    size_t j = 0;
    for(i=0; i<length; i++){
        if(text[i]=='"' || text[i]=='\\'){
            dest[j] = '\\';
            j++;
        }
        dest[j] = text[i];
        j++;
    }
    dest[j] = '\0';
    return j;
}

static char hex2char(char h, char l){
    char ret = 0;
    if('0'<=h && h<='9'){
        ret = (char)(h-'0');
    }else if('a'<=h && h<='f'){
        ret = (char)(h-'a');
    }else if('A'<=h && h<='F'){
        ret = (char)(h-'A');
    }else{
        return h;
    }
    ret = (char)(ret << 4);
    if('0'<=l && l<='9'){
        ret |= (char)(l-'0');
    }else if('a'<=l && l<='f'){
        ret |= (char)(l-'a');
    }else if('A'<=l && l<='F'){
        ret |= (char)(l-'A');
    }else{
        return l;
    }
    return ret;
}

size_t json_string_unsecape(char *dest, const char *text, size_t length){
    size_t i = 0;
    size_t j = 0;
    char ch;
    for(i=0; i<length; i++){
        if(text[i] == '\\'){
            i++;
            switch(text[i]){
                case '"':
                    ch = '"';
                    break;
                case '\\':
                    ch = '\\';
                    break;
                case '/':
                    ch = '/';
                    break;
                case 'b':
                    ch = '\b';
                    break;
                case 'f':
                    ch = '\f';
                    break;
                // NTR
                case 'n':
                    ch = '\n';
                    break;
                case 't':
                    ch = '\t';
                    break;
                case 'r':
                    ch = '\r';
                    break;
                case 'u':
                    i++;
                    dest[j] = hex2char(text[i], text[i+1]);
                    j++; i+=2;
                    ch = hex2char(text[i], text[i+1]);
                    i++;
                    break;
                default:
                    ch = text[i];
            }
        }
        else{
            ch = text[i];
        }
        dest[j] = ch;
        j++;
        
    }
    dest[j] = '\0';
    return j;
}

static string_builder_t *json_value_to_string_builder(string_builder_t *sb, json_value_t *value){
    json_value_t *iter;
    char *buff = NULL;
    switch(value->type){
        case json_value_string_e:
            string_builder_append(sb, "\"");
            buff = (char*)malloc(value->part._string.buffer_length*2+1);
            buff[json_string_secape(buff, value->part._string.buffer, value->part._string.buffer_length)] = '\0';
            string_builder_append(sb, buff);
            string_builder_append(sb, "\"");
            free(buff);
            break;
        case json_value_number_e:
            buff = (char*)malloc(128);
            if(json_float_is_unuse_(&value->part._number.part_float)){
                sprintf(buff, JSON_INT_FMT, value->part._number.part_int);
                string_builder_append(sb, buff);
            }
            else{
                sprintf(buff, JSON_FLOAT_FMT, value->part._number.part_float);
                string_builder_append(sb, buff);
            }
            free(buff);
            break;
        case json_value_object_e:
            string_builder_append(sb, "{");
            if(value->part._object.value != NULL){
                for(iter=value; iter!=NULL; iter=iter->part._object.next){
                    if(iter != value){
                        string_builder_append(sb, ", ");
                    }
                    json_value_to_string_builder(sb, iter->part._object.name);
                    string_builder_append(sb, ":");
                    json_value_to_string_builder(sb, iter->part._object.value);
                }
            }
            string_builder_append(sb, "}");
            break;
        case json_value_array_e:
            string_builder_append(sb, "[");
            if(value->part._array.value != NULL){
                for(iter=value; iter!=NULL; iter=iter->part._array.next){
                    if(iter != value){
                        string_builder_append(sb, ", ");
                    }
                    json_value_to_string_builder(sb, iter->part._array.value);
                }
            }
            string_builder_append(sb, "]");
            break;
        case json_value_true_e:
            string_builder_append(sb, "true");
            break;
        case json_value_false_e:
            string_builder_append(sb, "false");
            break;
        case json_value_null_e:
            string_builder_append(sb, "null");
            break;
        default:
            printf("wtf?\n");
            exit(-1);
            return NULL;
    }
    return sb;
}

size_t json_value_to_string(char *dest, json_value_t *value){
    string_builder_t *sb =  string_builder_new();
    size_t dest_length;
    json_value_to_string_builder(sb, value);
    string_builder_c_str(sb);
    if(dest != NULL){
        strncpy(dest, sb->buffer, sb->buffer_length);
        dest[sb->buffer_length] = '\0';
    }
    dest_length = sb->buffer_length;
    string_builder_delete(sb);
    return dest_length;
}

static int is_number(char ch){
    return '0'<=ch && ch<='9';
}

static json_value_t *read_number(const char *text, size_t *pos){
    json_value_t *ret;
    json_int_t int_ret;
    json_int_t exp;
    json_float_t float_ret;
    json_float_t float_dleta;
    size_t p = *pos;
    int sign = 0;
    int e_sign = 0;
    int_ret = 0;
    float_dleta = 1;
    
    if(text[p] == '-'){
        sign = -1;
        p++;
    }
    while(is_number(text[p])){
        int_ret *= 10;
        int_ret += text[p] - '0';
        p++;
    }
    if(text[p] == '.'){
        float_ret = (json_float_t)int_ret;
        p++;
        while(is_number(text[p])){
            float_dleta /= 10;
            float_ret += float_dleta * (text[p] - '0');
            p++;
        }
        if(text[p]=='e' || text[p]=='E'){
            p++;
            if(text[p]=='+') {p++;}
            else if(text[p]=='-') {e_sign=-1; p++;}
            exp = 0;
            while(is_number(text[p])){
                exp *= 10;
                exp += text[p] - '0';
                p++;
            }
            while(exp --> 0){
                if(e_sign == -1){
                    float_ret /= 10;
                }
                else{
                    float_ret *= 10;
                }
            }
        }
        ret = json_number_from_float(float_ret);
    }
    else{
        exp = 0;
        while(is_number(text[p])){
            exp *= 10;
            exp += text[p] - '0';
            p++;
        }
        while(exp --> 0){
            if(e_sign == -1){
                int_ret /= 10;
            }
            else{
                int_ret *= 10;
            }
        }
        ret = json_number_from_int(sign==-1? -int_ret: int_ret);
    }
    *pos = p;
    return ret;
}

static int is_white_space(char ch){
    return ch==' ' || ch=='\n' || ch=='\t' || ch=='\r';
}

static void skip_white_space(const char *text, size_t *pos){
    while(is_white_space(text[*pos])){
        (*pos)++;
    }
}

static void skip_string(const char *text, size_t *pos){
    while(text[*pos]!='"' || text[(*pos)-1]=='\\'){
        if(text[*pos]=='\0'){
            return;
        }
        (*pos)++;
    }
}

static json_value_t *read_k_v(const char *text, size_t *pos){
    json_value_t *ret;
    json_value_t *k;
    json_value_t *v;
    size_t p = *pos;
    
    k = json_parser(text, &p);
    if(k == NULL){
        *pos = p;
        return NULL;
    }
    skip_white_space(text, &p);
    if(text[p] != ':'){
        json_value_delete(k);
        *pos = p;
        return NULL;
    }
    p++;
    skip_white_space(text, &p);
    v = json_parser(text, &p);
    if(v == NULL){
        json_value_delete(k);
        *pos = p;
        return NULL;
    }
    ret = json_object_from_k_v(k, v);
    *pos = p;
    return ret;
}

json_value_t *json_parser(const char *text, size_t *pos){
    json_value_t *ret;
    json_value_t *item;
    json_value_t *back;
    size_t p = *pos;
    size_t b;
    size_t string_length;
    char *buff;
    
    skip_white_space(text, &p);
    switch(text[p]){
        case '"':
            p++;
            b = p;
            skip_string(text, &p);
            buff = (char*)malloc((p-b+1) * sizeof(char));
            string_length = json_string_unsecape(buff, text+b, p-b);
            ret = json_string_from_pchar(buff, string_length);
            free(buff);
            if(text[p]=='"'){
                p++;
            }
            break;
        case '{':
            p++;
            b = p;
            skip_white_space(text, &p);
            if(text[p]=='\0' || text[p]=='}'){
                ret = json_object_new();
            }
            else{
                ret = read_k_v(text, &p);
                skip_white_space(text, &p);
                if(text[p]==','){
                    p++;
                    skip_white_space(text, &p);
                }
                back = ret;
                while((item=read_k_v(text, &p)) != NULL){
                    skip_white_space(text, &p);
                    if(text[p]==','){
                        p++;
                        skip_white_space(text, &p);
                    }
                    back->part._object.next = item;
                    back = item;
                }
            }
            if(text[p]=='}'){
                p++;
            }
            break;
        case '[':
            p++;
            b = p;
            skip_white_space(text, &p);
            if(text[p]=='\0' || text[p]==']'){
                ret = json_array_new();
            }
            else{
                item = json_parser(text, &p);
                ret = json_array_from_v(item);
                skip_white_space(text, &p);
                if(text[p]==','){
                    p++;
                    skip_white_space(text, &p);
                }
                back = ret;
                while((item=json_parser(text, &p)) != NULL){
                    skip_white_space(text, &p);
                    if(text[p]==','){
                        p++;
                        skip_white_space(text, &p);
                    }
                    back->part._array.next = json_array_from_v(item);
                    back = back->part._array.next;
                }
            }
            if(text[p]==']'){
                p++;
            }
            break;
        case 't':
            if(strcmp(&text[p],"true")==0){
                ret = json_true_new();
                p += 4;
                skip_white_space(text, &p);
            }
            else{
                ret = NULL;
            }
            break;
        case 'f':
            if(strcmp(&text[p],"false")==0){
                ret = json_false_new();
                p += 5;
                skip_white_space(text, &p);
            }
            else{
                ret = NULL;
            }
            break;
        case 'n':
            if(strcmp(&text[p],"null")==0){
                ret = json_null_new();
                p += 4;
                skip_white_space(text, &p);
            }
            else{
                ret = NULL;
            }
            break;
        case '\0':
            ret = json_null_new();
            break;
        default:
            if(is_number(text[p]) || text[p]=='-'){
                ret = read_number(text, &p);
            }
            else{
                *pos = p;
                return NULL;
            }
    }
    
    skip_white_space(text, &p);
    *pos = p;
    return ret;
}

json_value_t *json_convert_value_2_string(json_value_t *value){
    json_value_t *ret;
    string_builder_t *sb =  string_builder_new();
    json_value_to_string_builder(sb, value);
    string_builder_c_str(sb);
    ret = json_string_from_pchar(sb->buffer, sb->buffer_length);
    string_builder_delete(sb);
    return ret;
}

json_value_t *json_convert_string_2_value(json_value_t *json_string){
    json_value_t *ret;
    const char *text;
    size_t pos;
    
    text = json_string->part._string.buffer;
    pos = 0;
    ret = json_parser(text, &pos);
    return ret;
}

json_value_t *json_select(json_value_t *json_object, json_value_t *json_array_selector){
    size_t index;
    json_value_t *cur_selector;
    for(;json_array_selector!=NULL; json_array_selector=json_array_selector->part._array.next){
        cur_selector = json_array_selector->part._array.value;
        if(cur_selector->type == json_value_string_e){
            if(json_object->type == json_value_object_e){
                json_object = json_object_find(json_object, cur_selector);
            }
            else{
                return json_null_new();
            }
        }
        else if(cur_selector->type == json_value_number_e){
            index = (size_t)json_number_get_int(cur_selector);
            if(json_object->type == json_value_object_e){
                json_object = json_object_at(json_object, index);
            }
            else if(json_object->type == json_value_array_e){
                json_object = json_array_at(json_object, index);
            }
            else{
                return json_null_new();
            }
        }
        else{
            return json_null_new();
        }
    }
    return json_object;
}