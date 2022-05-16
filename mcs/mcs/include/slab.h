#pragma once
#include "util/slab_field_array.h"
#include "util/slab_array.h"
enum efield_type
{
	FT_VARINT,
	FT_UNSIGNED_SHORT,
	FT_UNSIGNED_BYTE,
	FT_LONG,
	FT_UUID,
	FT_BOOLEAN,
	FT_DOUBLE,
	FT_ANGLE,
	FT_INT,
	FT_SHORT,
	FT_POSITION,
	FT_BYTE,
	FT_FLOAT,
	FT_STRING,
	FT_VARLONG,
};

struct slabinfo_t
{
	slab_array slabs;
};
