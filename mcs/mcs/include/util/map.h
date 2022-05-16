#pragma once

#include "util/string.h"
#include "util/def.h"
#undef uuid_t
#include "uuid.h"
#include "position.h"
enum emap_type
{
	MAPTYPE_I8,
	MAPTYPE_I16,
	MAPTYPE_I32,
	MAPTYPE_I64,
	MAPTYPE_U8,
	MAPTYPE_U16,
	MAPTYPE_U32,
	MAPTYPE_U64,
	MAPTYPE_FLOAT,
	MAPTYPE_DOUBLE,
	MAPTYPE_UUID,
	MAPTYPE_STRING,
	MAPTYPE_POSITION
};
typedef struct map_value
{
	enum emap_type type;
	union
	{
		i8 i8;
		i16 i16;
		i32 i32;
		i64 i64;
		u8 u8;
		u16 u16;
		u32 u32;
		u64 u64;
		float f;
		double d;
		struct uuid_t uuid;
		struct position_t pos;
		str str;
	};
} map_value;

map_value mv_i8(i8 i8);
map_value mv_i16(i16 i16);
map_value mv_i32(i32 i32);
map_value mv_i64(i64 i64);
map_value mv_u8(u8 u8);
map_value mv_u16(u16 u16);
map_value mv_u32(u32 u32);
map_value mv_u64(u64 u64);
map_value mv_float(float f);
map_value mv_double(double d);
map_value mv_uuid(struct uuid_t uuid);
map_value mv_str(str str);
map_value mv_pos(struct position_t pos);

typedef void* map;

map map_construct();

void map_set(map m, str s, map_value mv);

map_value map_get(map m, str s);

void map_destroy(map m);


