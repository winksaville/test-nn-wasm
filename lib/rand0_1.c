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

#include "rand0_1.h"
#include "./xoroshiro128plus.h"
#include "sizedtypes.h"

void srand0_1(u32 sr1, u32 sr2, u32 sr3, u32 sr4) {
    xoroshiro128plus_srand(((u64)sr1 << 32) | sr2, ((u64)sr3 << 32) | sr4);
}

double rand0_1(void) {
    double v;
    u64 r = xoroshiro128plus_rand();

#if (RAND_MAX > (1ULL << 53))
    // As suggested by Sebastiano "Generating uniform doubles in the unit interval"
    // at http://xoroshiro.di.unimi.it
    v = (r >> 11) * (1.0 / ((u64)1 << 53));
#else
    v = r * (1.0 / (RAND_MAX + 1.0));
#endif
    return v;
}
