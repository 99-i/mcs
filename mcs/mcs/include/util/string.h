#pragma once

#include <stdbool.h>
#include <stdbool.h>

typedef void* str;

char* str_cstr(str s);

str str_construct();
str str_construct_from_cstr(const char* original);

bool str_equal(str s1, str s2);
size_t str_length(str s);
char str_get(str s, size_t i);
void str_append(str s1, const char* s2);
void str_append(str s1, str s2);

