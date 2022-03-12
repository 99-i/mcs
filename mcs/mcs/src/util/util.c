#include "util.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>


typedef struct
{
	uint32_t num_fields;
	struct map_field_t
	{
		char* name;
		void* data;
	}*fields;
} map;


map_t construct_map(void)
{
	map* m = malloc(sizeof(map));

	return m;
}

bool map_has_field(map* pmap, char* key)
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
struct map_field_t* map_get_field_by_key(map* pmap, char* key)
{
	size_t i;
	if (!map_has_field(pmap, key))
	{
		pmap->num_fields += 1;
		pmap->fields = realloc(pmap->fields, pmap->num_fields * sizeof(struct map_field_t));

		pmap->fields[pmap->num_fields - 1].name = strdup(key);
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
	asse
}

void map_set_raw(map_t pmap, char* key, void* value)
{
	map_get_field_by_key(pmap, key)->data = value;
}


void map_set_uint32(map_t pmap, char* key, uint32_t value)
{
	map_get_field_by_key(pmap, key)->data = value;
}


bool map_get_raw(map_t map, char* key, void** dest)
{

}
bool map_get_uint32(map_t map, char* key, uint32_t* dest)
{

}





void map_free(map_t pmap)
{
	free(pmap);
}
