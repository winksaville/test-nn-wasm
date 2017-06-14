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

#include "dbg.h"
#include "rand0_1.h"
#include <stdio.h>
#include <stdlib.h>

#define debugging 0

#if debugging
static int rand_idx = 0;
static double rand_nums[] = {
    0.840188,
    0.394383,
    0.783099,
    0.798440,
    0.911647,
    0.197551,
    0.335223,
    0.768230,
    0.277775,
    0.553970,
    0.477397,
    0.628871,
    0.364784,
    0.513401,
    0.952230,
    0.916195,
    0.635712
};
#endif

double rand0_1(void) {
    double v;
#if !debugging
    double r = rand();
    v = r/((double)RAND_MAX+1);
#else
    v = rand_nums[rand_idx++];
    if (rand_idx >= sizeof(rand_nums)/sizeof(rand_nums[0])) {
        dbg("rand0_1=overlowed\n");
        rand_idx = 0;
    }
#endif
    dbg("rand0_1=%lf\n", v);
    return v;
}
