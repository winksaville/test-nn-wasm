/*  Written in 2016 by David Blackman and Sebastiano Vigna (vigna@acm.org)

To the extent possible under law, the author has dedicated all copyright
and related and neighboring rights to this software to the public domain
worldwide. This software is distributed without any warranty.

See <http://creativecommons.org/publicdomain/zero/1.0/>. */

/**
 * Original code from:
 * http://xoroshiro.di.unimi.it/xoroshiro128plus.c
 */

#include "./xoroshiro128plus.h"

/* This is the successor to xorshift128+. It is the fastest full-period
   generator passing BigCrush without systematic failures, but due to the
   relatively short period it is acceptable only for applications with a
   mild amount of parallelism; otherwise, use a xorshift1024* generator.

   Beside passing BigCrush, this generator passes the PractRand test suite
   up to (and included) 16TB, with the exception of binary rank tests,
   which fail due to the lowest bit being an LFSR; all other bits pass all
   tests. We suggest to use a sign test to extract a random Boolean value.
   
   Note that the generator uses a simulated rotate operation, which most C
   compilers will turn into a single instruction. In Java, you can use
   Long.rotateLeft(). In languages that do not make low-level rotation
   instructions accessible xorshift128+ could be faster.

   The state must be seeded so that it is not everywhere zero. If you have
   a 64-bit seed, we suggest to seed a splitmix64 generator and use its
   output to fill s. */

#include <sizedtypes.h>

#define GLOBAL_INITIALIZER   { 0x853c49e6748fea9bULL, 0xda3e39cb94b95bdbULL }

static u64 s[2] = GLOBAL_INITIALIZER;

static inline u64 rotl(const u64 x, int k) {
	return (x << k) | (x >> (64 - k));
}

void xoroshiro128plus_srand(u64 v1, u64 v2) {
    s[0] = v1;
    s[1] = v2;
}

u64 xoroshiro128plus_rand(void) {
	const u64 s0 = s[0];
	u64 s1 = s[1];
	const u64 result = s0 + s1;

	s1 ^= s0;
	s[0] = rotl(s0, 55) ^ s1 ^ (s1 << 14); // a, b
	s[1] = rotl(s1, 36); // c

	return result;
}
