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

#include "calloc.h"
#include "malloc.h"
#include "memset.h"
#include "types.h"

/**
 * Allocate memory and zero.
 * 
 * @param nitems is number of items to allocate and clear
 * @param size is size of each element
 * @return address and the memory zeroed
 */
void *calloc(size_t nitems, size_t size) {
    u8* p = malloc(nitems * size);
    if (p != NULL) {
        memset(p, 0, nitems * size);
    }
    return p;
}
