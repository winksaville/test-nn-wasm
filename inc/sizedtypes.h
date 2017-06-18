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

typedef __INT64_TYPE__ i64;
typedef __INT32_TYPE__ i32;
typedef __INT16_TYPE__ i16;
typedef __INT8_TYPE__  i8;

typedef __UINT64_TYPE__ u64;
typedef __UINT32_TYPE__ u32;
typedef __UINT16_TYPE__ u16;
typedef __UINT8_TYPE__  u8;

#define PRId64 __INT64_FMTd__
#define PRId32 __INT32_FMTd__
#define PRId16 __INT16_FMTd__
#define PRId8  __INT8_FMTd__

#define PRIu64 __UINT64_FMTu__
#define PRIu32 __UINT32_FMTu__
#define PRIu16 __UINT16_FMTu__
#define PRIu8  __UINT8_FMTu__


typedef double f64;
typedef float f32;

#endif
