#ifndef SIZEDTYPES_H
#define SIZEDTYPES_H

#define I64_MAX 0x7FFFFFFFFFFFFFFFLL
#define I32_MAX 0x7FFFFFFF
#define I16_MAX 0x7FFF
#define I8_MAX  0x7F

#define I64_MIN 0xFFFFFFFFFFFFFFFFLL
#define I32_MIN 0xFFFFFFFF
#define I16_MIN 0xFFFF
#define I8_MIN  0xFF

#define U64_MAX 0xFFFFFFFFFFFFFFFFULL
#define U32_MAX 0xFFFFFFFF
#define U16_MAX 0xFFFF
#define U8_MAX  0xFF

#define U64_MIN 0x0ULL
#define U32_MIN 0x0
#define U16_MIN 0x0
#define U8_MIN  0x0

typedef long long i64;
typedef int i32;
typedef short i16;
typedef char i8;

typedef unsigned long long u64;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

typedef double f64;
typedef float f32;

#endif
