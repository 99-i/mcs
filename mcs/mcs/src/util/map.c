#include "util/map.h"
#include <stdlib.h>
#include "types.h"

map_value mv_i8(i8 i8)
{
	return (map_value) {
		.i8 = i8,
		.type = MAPTYPE_I8
	};
}
map_value mv_i16(i16 i16)
{
	return (map_value) {
		.i16 = i16,
		.type = MAPTYPE_I16
	};
}
map_value mv_i32(i32 i32)
{

	return (map_value) {
		.i32 = i32,
		.type = MAPTYPE_I32
	};
}
map_value mv_i64(i64 i64)
{
	return (map_value) {
		.i64 = i64,
		.type = MAPTYPE_I64
	};

}
map_value mv_u8(u8 u8)
{
	return (map_value) {
		.u8 = u8,
		.type = MAPTYPE_U8
	};

}
map_value mv_u16(u16 u16)
{
	return (map_value) {
		.u16 = u16,
		.type = MAPTYPE_U16
	};

}
map_value mv_u32(u32 u32)
{
	return (map_value) {
		.u32 = u32,
		.type = MAPTYPE_U32
	};

}
map_value mv_u64(u64 u64)
{
	return (map_value) {
		.u64 = u64,
		.type = MAPTYPE_U64
	};

}
map_value mv_float(float f)
{
	return (map_value) {
		.f = f,
		.type = MAPTYPE_FLOAT
	};

}
map_value mv_double(double d)
{
	return (map_value) {
		.d = d,
		.type = MAPTYPE_DOUBLE
	};

}
map_value mv_uuid(struct uuid_t uuid)
{
	return (map_value) {
		.uuid = uuid,
		.type = MAPTYPE_UUID
	};

}
map_value mv_str(str str)
{
	return (map_value) {
		.str = str,
		.type = MAPTYPE_STRING
	};

}
map_value mv_pos(struct position_t pos)
{
	return (map_value) {
		.pos = pos,
		.type = MAPTYPE_I8
	};

}

typedef struct
{
	i32 fields_size;
	struct map_field
	{
		str key;
		map_value value;
	} *fields;
} map_t;

map map_construct()
{
	map_t* m = mcsalloc(sizeof(map_t));
	m->fields = 0;
	m->fields_size = 0;

	return m;
}

void map_set(map m, str s, map_value mv)
{
	map_t* real = (map_t*)m;
	i32 i;
	for (i = 0; i < real->fields_size; i++)
	{
		if (streq_str(real->fields[i].key, s))
		{
			real->fields[i].value = mv;
			return;
		}
	}

	if (!real->fields_size)
	{
		real->fields_size = 1;
		real->fields = mcsalloc(sizeof(struct map_field));
	}
	else
	{
		real->fields_size += 1;
		real->fields = mcsrealloc(real->fields, ((u64)sizeof(struct map_field)) * real->fields_size);
	}
	real->fields[real->fields_size - 1].key = str_clone_str(s);
	real->fields[real->fields_size - 1].value = mv;
}

map_value map_get_str(map m, str s)
{
	map_t* real = (map_t*)m;
	i32 i;
	for (i = 0; i < real->fields_size; i++)
	{
		if (streq_str(real->fields[i].key, s))
		{
			return real->fields[i].value;
		}
	}
	return mv_u64(-1);
}

map_value map_get_cstr(map m, const char* s)
{
	map_t* real = (map_t*)m;
	i32 i;
	for (i = 0; i < real->fields_size; i++)
	{
		if (streq_cstr(real->fields[i].key, s))
		{
			return real->fields[i].value;
		}
	}
	return mv_u8(-1);
}
void map_destroy(map m)
{
	map_t* real = (map_t*)m;
	i32 i;
	for (i = 0; i < real->fields_size; i++)
	{
		if(real->fields->value.type == MAPTYPE_STRING)
		{
			str_destroy(&real->fields[i].value.str);
		}
		str_destroy(&real->fields[i].key);
	}
	if(real->fields_size != 0)
	{
		mcsfree(real->fields);
	}
	mcsfree(real);
}
