#define _CRT_SECURE_NO_WARNINGS
#include "util.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "mc_packets.h"

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


void map_set_raw(map_t pmap, str key, void *value)
{
	map_get_field_by_key(pmap, key)->data = value;
}
bool map_get_raw(map_t pmap, str key, void **dest)
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

void map_set_uint32(map_t pmap, str key, uint32_t value)
{
	map_get_field_by_key(pmap, key)->data = value;
}
bool map_get_uint32(map_t pmap, str key, uint32_t *dest)
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

void map_set_uint8(map_t pmap, str key, uint8_t value)
{
	map_get_field_by_key(pmap, key)->data = value;
};
bool map_get_uint8(map_t pmap, str key, uint8_t *dest)
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
};

void map_set_uint16(map_t pmap, str key, uint16_t value)
{
	map_get_field_by_key(pmap, key)->data = value;
};
bool map_get_uint16(map_t pmap, str key, uint16_t *dest)
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
};

void map_set_int32(map_t pmap, str key, int32_t value)
{
	map_get_field_by_key(pmap, key)->data = value;
};
bool map_get_int32(map_t pmap, str key, int32_t *dest)
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
};

void map_set_uint64(map_t pmap, str key, uint64_t value)
{
	uint64_t *ptr = malloc(sizeof(uint64_t));
	*ptr = value;
	map_get_field_by_key(pmap, key)->data = ptr;
	map *clearmap = (map *)pmap;
	clearmap->num_referenced_fields += 1;
	if (clearmap->num_referenced_fields == 1)
	{
		clearmap->referenced_fields = malloc(clearmap->num_referenced_fields * sizeof(str));
	}
	else
	{
		clearmap->referenced_fields = realloc(clearmap->referenced_fields, clearmap->num_referenced_fields * sizeof(str));
	}
	clearmap->referenced_fields[clearmap->num_referenced_fields - 1] = _strdup(key);
}
bool map_get_uint64(map_t pmap, str key, uint64_t *dest)
{
	if (map_has_field(pmap, key))
	{
		*dest = *((uint64_t *)(map_get_field_by_key(pmap, key)->data));
		return 1;
	}
	else
	{
		return 0;
	}
}

void map_set_int64(map_t pmap, str key, int64_t value)
{
	int64_t *ptr = malloc(sizeof(int64_t));
	*ptr = value;
	map_get_field_by_key(pmap, key)->data = ptr;
	map *clearmap = (map *)pmap;
	clearmap->num_referenced_fields += 1;
	if (clearmap->num_referenced_fields == 1)
	{
		clearmap->referenced_fields = malloc(clearmap->num_referenced_fields * sizeof(str));
	}
	else
	{
		clearmap->referenced_fields = realloc(clearmap->referenced_fields, clearmap->num_referenced_fields * sizeof(str));
	}
	clearmap->referenced_fields[clearmap->num_referenced_fields - 1] = _strdup(key);
}
bool map_get_int64(map_t pmap, str key, int64_t *dest)
{
	if (map_has_field(pmap, key))
	{
		*dest = *((int64_t *)(map_get_field_by_key(pmap, key)->data));
		return 1;
	}
	else
	{
		return 0;
	}
}

void map_set_uuid(map_t pmap, str key, struct uuid_t value)
{
	struct uuid_t *ptr = malloc(sizeof(struct uuid_t));
	*ptr = value;
	map_get_field_by_key(pmap, key)->data = ptr;
	map *clearmap = (map *)pmap;
	clearmap->num_referenced_fields += 1;
	if (clearmap->num_referenced_fields == 1)
	{
		clearmap->referenced_fields = malloc(clearmap->num_referenced_fields * sizeof(str));
	}
	else
	{
		clearmap->referenced_fields = realloc(clearmap->referenced_fields, clearmap->num_referenced_fields * sizeof(str));
	}
	clearmap->referenced_fields[clearmap->num_referenced_fields - 1] = _strdup(key);
}
bool map_get_uuid(map_t pmap, str key, struct uuid_t *dest)
{
	if (map_has_field(pmap, key))
	{
		*dest = *((struct uuid_t *)(map_get_field_by_key(pmap, key)->data));
		return 1;
	}
	else
	{
		return 0;
	}
}

void map_set_position(map_t pmap, str key, struct position_t value)
{
	struct position_t *ptr = malloc(sizeof(struct position_t));
	*ptr = value;
	map_get_field_by_key(pmap, key)->data = ptr;
	map *clearmap = (map *)pmap;
	clearmap->num_referenced_fields += 1;
	if (clearmap->num_referenced_fields == 1)
	{
		clearmap->referenced_fields = malloc(clearmap->num_referenced_fields * sizeof(str));
	}
	else
	{
		clearmap->referenced_fields = realloc(clearmap->referenced_fields, clearmap->num_referenced_fields * sizeof(str));
	}
	clearmap->referenced_fields[clearmap->num_referenced_fields - 1] = _strdup(key);
}
bool map_get_position(map_t pmap, str key, struct position_t *dest)
{
	if (map_has_field(pmap, key))
	{
		*dest = *((struct position_t *)(map_get_field_by_key(pmap, key)->data));
		return 1;
	}
	else
	{
		return 0;
	}
}

void map_set_double(map_t pmap, str key, double value)
{
	double *ptr = malloc(sizeof(double));
	*ptr = value;
	map_get_field_by_key(pmap, key)->data = ptr;
	map *clearmap = (map *)pmap;
	clearmap->num_referenced_fields += 1;
	if (clearmap->num_referenced_fields == 1)
	{
		clearmap->referenced_fields = malloc(clearmap->num_referenced_fields * sizeof(str));
	}
	else
	{
		clearmap->referenced_fields = realloc(clearmap->referenced_fields, clearmap->num_referenced_fields * sizeof(str));
	}
	clearmap->referenced_fields[clearmap->num_referenced_fields - 1] = _strdup(key);
}
bool map_get_double(map_t pmap, str key, double *dest)
{
	if (map_has_field(pmap, key))
	{
		*dest = *((double *)(map_get_field_by_key(pmap, key)->data));
		return 1;
	}
	else
	{
		return 0;
	}
}

void map_set_boolean(map_t pmap, str key, bool value)
{
	map_get_field_by_key(pmap, key)->data = value;
};
bool map_get_boolean(map_t pmap, str key, bool *dest)
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
};

void map_set_int16(map_t pmap, str key, int16_t value)
{
	map_get_field_by_key(pmap, key)->data = value;
};
bool map_get_int16(map_t pmap, str key, int16_t *dest)
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
};

bool map_get_int8(map_t pmap, char *key, int8_t *dest)
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
void map_set_int8(map_t pmap, char *key, int8_t value)
{
	map_get_field_by_key(pmap, key)->data = value;
}

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
	if (string != 0)
	{
		map_get_field_by_key(pmap, key)->data = _strdup(string);
	}
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
	map *map = pmap;
	size_t i;

	for (i = 0; i < map->num_referenced_fields; i++)
	{
		free(map_get_field_by_key(pmap, map->referenced_fields[i])->data);
		free(map->referenced_fields[i]);
	}

	for (i = 0; i < map->num_fields; i++)
	{
		free(map->fields[i].name);
	}

	free(pmap);
}
