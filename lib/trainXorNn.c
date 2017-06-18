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
#if !defined(DBG)
#define DBG 0
#endif

#include "trainXorNn.h"

#include "NeuralNet.h"
#include "dbg.h"
#include "rand0_1.h"
#include "calloc.h"
#include "types.h"

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


static InputPattern xor_input_patterns[] = {
  { .count = INPUT_COUNT, .data[0] = 0, .data[1] = 0 },
  { .count = INPUT_COUNT, .data[0] = 1, .data[1] = 0 },
  { .count = INPUT_COUNT, .data[0] = 0, .data[1] = 1 },
  { .count = INPUT_COUNT, .data[0] = 1, .data[1] = 1 },
};

static OutputPattern xor_target_patterns[] = {
  { .count = OUTPUT_COUNT, .data[0] = 0 },
  { .count = OUTPUT_COUNT, .data[0] = 1 },
  { .count = OUTPUT_COUNT, .data[0] = 1 },
  { .count = OUTPUT_COUNT, .data[0] = 0 },
};

static NeuralNet xorNn;

static OutputPattern xor_output[sizeof(xor_target_patterns)/sizeof(OutputPattern)];

/**
 * Train a Neural Network to solve an xor function where
 * two binary inputs are xor'd together to produce an output
 *
 * TODO: Evenutally this should accept a NeuralNetwork and a
 * set of training patterns as parameters.
 *
 * @param epoch_count is maximum number of traning loops
 * @param error_threshold is the minimum error to determine when training
 *        is complete. A value of 0.0 will cause all training loops to be
 *        run. A value of 0.0004 is a decent number threshold and will take
 *        about 1500 to 1600 loops.
 * @param rand_seed is the seed for the random number generator.
 *
 * @returns error_threshold >= 0 and its the negative of STATUS_xxx code if < 0.
 */
f64 trainXorNn(u64 epoch_count, f64 error_threshold, u64 rand_seed) {
  Status status = 0;
  u64 epoch = 0;
  f64 error = 0.0;

  dbg("trainXorNn:+ epoch_count=%" PRIu64 " error_threshold=%lf rand_seed=%" PRIu64 "\n",
          epoch_count, error_threshold, rand_seed);

  srand0_1(
      (rand_seed >> (64-16)) & 0xffff,
      (rand_seed >> (64-32)) & 0xffff,
      (rand_seed >> (64-48)) & 0xffff,
      (rand_seed >> (64-64)) & 0xffff
  );

  u64 num_inputs = 2;
  u64 num_hidden = 1;
  u64 num_outputs = 1;
  status = NeuralNet_init(&xorNn, num_inputs, num_hidden, num_outputs);
  if (StatusErr(status)) goto done;

  // Each hidden layer is fully connected plus a bias
  u64 hidden_neurons = 2;
  status = xorNn.add_hidden(&xorNn, hidden_neurons);
  if (StatusErr(status)) goto done;

  status = xorNn.start(&xorNn);
  if (StatusErr(status)) goto done;

  unsigned int pattern_count = sizeof(xor_input_patterns)/sizeof(InputPattern);
  unsigned int* rand_ps = calloc(pattern_count, sizeof(unsigned int));

  for (epoch = 0; epoch < epoch_count; epoch++) {
    error = 0.0;

    // Shuffle rand_patterns by swapping the current
    // position t with a random location after the
    // current position.

    // Start by resetting to sequential order
    for (unsigned int p = 0; p < pattern_count; p++) {
      rand_ps[p] = p;
    }

    // Shuffle
    for (unsigned int p = 0; p < pattern_count; p++) {
      f64 r0_1 = rand0_1();
      unsigned int rp = p + (unsigned int)(r0_1 * (pattern_count - p));
      unsigned t = rand_ps[p];
      rand_ps[p] = rand_ps[rp];
      rand_ps[rp] = t;
      //dbg("r0_1=%lf rp=%d rand_ps[%d]=%d\n", r0_1, rp, p, rand_ps[p]);
    }

    // Process the pattern and accumulate the error
    for (unsigned int rp = 0; rp < pattern_count; rp++) {
      unsigned int p = rand_ps[rp];
      xorNn.set_inputs(&xorNn, (Pattern*)&xor_input_patterns[p]);
      xorNn.process(&xorNn);
      xor_output[p].count = OUTPUT_COUNT;
      xorNn.get_outputs(&xorNn, (Pattern*)&xor_output[p]);
      error += xorNn.adjust_weights(&xorNn, (Pattern*)&xor_output[p],
          (Pattern*)&xor_target_patterns[p]);
    }

    // Stop if we've reached the error_threshold
    if (error < error_threshold) {
      break;
    }
  }

  status = 0;

done:
  xorNn.deinit(&xorNn);

  if (status > 0) {
      error_threshold = -status;
  }
  dbg("trainXorNn:- error_threshold=%lf\n", error_threshold);
  return error_threshold;
}
