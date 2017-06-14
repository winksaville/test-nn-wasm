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

#ifndef NEURAL_NET_IO_H
#define NEURAL_NET_IO_H

#include "NeuralNet.h"

#include <stdio.h>

typedef struct NeuralNetIoWriter NeuralNetIoWriter;

typedef void (*NeuralNetIoWriter_deinit)(NeuralNetIoWriter* writer, unsigned long epochs);
typedef Status (*NeuralNetIoWriter_write_str)(NeuralNetIoWriter* writer, char* s);
typedef Status (*NeuralNetIoWriter_write_int)(NeuralNetIoWriter* writer, unsigned long i);
typedef Status (*NeuralNetIoWriter_write_float)(NeuralNetIoWriter* writer, double f);
typedef Status (*NeuralNetIoWriter_write_double)(NeuralNetIoWriter* writer, double d);
typedef Status (*NeuralNetIoWriter_write_point_val)(NeuralNetIoWriter* writer, double* f);
typedef Status (*NeuralNetIoWriter_open_file)(NeuralNetIoWriter* writer);
typedef Status (*NeuralNetIoWriter_close_file)(NeuralNetIoWriter* writer, unsigned long epochs);
typedef Status (*NeuralNetIoWriter_begin_epoch)(NeuralNetIoWriter* writer, size_t epoch);
typedef Status (*NeuralNetIoWriter_write_epoch)(NeuralNetIoWriter* writer);
typedef Status (*NeuralNetIoWriter_end_epoch)(NeuralNetIoWriter* writer);

typedef struct NeuralNetIoWriter {
  FILE* out_file;       // Output file
  NeuralNet* nn;        // Neural net
  char* out_path;       // output path

  // Methods
  NeuralNetIoWriter_deinit deinit;
  NeuralNetIoWriter_open_file open_file;
  NeuralNetIoWriter_close_file close_file;
  NeuralNetIoWriter_begin_epoch begin_epoch;
  NeuralNetIoWriter_write_epoch write_epoch;
  NeuralNetIoWriter_end_epoch end_epoch;
  NeuralNetIoWriter_write_str write_str;
  NeuralNetIoWriter_write_int write_int;
  NeuralNetIoWriter_write_float write_float;
  NeuralNetIoWriter_write_double write_double;
  NeuralNetIoWriter_write_point_val write_point_val;

} NeuralNetIoWriter;

Status NeuralNetIoWriter_init(NeuralNetIoWriter* writer, NeuralNet* nn,
    unsigned long points_per_epoch, char* out_path);


#endif
