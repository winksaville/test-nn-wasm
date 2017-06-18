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

#ifndef NEURAL_NET_H
#define NEURAL_NET_H

#include "types.h"

// Forward declarations
typedef struct Pattern Pattern;
typedef struct Neuron Neuron;
typedef struct NeuronLayer NeuronLayer;
typedef struct NeuralNet NeuralNet;

// NeuralNet methods
typedef void (*NeuralNet_Deinit)(NeuralNet* nn);

typedef Status (*NeuralNet_Start)(NeuralNet* nn);

typedef void (*NeuralNet_Stop)(NeuralNet* nn);

typedef Status (*NeuralNet_AddHidden)(NeuralNet* nn, u64 count);

typedef u64 (*NeuralNet_GetPoints)(NeuralNet* nn);

typedef void (*NeuralNet_SetInputs)(NeuralNet* nn, Pattern* input);

typedef void (*NeuralNet_GetOutputs)(NeuralNet* nn, Pattern* output);

typedef f64 (*NeuralNet_AdjustWeights)(NeuralNet* nn, Pattern* output, Pattern* target);

typedef void (*NeuralNet_Process)(NeuralNet* nn);


#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpadded"
typedef struct Pattern {
  u64 count;
  f64 data[];
} Pattern;

typedef struct Neuron {
  NeuronLayer* inputs;  // Neuron layer of inputs
  f64* weights;      // Array of weights for each input plus the bias
  f64* momentums;    // Array of momentums for each input plus the bias
  f64 output;        // The output of this neuron
  f64 pd_error;      // Partial derative of this neurons error
  u64 points;        // Points is number of graphic points
} Neuron;

typedef struct NeuronLayer {
  u64 count;          // Number of neurons
  Neuron* neurons;    // The neurons
} NeuronLayer;

typedef struct NeuralNet {
  u64 max_layers; // Maximum layers in the nn
                            // layers[0] input layer
                            // layers[1] first hidden layer
  u64 out_layer;  // layers[out_layer] is output layer
  u64 last_hidden;// layers[last_hidden] is last hidden layer
  f64 error;             // The overall network error
  f64 learning_rate;     // Learning rate aka 'eta'
  f64 momentum_factor;   // Momentum factor aka 'aplha'
  u64 points;     // Points is number

  Pattern* input;           // Input pattern

  // There will always be at least two layers,
  // plus there are zero or more hidden layers.
  NeuronLayer* layers;

  // Methods
  NeuralNet_Deinit deinit;
  NeuralNet_Start start;
  NeuralNet_Stop stop;
  NeuralNet_AddHidden add_hidden;
  NeuralNet_GetPoints get_points;
  NeuralNet_SetInputs set_inputs;
  NeuralNet_GetOutputs get_outputs;
  NeuralNet_AdjustWeights adjust_weights;
  NeuralNet_Process process;

} NeuralNet;
#pragma clang diagnostic pop

Status NeuralNet_init(NeuralNet* nn, u64 num_in, u64 num_hidden, u64 num_out);


#endif
