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

#include "memset.h"
#include "types.h"

/**
 * Fill memory with val
 *
 * @param pDst is starting address
 * @param byte is used to fill memory, byte is cast to a u8
 * @param size is the number of bytes to write
 * @return pDst
 */
void *memset(void* pDst, int byte, size_t size) {
    u8* p = (u8*)pDst;
    u8* e = p + size;
    while(p < e) {
        *p++ = (u8)byte;
    }
    return pDst;
}
