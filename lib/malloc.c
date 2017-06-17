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

#include "malloc.h"
#include "types.h"

#ifndef MEMORY_SIZE
#define MEMORY_SIZE 0x8000
#endif

static u8 memory[MEMORY_SIZE];
static u8* pHeap = memory;
static u8* pEnd = &memory[MEMORY_SIZE];

/**
 * Allocate memory
 *
 * @param size is number of bytes to allocate
 * @return address
 */
void *malloc(size_t size) {
    u8* p = (u8*)ROUNDUP(pHeap, 8);
    u8* e = p + size;
    if (e >= pEnd) {
        return NULL;
    }
    pHeap = e;
    return p;
}

/**
 * Free memory
 *
 * @p is address of memory allocated with malloc
 */
void free(void *p) {
    UNUSED(p);
}
