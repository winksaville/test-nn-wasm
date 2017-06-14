/*
 * Copyright 2016 Wink Saville
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

#include "NeuralNet.h"
#include "NeuralNetIo.h"
#include "dbg.h"
#include "rand0_1.h"

#include <errno.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <locale.h>

#define INPUT_COUNT 2
typedef struct InputPattern {
  unsigned long count;
  double data[INPUT_COUNT];
} InputPattern;

#define OUTPUT_COUNT 1
typedef struct OutputPattern {
  unsigned long count;
  double data[OUTPUT_COUNT];
} OutputPattern;

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

static NeuralNet nn;

static OutputPattern xor_output[sizeof(xor_target_patterns)/sizeof(OutputPattern)];

int main(int argc, char** argv) {
  Status status;
  unsigned long epoch = 0;
  unsigned long epoch_count = 0;
  double error = 0.0;
  double error_threshold = 0.0004;

  NeuralNetIoWriter *writer = NULL;

  setlocale(LC_NUMERIC, "");

  dbg("test-nn:+\n");

  if (argc < 2) {
    printf("Usage: %s <param1> <file>\n", argv[0]);
    printf("  param1: if param1 >= 1 then number of epochs\n");
    printf("          else if param1 >= 0.0 && param1 < 1.0 then error threshold typical = 0.0004\n");
    printf("          else param1 invalid\n");
    printf("  file:   output file, optional\n");
    status = STATUS_ERR;
    goto donedone;
  }

  error_threshold = strtod(argv[1], NULL);
  if (error_threshold < 0.0) {
    printf("param1:%'lg is < 0.0, aborting\n", error_threshold);
    status = STATUS_ERR;
    goto donedone;
  } else if ((error_threshold > 0.0) && (error_threshold < 1.0)) {
    epoch_count = ULONG_MAX;
  } else {
    double count = floor(error_threshold);
    if (count > ULONG_MAX) {
      printf("param1:%'lg > %'lg:%'lu, aborting\n", count, (double)ULONG_MAX, ULONG_MAX);
      status = STATUS_ERR;
      goto donedone;
    }
    epoch_count = (unsigned long)count;
    error_threshold = 0.0;
  }


  char* out_path = "";
  if (argc == 3) {
    out_path = argv[2];
  }

  dbg("test-nn: epoch_count=%ld out_pat='%s'\n", epoch_count, out_path);

  // seed the random number generator
#if 0
  struct timespec spec;
  clock_gettime(CLOCK_REALTIME, &spec);
  double dnow_us = (((double)spec.tv_sec * 1.0e9) + spec.tv_nsec) / 1.0e3;
  int now = (int)(long)dnow_us;
  dbg("dnow_us=%lf now=0x%x\n", dnow_us, now);
  srand(now);
#else
  srand(1);
#endif

  unsigned long num_inputs = 2;
  unsigned long num_hidden = 1;
  unsigned long num_outputs = 1;
  status = NeuralNet_init(&nn, num_inputs, num_hidden, num_outputs);
  if (StatusErr(status)) goto done;

  // Each hidden layer is fully connected plus a bias
  unsigned long hidden_neurons = 2;
  status = nn.add_hidden(&nn, hidden_neurons);
  if (StatusErr(status)) goto done;

  status = nn.start(&nn);
  if (StatusErr(status)) goto done;

  unsigned int pattern_count = sizeof(xor_input_patterns)/sizeof(InputPattern);
  unsigned int* rand_ps = calloc(pattern_count, sizeof(unsigned int));

  if (strlen(out_path) > 0) {
    writer = calloc(1, sizeof(NeuralNetIoWriter));
    status = NeuralNetIoWriter_init(writer, &nn, nn.get_points(&nn), out_path);
    if (StatusErr(status)) goto done;
  } else {
    writer = NULL;
  }

  struct timeval start;
  gettimeofday(&start, NULL);
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
      double r0_1 = rand0_1();
      unsigned int rp = p + (unsigned int)(r0_1 * (pattern_count - p));
      unsigned t = rand_ps[p];
      rand_ps[p] = rand_ps[rp];
      rand_ps[rp] = t;
      //dbg("r0_1=%lf rp=%d rand_ps[%d]=%d\n", r0_1, rp, p, rand_ps[p]);
    }

    // Process the pattern and accumulate the error
    for (unsigned int rp = 0; rp < pattern_count; rp++) {
      unsigned int p = rand_ps[rp];
      nn.set_inputs(&nn, (Pattern*)&xor_input_patterns[p]);
      nn.process(&nn);
      xor_output[p].count = OUTPUT_COUNT;
      nn.get_outputs(&nn, (Pattern*)&xor_output[p]);
      error += nn.adjust_weights(&nn, (Pattern*)&xor_output[p],
          (Pattern*)&xor_target_patterns[p]);

      if (writer != NULL) {
        writer->begin_epoch(writer, (epoch * pattern_count) + rp);
        writer->write_epoch(writer);
        writer->end_epoch(writer);
      }
    }

    // Stop if we've reached the error_threshold
    if (error < error_threshold) {
      break;
    }
  }
  struct timeval end;
  gettimeofday(&end, NULL);

  double start_usec = (start.tv_sec * 1000000.0) + start.tv_usec;
  double end_usec = (end.tv_sec * 1000000.0) + end.tv_usec;
  double time_sec = (end_usec - start_usec) / 1000000;
  unsigned long eps = (unsigned long)(epoch / time_sec);

  printf("\n\nEpoch=%'ld Error=%.3lg time=%.3lfs eps=%'ld\n", epoch, error, time_sec, eps);

  nn.stop(&nn);

  printf("\nPat");
  for (unsigned long i = 0; i < xor_input_patterns[0].count; i++) {
    printf("\tInput%-4ld", i);
  }
  for (unsigned long t = 0; t < xor_target_patterns[0].count; t++) {
    printf("\tTarget%-4ld", t);
  }
  for (unsigned long o = 0; o < xor_output[0].count; o++) {
    printf("\tOutput%-4ld", o);
  }
  printf("\n");
  for (unsigned long p = 0; p < pattern_count; p++) {
    printf("%ld", p);
    for (unsigned long i = 0; i < xor_input_patterns[p].count; i++) {
      printf("\t%lf", xor_input_patterns[p].data[i]);
    }
    for (unsigned long t = 0; t < xor_target_patterns[p].count; t++) {
      printf("\t%lf", xor_target_patterns[p].data[t]);
    }
    for (unsigned long o = 0; o < xor_output[p].count; o++) {
      printf("\t%lf", xor_output[p].data[o]);
    }
    printf("\n");

  }


done:
  if (writer != NULL) {
    writer->deinit(writer, epoch);
  }
  nn.deinit(&nn);

donedone:
  dbg("test-nn:- status=%d\n", status);
  return 0;
}
