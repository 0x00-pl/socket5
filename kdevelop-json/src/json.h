#ifndef _JSON_H_
#define _JSON_H_

#include <stddef.h>

typedef enum json_value_type_decl{
    json_value_string_e,
    json_value_number_e,
    json_value_object_e,
    json_value_array_e,
    json_value_true_e,
    json_value_false_e,
    json_value_null_e
}json_value_type_t;

typedef long long int json_int_t;
#define JSON_INT_FMT "%lld"
typedef double json_float_t;
#define JSON_FLOAT_FMT "%lf"
typedef struct json_number_decl{
    json_float_t part_float;
    json_int_t part_int;
}json_number_t;

typedef struct json_string_decl{
    char *buffer;
    size_t buffer_length;
}json_string_t;

struct json_value_decl;

typedef struct json_array_decl{
    struct json_value_decl *value;
    struct json_value_decl *next;
}json_array_t;

typedef struct json_object_decl{
    struct json_value_decl *name;
    struct json_value_decl *value;
    struct json_value_decl *next;
}json_object_t;

typedef struct json_value_decl{
    json_value_type_t type;
    union{
        json_string_t _string;
        json_number_t _number;
        json_array_t _array;
        json_object_t _object;
    }part;
} json_value_t;



// string
json_value_t *json_string_new();
json_value_t *json_string_from_pchar(const char *text, size_t length);
void json_string_delete(json_value_t *json_string);
int json_string_eq(json_value_t *string_a, json_value_t *string_b);

// number
json_value_t *json_number_new();
json_value_t *json_number_from_int(json_int_t v);
json_value_t *json_number_from_float(json_float_t v);
void json_number_delete(json_value_t *json_number);
json_value_t *json_number_set_int(json_value_t *json_number, json_int_t v);
json_value_t *json_number_set_float(json_value_t *json_number, json_float_t v);
json_int_t json_number_get_int(json_value_t *json_number);
json_float_t json_number_get_float(json_value_t *json_number);

// object
json_value_t *json_object_new();
json_value_t *json_object_from_k_v(json_value_t *k, json_value_t *v);
void json_object_delete(json_value_t *json_object);
// notice: object_a is unsafe when after function call
json_value_t *json_object_concat(json_value_t *object_a, json_value_t *object_b);
json_value_t *json_object_at(json_value_t *json_object, size_t n);
json_value_t *json_object_find(json_value_t *json_object, json_value_t *json_string);

// array
json_value_t *json_array_new();
json_value_t *json_array_from_v(json_value_t *v);
void json_array_delete(json_value_t *json_array);
// notice: array_a is unsafe when after function call
json_value_t *json_array_concat(json_value_t *array_a, json_value_t *array_b);
json_value_t *json_array_at(json_value_t *json_array, size_t n);

// true,false,null
json_value_t *json_true_new();
json_value_t *json_false_new();
json_value_t *json_null_new();
void json_true_delete(json_value_t *value);
void json_false_delete(json_value_t *value);
void json_null_delete(json_value_t *value);


// value
json_value_t *json_value_new();
void json_value_delete(json_value_t *value);

// convert
size_t json_string_secape(char *dest, const char *text, size_t length);
size_t json_string_unsecape(char *dest, const char *text, size_t length);

size_t json_value_to_string(char *dest, json_value_t *value);
json_value_t *json_parser(const char *text, size_t *pos);

json_value_t *json_convert_value_2_string(json_value_t *value);
json_value_t *json_convert_string_2_value(json_value_t *json_string);

json_value_t *json_select(json_value_t *json_object, json_value_t *json_array_selector);

#endif
