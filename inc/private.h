/*
 * Copyright 2017 Wink Saville
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * Modeled after freeBSD math_private.h
 *
 * This is only what's needed to compile our files.
 */
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
