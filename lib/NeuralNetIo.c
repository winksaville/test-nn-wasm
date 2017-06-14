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

#include "NeuralNet.h"
#include "NeuralNetIo.h"
#include "unused.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

static Status write_str(NeuralNetIoWriter* writer, char* data) {
  Status status;

  fputs(data, writer->out_file);
  if (ferror(writer->out_file)) {
     printf("NeuralNetIo.write_str: %s\n", strerror(errno));
  }

  status = STATUS_OK;
  return status;
}

static Status write_int(NeuralNetIoWriter* writer, unsigned long data) {
  Status status;

  fwrite(&data, sizeof(data), 1, writer->out_file);
  if (ferror(writer->out_file)) {
     printf("NeuralNetIo.write_int: %s\n", strerror(errno));
  }

  status = STATUS_OK;
  return status;
}

static Status write_float(NeuralNetIoWriter* writer, double data) {
  Status status;

  fwrite(&data, sizeof(data), 1, writer->out_file);
  if (ferror(writer->out_file)) {
     printf("NeuralNetIo.write_float: %s\n", strerror(errno));
  }

  status = STATUS_OK;
  return status;
}

static Status write_double(NeuralNetIoWriter* writer, double data) {
  Status status;

  fwrite(&data, sizeof(data), 1, writer->out_file);
  if (ferror(writer->out_file)) {
     printf("NeuralNetIo.write_double: %s\n", strerror(errno));
  }

  status = STATUS_OK;
  return status;
}

static void deinit(NeuralNetIoWriter* writer, unsigned long epochs) {
  writer->close_file(writer, epochs);
}

static Status open_file(NeuralNetIoWriter* writer) {
  Status status;

  writer->out_file = fopen(writer->out_path, "w");
  if (writer->out_file == NULL) {
    printf("NeuralNetIo::start_epoch: could not open file: '%s' err=%s\n",
        writer->out_path, strerror(errno));
    status = STATUS_ERR;
    goto done;
  }

  status = STATUS_OK;

done:
  return status;
}

static Status close_file(NeuralNetIoWriter* writer, unsigned long epochs) {
  Status status;

  if (writer->out_file != NULL) {
    fseek(writer->out_file, 0, 0);
    writer->write_int(writer, epochs);

    fclose(writer->out_file);
    writer->out_file = NULL;
  }

  status = STATUS_OK;

  return status;
}


static Status begin_epoch(NeuralNetIoWriter* writer, size_t epoch) {
  Status status;

  unused(writer);
  unused(epoch);

  status = STATUS_OK;

  return status;
}

static Status write_point_val(NeuralNetIoWriter* writer, double* point) {
  Status status = STATUS_OK;

  for (unsigned long b = 0; b < 4; b++) {
    status = writer->write_float(writer, point[b]);
    if (StatusErr(status)) {
      return status;
    }
  }
  return status;
}

static Status write_epoch(NeuralNetIoWriter* writer) {
  Status status;

  NeuralNet* nn = writer->nn;
  double xaxis;
  double yaxis;
  //double zaxis;

  double xaxis_max = 1.0;
  double yaxis_max = 1.0;
  //double zaxis_max = 1.0;

  double xaxis_count = nn->out_layer + 1.0;
  xaxis_count += 1.0; // +1.0 for the output layer's output
  double xaxis_offset = xaxis_max / (xaxis_count + 1.0);

  double yaxis_count;
  double yaxis_offset;

  // Write bounding box
  double bounding_box[8] = {
    0.0, 0.0, -12.0, -12.0,
    1.0, 1.0, +12.0, +12.0
  };
  status = writer->write_point_val(writer, &bounding_box[0]);
  if (StatusErr(status)) {
    printf("NeuralNetIoWriter_init: unable to write &bounding_box[0]\n");
    goto done;
  }
  status = writer->write_point_val(writer, &bounding_box[4]);
  if (StatusErr(status)) {
    printf("NeuralNetIoWriter_init: unable to write &bounding_box[4]\n");
    goto done;
  }

  // Write the input neuron's output values
  yaxis_count = nn->layers[0].count;
  yaxis_offset = yaxis_max / (yaxis_count + 1.0);
  yaxis = yaxis_offset;

  xaxis = xaxis_offset;
  for (unsigned long n = 0; n < yaxis_count; n++) {
    Neuron* neuron = &nn->layers[0].neurons[n];
    double point[4] = { xaxis, yaxis, neuron->output, neuron->output };
    status = writer->write_point_val(writer, point);
    if (StatusErr(status)) {
      printf("NeuralNetIoWriter_init: unable to write input layer\n");
      goto done;
    }
    yaxis += yaxis_offset;
  }

  // Write the hidden and output layers weights
  xaxis += xaxis_offset;
  for (unsigned long l = 1; l <= nn->out_layer; l++) {
    NeuronLayer* layer = &nn->layers[l];

    // Count the total connectons for the layer
    unsigned long layer_count = 0;
    for (unsigned long n = 0; n < layer->count; n++) {
      // The additional value is for the bias
      layer_count += (layer->neurons[0].inputs->count + 1);
    }
    yaxis_offset = yaxis_max / (layer_count + 1.0);

    yaxis = yaxis_offset;
    for (unsigned long n = 0; n < layer->count; n++) {
      // Get the next neuron
      Neuron* neuron = &layer->neurons[n];

      // Point at the first of the neuron's inputs and weights arrays
      double* weights = neuron->weights;

      // Loop thought all of the neuron's output the weights
      // which includes the bias, hence the <= test.
      for (unsigned long i = 0; i <= neuron->inputs->count; i++) {
        double point[4] = { xaxis, yaxis, weights[i], weights[i] };
        status = writer->write_point_val(writer, point);
        if (StatusErr(status)) {
          printf("NeuralNetIoWriter_init: unable to write weights\n");
          goto done;
        }
        yaxis += yaxis_offset;
      }
    }
    xaxis += xaxis_offset;
  }

  // Write the onput neuron's output values
  NeuronLayer* layer = &nn->layers[nn->out_layer];
  yaxis_count = layer->count;
  yaxis_offset = yaxis_max / (yaxis_count + 1.0);
  yaxis = yaxis_offset;

  for (unsigned long n = 0; n < yaxis_count; n++) {
    Neuron* neuron = &layer->neurons[n];
    double point[4] = { xaxis, yaxis, neuron->output, neuron->output };
    status = writer->write_point_val(writer, point);
    if (StatusErr(status)) {
      printf("NeuralNetIoWriter_init: unable to write weights\n");
      goto done;
    }
    yaxis += yaxis_offset;
  }

  status = STATUS_OK;

done:
  return status;
}

static Status end_epoch(NeuralNetIoWriter* writer) {
  Status status;

  unused(writer);

  status = STATUS_OK;

  return status;
}

Status NeuralNetIoWriter_init(NeuralNetIoWriter* writer, NeuralNet* nn,
    unsigned long points_per_epoch, char* out_path) {
  Status status;

  // Initialize
  writer->out_file = NULL;
  writer->out_path = out_path;
  writer->nn = nn;
  writer->deinit = deinit;
  writer->open_file = open_file;
  writer->close_file = close_file;
  writer->begin_epoch = begin_epoch;
  writer->write_epoch = write_epoch;
  writer->end_epoch = end_epoch;
  writer->write_str = write_str;
  writer->write_int = write_int;
  writer->write_float = write_float;
  writer->write_double = write_double;
  writer->write_point_val = write_point_val;

  if (writer->nn == NULL) {
    printf("NeuralNetIoWriter_init: nn is NULL\n");
    status = STATUS_BAD_PARAM;
  }

  status = writer->open_file(writer);
  if (StatusErr(status)) {
    goto done;
  }

  // Write Number of epochs as 0 right now
  status = writer->write_int(writer, 0);
  if (StatusErr(status)) {
    printf("NeuralNetIoWriter_init: unable to write epochs\n");
    goto done;
  }

  // Write Points per Epoch
  status = writer->write_int(writer, points_per_epoch);
  if (StatusErr(status)) {
    printf("NeuralNetIoWriter_init: unable to write points_per_epoch\n");
    goto done;
  }

  // Write header
  char* header = "x y z value\n";
  status = writer->write_str(writer, header);
  if (StatusErr(status)) {
    printf("NeuralNetIoWriter_init: unable to write header\n");
    goto done;
  }

  status = STATUS_OK;

done:
  if (StatusErr(status)) {
    writer->deinit(writer, 0);
  }

  return status;
}
