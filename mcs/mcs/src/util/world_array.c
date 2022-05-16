#include "util/world_array.h"

#define ARRAY_TYPE struct world_t
#define ARRAY_NAME world
#define ARRAY_IMPLEMENTATION

#include "util/template/array.inc"
#undef ARRAY_TYPE
#undef ARRAY_NAME
#undef ARRAY_DEFINITIONS