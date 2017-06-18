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

#ifndef TYPES_H
#define TYPES_H

#include "sizedtypes.h"

#ifndef size_t
typedef __SIZE_TYPE__ size_t;
#endif

#ifndef uintptr_t
typedef __UINTPTR_TYPE__ uintptr_t;
#endif

#ifndef ptrdiff_t
typedef __PTRDIFF_TYPE__ ptrdiff_t;
#endif

#ifndef NULL
#define NULL ((void*)0)
#endif

#ifndef UNUSED
#define UNUSED(s) (void)s
#endif

typedef u32 Status;
#define STATUS_OK  0 ///< OK
#define STATUS_ERR 1 ///< Error
#define STATUS_OOM 2 ///< Out of memory
#define STATUS_BAD_PARAM 3 ///< Bad parameter
#define STATUS_BAD_CODE 4  ///< A bug in the code

#define STATUS_TO_MANY_HIDDEN 100 ///< To many calls to NeuralNet_add_hidden

/** Evaluates to true if status is good */
#define StatusOk(s) ((s) == STATUS_OK)

/** Evaluates to false if status is bad */
#define StatusErr(s) (!StatusOk(s))

/** Evaluates to the status integer value */
#define StatusVal(s) (s)

/**
 * Round up v to a multiple of size. It is assumed if size is a
 * power of two the compiler will optimize to a constant or an
 * add anded with a mask.
 */
#define ROUNDUP(v, size) ((((u64)v + size - 1) / (u64)size) * (u64)size)

/**
 * Align to the type and cast return value to (typeof(type)*)
 */
#define ALIGNTO_TYPE(addr, type) (typeof(type)*)ROUNDUP(addr, sizeof(type))

/**
 * NAN returns a double
 */
#ifndef NAN
#define NAN __builtin_nan("")
#endif

/**
 * NANf returns a float
 */
#ifndef NANF
#define NANF __builtin_nanf("")
#endif

#endif
