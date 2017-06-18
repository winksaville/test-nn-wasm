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
#include "trainXorNn.h"

#include <errno.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <locale.h>

int main(int argc, char** argv) {
  Status status;
  u64 epoch = 0;
  u64 epoch_count = 0;
  f64 error = 0.0;
  f64 error_threshold = 0.0004;

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
    f64 count = floor(error_threshold);
    if (count > ULONG_MAX) {
      printf("param1:%'lg > %'lg:%'lu, aborting\n", count, (f64)ULONG_MAX, ULONG_MAX);
      status = STATUS_ERR;
      goto donedone;
    }
    epoch_count = (u64)count;
    error_threshold = 0.0;
  }


  char* out_path = "";
  if (argc == 3) {
    out_path = argv[2];
  }

  dbg("test-nn: epoch_count=%" PRIu64 " out_pat='%s'\n", epoch_count, out_path);

  // seed the random number generator
#if 0
  struct timespec spec;
  clock_gettime(CLOCK_REALTIME, &spec);
  f64 dnow_us = (((f64)spec.tv_sec * 1.0e9) + spec.tv_nsec) / 1.0e3;
  int now = (int)(long)dnow_us;
  dbg("dnow_us=%lf now=0x%x\n", dnow_us, now);
  srand(now);
#else
  srand(1);
#endif

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

  unsigned int pattern_count = INPUT_PATTERNS_COUNT;

  if (strlen(out_path) > 0) {
    writer = calloc(1, sizeof(NeuralNetIoWriter));
    status = NeuralNetIoWriter_init(writer, &xorNn, xorNn.get_points(&xorNn), out_path);
    if (StatusErr(status)) goto done;
  } else {
    writer = NULL;
  }

  struct timeval start;
  gettimeofday(&start, NULL);
  status = trainXorNn(epoch_count, error_threshold, 1, 2, 3, 4);
  if (StatusErr(status)) goto done;
  f64 epochs = getEpochs();
  epoch = (u64)epochs;
  error = getError();
  struct timeval end;
  gettimeofday(&end, NULL);

  f64 start_usec = (start.tv_sec * 1000000.0) + start.tv_usec;
  f64 end_usec = (end.tv_sec * 1000000.0) + end.tv_usec;
  f64 time_sec = (end_usec - start_usec) / 1000000;
  u64 eps = (u64)(epoch / time_sec);

  printf("\n\nEpoch=%'" PRIu64 " error=%.3lg time=%.3lfs eps=%'" PRIu64 "\n",
          epoch, error, time_sec, eps);

  xorNn.stop(&xorNn);

  printf("\nPat");
  for (u64 i = 0; i < xor_input_patterns[0].count; i++) {
    printf("\tInput%-4" PRIu64 "", i);
  }
  for (u64 t = 0; t < xor_target_patterns[0].count; t++) {
    printf("\tTarget%-4" PRIu64 "", t);
  }
  for (u64 o = 0; o < xor_output[0].count; o++) {
    printf("\tOutput%-4" PRIu64 "", o);
  }
  printf("\n");
  for (u64 p = 0; p < pattern_count; p++) {
    printf("%" PRIu64 "", p);
    for (u64 i = 0; i < xor_input_patterns[p].count; i++) {
      printf("\t%lf", xor_input_patterns[p].data[i]);
    }
    for (u64 t = 0; t < xor_target_patterns[p].count; t++) {
      printf("\t%lf", xor_target_patterns[p].data[t]);
    }
    for (u64 o = 0; o < xor_output[p].count; o++) {
      printf("\t%lf", xor_output[p].data[o]);
    }
    printf("\n");

  }


done:
  if (writer != NULL) {
    writer->deinit(writer, epoch);
  }
  xorNn.deinit(&xorNn);

donedone:
  dbg("test-nn:- status=%d\n", status);
  return 0;
}
