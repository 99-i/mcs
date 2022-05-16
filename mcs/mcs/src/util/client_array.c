#include "util/client_array.h"

#define ARRAY_TYPE struct client_t*
#define ARRAY_NAME client
#define ARRAY_IMPLEMENTATION

#include "util/template/array.inc"
#undef ARRAY_TYPE
#undef ARRAY_NAME
#undef ARRAY_DEFINITIONS
