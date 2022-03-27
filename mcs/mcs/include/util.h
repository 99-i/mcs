#pragma once
#include <stdint.h>
#include <stdbool.h>
typedef void *map_t;

typedef char *str;

void slurp_file_to_cstr(char *filepath, char **dest);

map_t construct_map(void);
void map_free(map_t pmap);

bool map_get_raw(map_t pmap, str key, void **dest);
void map_set_raw(map_t pmap, str key, void *value);

bool map_get_uint32(map_t pmap, str key, uint32_t *dest);
void map_set_uint32(map_t pmap, str key, uint32_t value);

bool map_get_uint64(map_t pmap, str key, uint64_t *dest);
void map_set_uint64(map_t pmap, str key, uint64_t value);

bool map_get_int64(map_t pmap, str key, int64_t *dest);
void map_set_int64(map_t pmap, str key, int64_t value);

bool map_get_uint8(map_t pmap, str key, uint8_t *dest);
void map_set_uint8(map_t pmap, str key, uint8_t value);

bool map_get_uint16(map_t pmap, str key, uint16_t *dest);
void map_set_uint16(map_t pmap, str key, uint16_t value);

bool map_get_int32(map_t pmap, str key, int32_t *dest);
void map_set_int32(map_t pmap, str key, int32_t value);

bool map_get_string(map_t pmap, str key, char **dest);
void map_set_string(map_t pmap, str key, char *value);

bool map_get_uuid(map_t pmap, str key, struct uuid_t *dest);
void map_set_uuid(map_t pmap, str key, struct uuid_t value);

bool map_get_position(map_t pmap, str key, struct position_t *dest);
void map_set_position(map_t pmap, str key, struct position_t value);

bool map_get_boolean(map_t pmap, str key, bool *dest);
void map_set_boolean(map_t pmap, str key, bool value);

bool map_get_short(map_t pmap, str key, int16_t *dest);
void map_set_short(map_t pmap, str key, int16_t value);

bool map_get_float(map_t pmap, str key, float *dest);
void map_set_float(map_t pmap, str key, float value);

bool map_get_double(map_t pmap, str key, double *dest);
void map_set_double(map_t pmap, str key, double value);
