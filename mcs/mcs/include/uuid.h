#pragma once
#include "util/def.h"
#undef uuid_t
struct uuid_t
{
	u64 high;
	u64 low;
};
