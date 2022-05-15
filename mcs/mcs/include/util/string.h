#pragma once

#include <stdbool.h>
#include <stdint.h>
typedef struct str
{
	size_t size;
	char* data;
	bool is_temp;
} str;

char* str_cstr(str s);

str str_construct();
str str_construct_from_cstr(const char *original);
str str_cstr_temp(const char *s);
str str_clone_str(str s1);
bool streq_str(str s1, str s2);
bool streq_cstr(str s1, const char* s2);
void str_append_cstr(str *s1, const char* s2);
void str_append_str(str *s1, str s2);
void str_destroy(str* s);
