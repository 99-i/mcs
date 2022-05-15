#include "util/string.h"
#include <stdlib.h>
#include <string.h>

char* str_cstr(str s)
{
	char* cstr = malloc(sizeof(char) * (s.size + 1));
	memcpy(cstr, s.data, s.size);
	cstr[s.size] = 0;
	return cstr;
}

str str_construct()
{
	str s;
	s.data = 0;
	s.size = 0;
	s.is_temp = false;
	return s;
}
str str_construct_from_cstr(const char* original)
{
	str s;
	s.data = malloc(sizeof(char) * strlen(original));
	s.size = strlen(original);
	memcpy(s.data, original, s.size);
	s.is_temp = false;
	return s;
}

str str_cstr_temp(const char* s)
{
	str temp;
	temp.is_temp = true;
	temp.data = s;
	temp.size = strlen(s);
	return temp;
}
str str_clone_str(str s1)
{
	str s;
	s.size = s1.size;
	s.is_temp = false;
	memcpy(s.data, s1.data, s.size);
	
	return s;
}
bool streq_str(str s1, str s2)
{
	return ((s1.size == s2.size) && !memcmp(s1.data, s2.data, s1.size));
}
bool streq_cstr(str s1, const char* s2)
{
	return ((s1.size == strlen(s2)) && !memcmp(s1.data, s2, s1.size));
}
void str_append_cstr(str *s1, const char* s2)
{
	size_t original_size = s1->size;
	s1->size += strlen(s2);
	s1->data = realloc(s1->data, sizeof(char) * s1->size);
	memcpy(s1->data + original_size, s2, strlen(s2));
}
void str_append_str(str* s1, str s2)
{
	size_t original_size = s1->size;
	if (s2.size == 0) return;
	if (s1->size == 0) return;

	s1->size += s2.size;
	s1->data = realloc(s1->data, sizeof(char) * s1->size);
	memcpy(s1->data + original_size, s2.data, s2.size);
}

void str_destroy(str* s)
{
	if (s->is_temp) return;
	free(s->data);
	s->data = 0;
	s->data = 0;
}
