#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "util/def.h"

typedef struct str
{
	i32 size;
	char* data;
	const char* tempdata;
	bool is_temp;
} str;

char* str_cstr(str s);

str str_construct();
str str_construct_from_cstr(const char *original);
str str_clone_str(str s1);
bool streq_str(str s1, str s2);
bool streq_cstr(str s1, const char* s2);
void str_append_cstr(str *s1, const char* s2);
void str_append_str(str *s1, str s2);
void str_append_char(str *s1, char c);
char str_getchar(str s1, i32 index);
void str_destroy(str* s);
bool str_starts_with_cstr(str check, const char* to_start_with);
bool str_starts_with_str(str check, str to_start_with);
