#pragma once

#include <stdint.h>
#include <stdbool.h>
#define mcsalloc(s) mcsalloc_(s, __FUNCTION__)
#define mcsrealloc(b, s) mcsrealloc_(b, s, __FUNCTION__)
#define mcsfree(s) mcsfree_(s, __FUNCTION__)
typedef uint8_t		u8;
typedef uint16_t	u16;
typedef uint32_t	u32;
typedef uint64_t	u64;

typedef int8_t		i8;
typedef int16_t		i16;
typedef int32_t		i32;
typedef int64_t		i64;

void* mcsalloc_(i32 s);
void *mcsrealloc_(void *block);
void mcsfree_(void *block);
