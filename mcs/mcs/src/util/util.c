#define _CRT_SECURE_NO_WARNINGS
#include "util.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "mc_packets.h"

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

#pragma region Map


typedef struct
{
	uint32_t num_fields;
	struct map_field_t
	{
		char *name;
		void *data;
	}*fields;
} map;





map_t construct_map(void)
{
	map *m = malloc(sizeof(map));
	m->num_fields = 0;
	m->fields = 0;
	return m;
}

bool map_has_field(map *pmap, char *key)
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
struct map_field_t *map_get_field_by_key(map *pmap, char *key)
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


void map_set(map_t pmap, char *key, void *value)
{
	map_get_field_by_key(pmap, key)->data = value;
}
bool map_get(map_t pmap, char *key, void **dest)
{
	if (map_has_field(pmap, key))
	{
		*dest = map_get_field_by_key(pmap, key)->data;
		return 1;
	}
	else
	{
		return 0;
	}
}


void map_free(map_t pmap)
{
	map *map = pmap;
	size_t i;

	for (i = 0; i < map->num_fields; i++)
	{
		map_destroy(pmap, map->fields[i].name);
	}

	free(pmap);
}

void map_destroy(map_t pmap, char* key)
{
	size_t i;
	struct map_field_t* field;
	map* real = (map*)pmap;
	if (map_has_field(pmap, key))
	{
		field = map_get_field_by_key(pmap, key);
		free(field->name);
		for (i = (field - real->fields); i < real->num_fields - 1; i++)
		{
			real->fields[i] = real->fields[i + 1];
		}
		real->num_fields--;
		real->fields = realloc(real->fields, sizeof(struct map_field_t) * real->num_fields);
	}

}


#pragma endregion

#pragma region Buffer

typedef struct
{
	uint8_t *data;
	size_t data_length;
} buffer;


buffer_t construct_buffer(void)
{
	buffer *b;
	b = calloc(sizeof(buffer), 1);
	return b;
}

void buffer_free(buffer_t buf)
{
	buffer *b = (buffer *)buf;
	free(b->data);
	free(b);
}

void buffer_append(buffer_t buf, uint8_t *data, size_t size)
{
	buffer *b = (buffer *)buf;
	size_t i;
	size_t dest;
	size_t original_length;

	if (size == 0) return;

	original_length = b->data_length;
	b->data_length += size;
	if (!b->data)
	{
		b->data = malloc(b->data_length);
	}
	else
	{
		b->data = realloc(b->data, sizeof(uint8_t) * (b->data_length));
	}

	for (size_t i = original_length; i < b->data_length; i++)
	{
		b->data[i] = data[i - original_length];
	}

}

void buffer_append_byte(buffer_t buf, uint8_t byte)
{
	buffer *b = (buffer *)buf;
	buffer_append(buf, &byte, 1);
}

void buffer_clear(buffer_t buf)
{
	buffer *b = (buffer *)buf;

	if (b->data)
	{
		free(b->data);
	}
	b->data = 0;
	b->data_length = 0;
}

uint8_t *buffer_get(buffer_t buf, size_t *length)
{
	uint8_t *arr;
	buffer *b = (buffer *)buf;

	*length = b->data_length;
	arr = malloc(sizeof(uint8_t) * b->data_length);
	memcpy(arr, b->data, sizeof(uint8_t) * b->data_length);
	return arr;
}

#pragma endregion

