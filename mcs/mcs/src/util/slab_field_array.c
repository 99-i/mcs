#include "util/slab_field_array.h"

#define ARRAY_TYPE struct field_t
#define ARRAY_NAME slab_field
#define ARRAY_IMPLEMENTATION

#include "util/template/array.inc"
#undef ARRAY_TYPE
#undef ARRAY_NAME
#undef ARRAY_DEFINITIONS