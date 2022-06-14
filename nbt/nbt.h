#pragma once
#include <stdint.h>
#include <stdbool.h>

typedef enum
{
	TAG_BYTE = 1,
	TAG_SHORT,
	TAG_INT,
	TAG_LONG,
	TAG_FLOAT,
	TAG_DOUBLE,
	TAG_BYTE_ARRAY,
	TAG_STRING,
	TAG_LIST,
	TAG_COMPOUND,
	TAG_INT_ARRAY,
	TAG_LONG_ARRAY
} nbt_tag_type;

struct nbt_tag;

struct nbt_byte_array
{
	int32_t size;
	int8_t *bytes;
};

struct nbt_string
{
	uint16_t size;
	wchar_t *str;
};
struct nbt_list
{
	nbt_tag_type type;
	int32_t size;
	struct nbt_tag *tags;
};
struct npt_compound
{
	int32_t size;
	struct nbt_tag *tags;
};
struct nbt_int_array
{
	int32_t size;
	int32_t *ints;
};
struct nbt_long_array
{
	int32_t size;
	int64_t *longs;
};

struct nbt_tag
{
	nbt_tag_type type;
	char *name;
	union
	{
		int8_t					tag_byte;
		int16_t					tag_short;
		int32_t					tag_int;
		int64_t					tag_long;
		float					tag_float;
		double					tag_double;
		struct nbt_byte_array	tag_byte_array;
		struct nbt_string		tag_string;
		struct nbt_list			tag_list;
		struct npt_compound		tag_compound;
		struct nbt_int_array	tag_int_array;
		struct nbt_long_array	tag_long_array;
	};
};

typedef struct nbt_s
{
	struct npt_tag *main_compound;
} nbt;

nbt *read_nbt(const uint8_t *bytes, size_t size);


