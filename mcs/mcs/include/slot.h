#pragma once
#include <stdbool.h>
#include "util/def.h"

struct slot_t
{
	bool present;
	i32 item_id;
	i8 item_count;
};
