#define _CRT_SECURE_NO_WARNINGS
#include "util.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "mc_packets.h"
#define MAP_REFERENCE_GET_FUNCTION(type, name) bool map_get_##name(map_t pmap, str key, ##type* dest) \
{ \
if (map_has_field(pmap, key)) \
{ \
*dest = *((##type*)(map_get_field_by_key(pmap, key)->data)); \
return true; \
} \
else \
{ \
return false; \
} \
}

#define MAP_REFERENCE_SET_FUNCTION(type, name) void map_set_##name(map_t pmap, str key, ##type value) \
{ \
##type* ptr = malloc(sizeof(##type)); \
*ptr = value; \
map_get_field_by_key(pmap, key)->data = ptr; \
map* clearmap = (map*)pmap; \
clearmap->num_referenced_fields += 1; \
if (clearmap->num_referenced_fields == 1) \
{ \
clearmap->referenced_fields = malloc(clearmap->num_referenced_fields * sizeof(str)); \
} \
else \
{ \
clearmap->referenced_fields = realloc(clearmap->referenced_fields, clearmap->num_referenced_fields * sizeof(str)); \
} \
clearmap->referenced_fields[clearmap->num_referenced_fields - 1] = _strdup(key); \
}

#define MAP_VALUE_SET_FUNCTION(type, name) void map_set_##name(map_t pmap, str key, ##type value) \
{ \
map_get_field_by_key(pmap, key)->data = value; \
}

#define MAP_VALUE_GET_FUNCTION(type, name) bool map_get_##name(map_t pmap, str key, ##type* dest) \
{ \
if (map_has_field(pmap, key)) \
{ \
	*dest = map_get_field_by_key(pmap, key)->data; \
	return true; \
} \
else \
{ \
	return false; \
} \
}


typedef struct
{
	uint32_t num_fields;
	struct map_field_t
	{
		str name;
		void *data;
	}*fields;

	uint32_t num_referenced_fields;
	str *referenced_fields;
} map;



void slurp_file_to_cstr(char *filepath, char **dest)
{
	FILE *f;
	size_t length;

	f = fopen(filepath, "r+");

	fseek(f, 0, SEEK_END);
	length = ftell(f);

	fseek(f, 0, SEEK_SET);

	*dest = malloc(length);

	fread(*dest, length, 1, f);
}


map_t construct_map(void)
{
	map *m = malloc(sizeof(map));
	m->num_fields = 0;
	m->fields = 0;
	m->num_referenced_fields = 0;
	m->referenced_fields = 0;
	return m;
}

bool map_has_field(map *pmap, str key)
{
	size_t i;
	for (i = 0; i < pmap->num_fields; i++)
	{
		if (!strcmp(pmap->fields[i].name, key))
		{
			return true;
		}
	}
	return false;
}
struct map_field_t *map_get_field_by_key(map *pmap, str key)
{
	size_t i;
	if (!map_has_field(pmap, key))
	{
		pmap->num_fields += 1;
		if (pmap->num_fields == 1)
		{
			pmap->fields = malloc(pmap->num_fields * sizeof(struct map_field_t));
		}
		else
		{
			pmap->fields = realloc(pmap->fields, pmap->num_fields * sizeof(struct map_field_t));
		}
		pmap->fields[pmap->num_fields - 1].name = _strdup(key);
		pmap->fields[pmap->num_fields - 1].data = 0;
		return &pmap->fields[pmap->num_fields - 1];
	}
	for (i = 0; i < pmap->num_fields; i++)
	{
		if (!strcmp(pmap->fields[i].name, key))
		{
			return &pmap->fields[i];
		}
	}
}

MAP_VALUE_SET_FUNCTION(void *, raw)
MAP_VALUE_GET_FUNCTION(void *, raw)

MAP_VALUE_SET_FUNCTION(uint32_t, uint32)
MAP_VALUE_GET_FUNCTION(uint32_t, uint32)

MAP_VALUE_SET_FUNCTION(uint8_t, uint8);
MAP_VALUE_GET_FUNCTION(uint8_t, uint8);

MAP_VALUE_SET_FUNCTION(uint16_t, uint16);
MAP_VALUE_GET_FUNCTION(uint16_t, uint16);

MAP_VALUE_SET_FUNCTION(int32_t, int32);
MAP_VALUE_GET_FUNCTION(int32_t, int32);

MAP_REFERENCE_SET_FUNCTION(uint64_t, uint64)
MAP_REFERENCE_GET_FUNCTION(uint64_t, uint64)

MAP_REFERENCE_SET_FUNCTION(int64_t, int64)
MAP_REFERENCE_GET_FUNCTION(int64_t, int64)

MAP_REFERENCE_SET_FUNCTION(struct uuid_t, uuid)
MAP_REFERENCE_GET_FUNCTION(struct uuid_t, uuid)

MAP_REFERENCE_SET_FUNCTION(struct position_t, position)
MAP_REFERENCE_GET_FUNCTION(struct position_t, position)

MAP_REFERENCE_SET_FUNCTION(double, double)
MAP_REFERENCE_GET_FUNCTION(double, double)

MAP_VALUE_SET_FUNCTION(bool, boolean);
MAP_VALUE_GET_FUNCTION(bool, boolean);

MAP_VALUE_SET_FUNCTION(int16_t, short);
MAP_VALUE_GET_FUNCTION(int16_t, short);

void map_set_float(map_t pmap, char *key, float flt)
{
	map_get_field_by_key(pmap, key)->data = *(int32_t *)&flt;
}

bool map_get_float(map_t pmap, char *key, float *dest)
{
	if (map_has_field(pmap, key))
	{
		*dest = *(float *)map_get_field_by_key(pmap, key);
		return true;
	}
	else
	{
		return false;
	}
}


void map_set_string(map_t pmap, char *key, char *string)
{
	map_get_field_by_key(pmap, key)->data = _strdup(string);
}

bool map_get_string(map_t pmap, char *key, char **dest)
{
	if (map_has_field(pmap, key))
	{
		*dest = _strdup(map_get_field_by_key(pmap, key)->data);
		return true;
	}
	else
	{
		return false;
	}
}

void map_free(map_t pmap)
{
	free(pmap);
}
