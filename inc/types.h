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

#endif
