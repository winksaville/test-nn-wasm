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
#include "memory.h"
#include "types.h"

InputPattern xor_input_patterns[INPUT_PATTERNS_COUNT] = {
  { .count = INPUT_COUNT, .data[0] = 0, .data[1] = 0 },
  { .count = INPUT_COUNT, .data[0] = 1, .data[1] = 0 },
  { .count = INPUT_COUNT, .data[0] = 0, .data[1] = 1 },
  { .count = INPUT_COUNT, .data[0] = 1, .data[1] = 1 },
};

OutputPattern xor_target_patterns[INPUT_PATTERNS_COUNT] = {
  { .count = OUTPUT_COUNT, .data[0] = 0 },
  { .count = OUTPUT_COUNT, .data[0] = 1 },
  { .count = OUTPUT_COUNT, .data[0] = 1 },
  { .count = OUTPUT_COUNT, .data[0] = 0 },
};

NeuralNet xorNn;

OutputPattern xor_output[INPUT_PATTERNS_COUNT];

static f64 g_error = 0;
static f64 g_epochs = 0;

/**
 * Train a Neural Network to solve an xor function where
 * two binary inputs are xor'd together to produce an output
 *
 * TODO: Evenutally this should accept a NeuralNetwork and a
 * set of training patterns as parameters.
 *
 * @param epoch_count_f is maximum number of traning loops
 * @param error_threshold is the minimum error to determine when training
 *        is complete. A value of 0.0 will cause all training loops to be
 *        run. A value of 0.0004 is a decent number threshold and will take
 *        about 1500 to 1600 loops.
 * @param sr1 first u32 for random seed
 * @param sr2 second u32 for random seed
 * @param sr3 third u32 for random seed
 * @param sr4 fourth u32 for random seed
 * @returns status 0 == no error
 */
u32 trainXorNn(f64 epoch_count_f, f64 error_threshold,
        u32 sr1, u32 sr2, u32 sr3, u32 sr4) {
  Status status = 0;
  u64 epoch_count;
  u64 epoch = 0;
  f64 error = 0.0;
  unsigned int* rand_ps = NULL;

  // Only 53 bits percision in an f64
  epoch_count = (epoch_count_f >= (f64)((u64)1 << 53)) ? U64_MAX : (u64)epoch_count_f;

  dbg("trainXorNn:+ epoch_count=%" PRIu64 " error_threshold=%lf"
          "sr1=%" PRIu32 "sr2=%" PRIu32 "sr3=%" PRIu32 "sr4=%" PRIu32,
          epoch_count, error_threshold, sr1, sr2, sr3, sr4);

  srand0_1(sr1, sr2, sr3, sr4);

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
  rand_ps = calloc(pattern_count, sizeof(unsigned int));

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

  g_error = error;
  g_epochs = epoch;

  status = 0;

done:
  xorNn.deinit(&xorNn);
  free(rand_ps);

  dbg("trainXorNn:- status=%" PRIu32 " error_threshold=%lf\n", status, error_threshold);
  return status;
}

f64 getError(void) {
    return g_error;
}

f64 getEpochs(void) {
    return g_epochs;
}
