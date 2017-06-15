#ifndef PRIVATE_H
#define PRIVATE_H

#include "sizedtypes.h"

typedef union {
    double value;
    struct {
        u32 lo;
        u32 hi;
    } parts;
    struct {
        u64 w;
    } xparts;
} ieee_double;

#define GET_LOW_WORD(i, d) do { \
    ieee_double dbl; \
    dbl.value = (d); \
    (i) = dbl.parts.lo; \
} while (0)

#define GET_HIGH_WORD(i, d) do { \
    ieee_double dbl; \
    dbl.value = (d); \
    (i) = dbl.parts.hi; \
} while (0)

#define INSERT_WORDS(d, h, l) do { \
    ieee_double dbl; \
    dbl.parts.hi = (u32)(h); \
    dbl.parts.lo = (u32)(l); \
    (d) = dbl.value; \
} while(0)

#define STRICT_ASSIGN(type, lhs, rhs) (lhs) = (rhs)

#endif
