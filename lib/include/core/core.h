#ifndef _CORE_H_
#define _CORE_H_

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <bits/types.h>

#define ccast(type, val) reinterpret_cast<type>(val)

typedef __int8_t i8;
typedef __uint8_t u8;
typedef __int16_t i16;
typedef __uint16_t u16;
typedef __int32_t i32;
typedef __uint32_t u32;
typedef __int64_t i64;
typedef __uint64_t u64;


i32 check_byte_order();

#endif