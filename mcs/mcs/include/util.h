#pragma once
#include <stdint.h>
#include <stdbool.h>
typedef void *map_t;

typedef const char *str;

void slurp_file_to_cstr(char *filepath, char **dest);

#define MAP_GET_SIGNATURE(type, name) bool map_get_##name(map_t pmap, str key, ##type* dest)
#define MAP_SET_SIGNATURE(type, name) void map_set_##name(map_t pmap, str key, ##type value)
map_t construct_map(void);
void map_free(map_t pmap);


MAP_SET_SIGNATURE(void *, raw);
MAP_GET_SIGNATURE(void *, raw);

MAP_SET_SIGNATURE(uint32_t, uint32);
MAP_GET_SIGNATURE(uint32_t, uint32);

MAP_SET_SIGNATURE(uint64_t, uint64);
MAP_GET_SIGNATURE(uint64_t, uint64);

MAP_SET_SIGNATURE(int64_t, int64);
MAP_GET_SIGNATURE(int64_t, int64);

MAP_SET_SIGNATURE(uint8_t, uint8);
MAP_GET_SIGNATURE(uint8_t, uint8);

MAP_SET_SIGNATURE(uint16_t, uint16);
MAP_GET_SIGNATURE(uint16_t, uint16);

MAP_SET_SIGNATURE(int32_t, int32);
MAP_GET_SIGNATURE(int32_t, int32);

MAP_SET_SIGNATURE(char *, string);
MAP_GET_SIGNATURE(char *, string);

MAP_SET_SIGNATURE(struct uuid_t, uuid);
MAP_GET_SIGNATURE(struct uuid_t, uuid);

MAP_SET_SIGNATURE(struct position_t, position);
MAP_GET_SIGNATURE(struct position_t, position);

MAP_SET_SIGNATURE(bool, boolean);
MAP_GET_SIGNATURE(bool, boolean);

MAP_SET_SIGNATURE(double, double);
MAP_SET_SIGNATURE(double, double);

MAP_SET_SIGNATURE(float, float);
MAP_SET_SIGNATURE(float, float);

MAP_SET_SIGNATURE(int16_t, short);
MAP_GET_SIGNATURE(int16_t, short);
