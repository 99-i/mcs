#pragma once
#include <stdint.h>
#include <stdbool.h>


typedef void *map_t;
typedef void *buffer_t;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

void slurp_file_to_cstr(char *filepath, char **dest);

//an associative array between char*'s and any value
#pragma region Map

map_t construct_map(void);
void map_free(map_t pmap);
void map_destroy(map_t pmap, char* key);
void map_set(map_t pmap, char *key, void *value);
bool map_get(map_t pmap, char *key, void **dest);


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

