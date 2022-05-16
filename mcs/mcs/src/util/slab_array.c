#include "util/slab_array.h"

#define ARRAY_TYPE struct slab_t
#define ARRAY_NAME slab
#define ARRAY_IMPLEMENTATION

#include "util/template/array.inc"
#undef ARRAY_TYPE
#undef ARRAY_NAME
#undef ARRAY_DEFINITIONS
