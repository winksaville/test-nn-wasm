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

#ifndef TRAINXORNN_H
#define TRAINXORNN_H

#include "types.h"
#include "NeuralNet.h"

#define INPUT_PATTERNS_COUNT 4

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpadded"
#define INPUT_COUNT 2
typedef struct InputPattern {
  u64 count;
  f64 data[INPUT_COUNT];
} InputPattern;

#define OUTPUT_COUNT 1
typedef struct OutputPattern {
  u64 count;
  f64 data[OUTPUT_COUNT];
} OutputPattern;
#pragma clang diagnostic pop

extern InputPattern xor_input_patterns[];
extern OutputPattern xor_target_patterns[];
extern OutputPattern xor_output[];

extern NeuralNet xorNn;

u32 trainXorNn(f64 epoch_count, f64 error_threshold,
        u32 sr1, u32 sr2, u32 sr3, u32 sr4);

f64 getError(void);
f64 getEpochs(void);

#endif
