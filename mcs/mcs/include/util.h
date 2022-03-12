#pragma once
#include <stdint.h>
#include <stdbool.h>
typedef void* map_t;


map_t construct_map(void);

void	map_set_raw(map_t map, char* key, void* value);
void	map_set_uint32(map_t map, char* key, uint32_t value);

bool	map_get_raw(map_t map, char* key, void** dest);
bool	map_get_uint32(map_t map, char* key, uint32_t* dest);
void	map_free(map_t map);
