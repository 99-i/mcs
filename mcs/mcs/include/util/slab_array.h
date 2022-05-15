#pragma once

#include "def.h"
#include "string.h"
#include "slab_field_array.h"

#define ARRAY_TYPE struct slab_t
#define ARRAY_NAME slab
#define ARRAY_DEFINITIONS

#include "template/array.h"

#undef ARRAY_DEFINITIONS
