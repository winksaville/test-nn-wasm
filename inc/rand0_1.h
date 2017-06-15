/*
 * Copyright 2016 Wink Saville
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

#ifndef RAND0_1_H
#define RAND0_1_H

#include "sizedtypes.h"

/**
 * @return a double, N, such that 0.0 >= N < 1.0
 */
#if 0
#define srand0_1(u32 v1, u32 v2, u32 v3, u32 v4) srand((unsigned int)(v1 + v2 + v3 + v4))
#define rand0_1(void) ((double)rand()/((double)RAND_MAX+1))
#else
extern void srand0_1(u32 v1, u32 v2, u32 v3, u32 v4);
extern f64 rand0_1(void);
#endif

#endif
