#pragma once

#include "def.h"
#include "string.h"


#define ARRAY_TYPE struct field_t
#define ARRAY_NAME slab_field
#define ARRAY_DEFINITIONS

#include "template/array.inc"

#undef ARRAY_DEFINITIONS

