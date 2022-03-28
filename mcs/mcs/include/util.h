#pragma once
#include <stdint.h>
#include <stdbool.h>


typedef void *map_t;
typedef void *buffer_t;


void slurp_file_to_cstr(char *filepath, char **dest);

//an associative array between char*'s and any value
#pragma region Map

map_t construct_map(void);
void map_free(map_t pmap);

bool map_get_raw(map_t pmap, char *key, void **dest);
void map_set_raw(map_t pmap, char *key, void *value);

bool map_get_uint32(map_t pmap, char *key, uint32_t *dest);
void map_set_uint32(map_t pmap, char *key, uint32_t value);

bool map_get_uint64(map_t pmap, char *key, uint64_t *dest);
void map_set_uint64(map_t pmap, char *key, uint64_t value);

bool map_get_int64(map_t pmap, char *key, int64_t *dest);
void map_set_int64(map_t pmap, char *key, int64_t value);

bool map_get_uint8(map_t pmap, char *key, uint8_t *dest);
void map_set_uint8(map_t pmap, char *key, uint8_t value);

bool map_get_uint16(map_t pmap, char *key, uint16_t *dest);
void map_set_uint16(map_t pmap, char *key, uint16_t value);

bool map_get_int8(map_t pmap, char *key, int8_t *dest);
void map_set_int8(map_t pmap, char *key, int8_t value);

bool map_get_int32(map_t pmap, char *key, int32_t *dest);
void map_set_int32(map_t pmap, char *key, int32_t value);

bool map_get_string(map_t pmap, char *key, char **dest);
void map_set_string(map_t pmap, char *key, char *value);

bool map_get_uuid(map_t pmap, char *key, struct uuid_t *dest);
void map_set_uuid(map_t pmap, char *key, struct uuid_t value);

bool map_get_position(map_t pmap, char *key, struct position_t *dest);
void map_set_position(map_t pmap, char *key, struct position_t value);

bool map_get_boolean(map_t pmap, char *key, bool *dest);
void map_set_boolean(map_t pmap, char *key, bool value);

bool map_get_int16(map_t pmap, char *key, int16_t *dest);
void map_set_int16(map_t pmap, char *key, int16_t value);

bool map_get_float(map_t pmap, char *key, float *dest);
void map_set_float(map_t pmap, char *key, float value);

bool map_get_double(map_t pmap, char *key, double *dest);
void map_set_double(map_t pmap, char *key, double value);

#pragma endregion

//a byte array
#pragma region Buffer

buffer_t construct_buffer(void);
void buffer_free(buffer_t buf);

void buffer_append(buffer_t buf, uint8_t *data, size_t size);
void buffer_append_byte(buffer_t buf, uint8_t byte);
void buffer_clear(buffer_t buf);

uint8_t *buffer_get(buffer_t buf, size_t *length);

#pragma endregion

