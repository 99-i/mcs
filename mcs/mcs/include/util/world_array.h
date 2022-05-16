#pragma once

#include "def.h"
#include "string.h"
#include "world.h"

#define ARRAY_TYPE struct world_t
#define ARRAY_NAME world
#define ARRAY_DEFINITIONS

#include "template/array.inc"

#undef ARRAY_TYPE
#undef ARRAY_NAME
#undef ARRAY_DEFINITIONS
