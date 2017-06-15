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

#include "limits.h"
#include "./xoroshiro128plus.h"
#include "./pcg_basic.h"
#include "rand0_1.h"

double rand0_1(void) {
    double v;
#if 1
    #define RAND_MAX xoroshiro128plus_rand_max
    uint64_t r = xoroshiro128plus_rand();
#else
    #define RAND_MAX pcg32_rand_max
    uint64_t r = pcg32_random();
#endif

#if (RAND_MAX > (1ULL << 53))
    // As suggested by Sebastiano "Generating uniform doubles in the unit interval"
    // at http://xoroshiro.di.unimi.it
    v = (r >> 11) * (1.0 / ((uint64_t)1 << 53));
#else
    v = r * (1.0 / (RAND_MAX + 1.0));
#endif
    return v;
}
