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

#include "sizedtypes.h"
#include "rand0_1.h"

//#include "./xoroshiro128plus.h"
//#include "./pcg_basic.h"

// wasm-link doesn't work if there are globals just
// so just add the xoroshiro128plus.c code here

#define xoroshiro128plus_rand_max 0xFFFFFFFFFFFFFFFFULL

#define GLOBAL_INITIALIZER   { 0x853c49e6748fea9bULL, 0xda3e39cb94b95bdbULL }

static u64 s[2] = GLOBAL_INITIALIZER;

static inline u64 rotl(const u64 x, int k) {
	return (x << k) | (x >> (64 - k));
}

static void xoroshiro128plus_srand(u64 v1, u64 v2) {
    s[0] = v1;
    s[1] = v2;
}

static u64 xoroshiro128plus_rand(void) {
	const u64 s0 = s[0];
	u64 s1 = s[1];
	const u64 result = s0 + s1;

	s1 ^= s0;
	s[0] = rotl(s0, 55) ^ s1 ^ (s1 << 14); // a, b
	s[1] = rotl(s1, 36); // c

	return result;
}

void srand0_1(u32 v1, u32 v2, u32 v3, u32 v4) {
    xoroshiro128plus_srand(((u64)v1 << 32) | v2, ((u64)v3 << 32) | v4);
}

double rand0_1(void) {
    double v;
    #define RAND_MAX xoroshiro128plus_rand_max
    u64 r = xoroshiro128plus_rand();
    //#define RAND_MAX pcg32_rand_max
    //u64 r = pcg32_rand();

#if (RAND_MAX > (1ULL << 53))
    // As suggested by Sebastiano "Generating uniform doubles in the unit interval"
    // at http://xoroshiro.di.unimi.it
    v = (r >> 11) * (1.0 / ((u64)1 << 53));
#else
    v = r * (1.0 / (RAND_MAX + 1.0));
#endif
    return v;
}
