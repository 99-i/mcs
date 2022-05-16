#pragma once
#include "def.h"
#include "string.h"
#include "slab_field_array.h"
struct slab_t
{
	str name;
	uint32_t id;
	enum estate state;
	enum epacket_direction direction;
	slab_field_array fields;
};

#define ARRAY_TYPE struct slab_t
#define ARRAY_NAME slab
#define ARRAY_DEFINITIONS
#include "template/array.inc"

#undef ARRAY_DEFINITIONS
