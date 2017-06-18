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
#include "dbg.h"
#include "rand0_1.h"
#include "e_exp.h"
#include "malloc.h"
#include "calloc.h"
#include "unused.h"
#include "types.h"

// Forward declarations
static void NeuralNet_deinit(NeuralNet* nn);
static Status NeuralNet_start(NeuralNet* nn);
static void NeuralNet_stop(NeuralNet* nn);
static Status NeuralNet_add_hidden(NeuralNet* nn, u64 count);
static u64 NeuralNet_get_points(NeuralNet* nn);
static void NeuralNet_set_inputs(NeuralNet* nn, Pattern* input);
static void NeuralNet_get_outputs(NeuralNet* nn, Pattern* output);
static f64 NeuralNet_adjust_weights(NeuralNet* nn, Pattern* output,
    Pattern* target);
static void NeuralNet_process(NeuralNet* nn);

static Status NeuralNet_create_layer(NeuronLayer* l, u64 count) {
  Status status;

  dbg("NeuralNet_create_layer:+%p count=%" PRIu64 "\n", (void*)l, count);

  l->neurons = calloc((size_t)count, sizeof(Neuron));
  if (l->neurons == NULL) {
    status = STATUS_OOM;
    goto done;
  }
  l->count = count;
  status = STATUS_OK;

done:
  dbg("NeuralNet_create_layer:-%p status=%d\n", (void*)l, StatusVal(status));
  return status;
}

static Status Neuron_init(Neuron* n, NeuronLayer* inputs) {
  Status status;
  f64* weights;
  f64* momentums;
  u64 points;
  dbg("Neuron_init:+%p inputs=%p\n", (void*)n, (void*)inputs);

  if (inputs == NULL) {
    weights = NULL;
    momentums = NULL;
    points = 1;
  } else {
    // Calculate the initial weights. Note weights[0] is the bias
    // so we increase count of weights by one.
    u64 count = inputs->count + 1;
    weights = calloc((size_t)count, sizeof(f64));
    if (weights == NULL) { status = STATUS_OOM; goto done; }

    // Initialize weights >= -0.5 and < 0.5
    dbg("Neuron_init:+%p inputs=%p count=%" PRIu64 "\n", (void*)n, (void*)inputs, count);
    for (u64 w = 0; w < count; w++) {
      weights[w] = rand0_1() - 0.5;
      dbg("Neuron_init: %p weights[%" PRIu64 "]=%lf\n", (void*)n, w, weights[w]);
    }

    // Allocate an array of mementums initialize to 0.0
    momentums = calloc((size_t)count, sizeof(f64));
    if (momentums == NULL) { status = STATUS_OOM; goto done; }

    points = count; // + 1 // for output
  }

  n->inputs = inputs;
  n->weights = weights;
  n->momentums = momentums;
  n->points = points;
  n->output = 0.0;
  n->pd_error = 0.0;
  status = STATUS_OK;

done:
  dbg("Neuron_init:-%p status=%d\n", (void*)n, status);

  return status;
}

Status NeuralNet_init(NeuralNet* nn, u64 num_in_neurons, u64 num_hidden_layers,
    u64 num_out_neurons) {
  Status status;
  dbg("NeuralNet_init:+%p in_neurons=%" PRIu64 " hidden_layers=%" PRIu64 "" "out_neurons=%" PRIu64 "\n",
      (void*)nn, num_in_neurons, num_hidden_layers, num_out_neurons);

  nn->max_layers = 2; // We always have an input and output layer
  nn->max_layers += num_hidden_layers; // Add num_hidden layers
  nn->out_layer = nn->max_layers - 1;  // last one is out_layer
  nn->last_hidden = 0; // No hidden layers yet
  nn->points = 0; // No points yet
  nn->error = 0;       // No errors yet
  nn->learning_rate = 0.5; // Learning rate aka eta
  nn->momentum_factor = 0.9; // momemtum factor aka alpha
  nn->layers = NULL;   // No layers yet

  // Create the layers
  nn->layers = calloc((size_t)nn->max_layers, sizeof(NeuronLayer));
  if (nn->layers == NULL) { status = STATUS_OOM; goto done; }

  // Initalize input and output layers
  status = NeuralNet_create_layer(&nn->layers[0], num_in_neurons);
  if (StatusErr(status)) goto done;
  status = NeuralNet_create_layer(&nn->layers[nn->out_layer], num_out_neurons);
  if (StatusErr(status)) goto done;

  nn->deinit = NeuralNet_deinit;
  nn->start = NeuralNet_start;
  nn->stop = NeuralNet_stop;
  nn->add_hidden = NeuralNet_add_hidden;
  nn->get_points = NeuralNet_get_points;
  nn->set_inputs = NeuralNet_set_inputs;
  nn->get_outputs = NeuralNet_get_outputs;
  nn->adjust_weights = NeuralNet_adjust_weights;
  nn->process = NeuralNet_process;

  status = STATUS_OK;

done:
  if (StatusErr(status)) {
    NeuralNet_deinit(nn);
  }
  dbg("NeuralNet_init:-%p status=%d\n", (void*)nn, StatusVal(status));
  return status;
}

void NeuralNet_deinit(NeuralNet* nn) {
  dbg("NeuralNet_deinit:+%p\n", (void*)nn);

  if (nn->layers != NULL) {
    for (u64 i = 0; i < nn->max_layers; i++) {
      if (nn->layers[i].neurons != NULL) {
        free(nn->layers[i].neurons);
      }
      nn->layers[i].neurons = NULL;
    }
    free(nn->layers);
    nn->max_layers = 0;
    nn->last_hidden = 0;
    nn->out_layer = 0;
    nn->points = 0;
    nn->layers = NULL;
  }

  dbg("NeuralNet_deinit:-%p\n", (void*)nn);
}

static Status NeuralNet_add_hidden(NeuralNet* nn, u64 count) {
  Status status;

  dbg("NeuralNet_add_hidden:+%p count=%" PRIu64 "\n", (void*)nn, count);

  nn->last_hidden += 1;
  if (nn->last_hidden >= (nn->max_layers - 1)) {
    status = STATUS_TO_MANY_HIDDEN;
    goto done;
  }
  status = NeuralNet_create_layer(&nn->layers[nn->last_hidden], count);
  if (StatusErr(status)) goto done;
  status = STATUS_OK;

done:
  dbg("NeuralNet_add_hidden:-%p status=%d\n", (void*)nn, StatusVal(status));
  return status;
}

static Status NeuralNet_start(NeuralNet* nn) {
  Status status;
  dbg("NeurnaNet_start:+%p\n", (void*)nn);

  // Check if the user added all of the hidden layers they could
  if ((nn->last_hidden + 1) < (nn->max_layers - 1)) {
    // Nope, there were fewer hidden layers than there could be
    // so move the output layer to be after the last hidden layer
    nn->out_layer = nn->last_hidden + 1;
    nn->layers[nn->out_layer].count = nn->layers[nn->max_layers - 1].count;
    nn->layers[nn->out_layer].neurons = nn->layers[nn->max_layers - 1].neurons;
    nn->layers[nn->max_layers - 1].count = 0;
    nn->layers[nn->max_layers - 1].neurons = NULL;
  }

  dbg("NeuralNet_start: %p max_layers=%" PRIu64 " last_hidden=%" PRIu64 " out_layer=%" PRIu64 "\n",
      (void*)nn, nn->max_layers, nn->last_hidden, nn->out_layer);

  // Initialize the neurons for all of the layers
  nn->points = 0;
  for (u64 l = 0; l < nn->max_layers; l++) {
    NeuronLayer* in_layer;
    if (l == 0) {
      // Layer 0 is the input layer so it has no inputs
      in_layer = NULL;
    } else {
      in_layer = &nn->layers[l-1];
    }
    dbg("NeuralNet_start: nn->layers[%" PRIu64 "].count=%" PRIu64 " in_layer=%p\n", l,
        nn->layers[l].count, (void*)in_layer);
    for (u64 n = 0; n < nn->layers[l].count; n++) {
      Neuron_init(&nn->layers[l].neurons[n], in_layer);
      nn->points += nn->layers[l].neurons[n].points;
    }
  }
  // Add the number of outputs since we're currently
  // not displaying outputs of hidden layers
  nn->points += nn->layers[nn->out_layer].count;

  // Add two more for the bounding box
  nn->points += 2;

  status = STATUS_OK;

  dbg("NeuralNet_start:-%p status=%d\n", (void*)nn, StatusVal(status));
  return status;
}

static void NeuralNet_stop(NeuralNet* nn) {
  unused(nn);
  dbg("NeuralNet_stop:+%p\n", (void*)nn);
  dbg("NeuralNet_stop:-%p\n", (void*)nn);
}

static u64 NeuralNet_get_points(NeuralNet* nn) {
  dbg("NeuralNet_get_points:+-%p count=%" PRIu64 "\n", (void*)nn, nn->points);
  return nn->points;
}

static void NeuralNet_set_inputs(NeuralNet* nn, Pattern* input) {
  dbg("NeuralNet_set_inputs_:+%p count=%" PRIu64 " input_layer count=%" PRIu64 "\n",
      (void*)nn, input->count, nn->layers[0].count);
  for (u64 n = 0; n < nn->layers[0].count; n++) {
    // Set then input neuron output
    Neuron* neuron = &nn->layers[0].neurons[n];
    neuron->output = input->data[n];
    dbg("NeuralNet_set_inputs_: %p neuron=%p output=%lf\n",
        (void*)nn, (void*)neuron, neuron->output);
  }
  dbg("NeuralNet_set_inputs_:-%p\n", (void*)nn);
}

static void NeuralNet_process(NeuralNet* nn) {
  dbg("NeuralNet_process_:+%p\n", (void*)nn);
  // Calcuate the output for the fully connected layers,
  // which start at nn->layers[1]
  for (u64 l = 1; l <= nn->out_layer; l++) {
    NeuronLayer* layer = &nn->layers[l];
    for (u64 n = 0; n < layer->count; n++) {
      // Get the next neuron
      Neuron* neuron = &layer->neurons[n];

      // Point at the first of the neuron's inputs and weights arrays
      Neuron* inputs = neuron->inputs->neurons;
      f64* weights = neuron->weights;

      // Initialize the weighted_sum to the first weight, this is the bias
      f64 weighted_sum = *weights;

      // Skip past bias
      weights += 1;

      // Loop though all of the neuron's inputs summing inputs scaled
      // by the weight
      for (u64 i = 0; i < neuron->inputs->count; i++) {
        weighted_sum += weights[i] * inputs[i].output;
      }

      // Calcuate the output using a Sigmoidal Activation function
      neuron->output = 1.0 / (1.0 + e_exp(-weighted_sum));
      dbg("NeuralNet_process_: %p output=%lf weighted_sum=%lf\n",
          (void*)neuron, neuron->output, weighted_sum);
    }
  }
  dbg("NeuralNet_process_:-%p\n", (void*)nn);
}

static void NeuralNet_get_outputs(NeuralNet* nn, Pattern* output) {
  u64 count;

  dbg("NeuralNet_outputs_:+%p count=%" PRIu64 "\n", (void*)nn, output->count);
  if (output->count > nn->layers[nn->out_layer].count) {
    count = nn->layers[nn->out_layer].count;
  } else {
    count = output->count;
  }
  for (u64 i = 0; i < count; i++) {
    output->data[i] = nn->layers[nn->out_layer].neurons[i].output;
    dbg("NeuralNet_outputs_: %p output[%" PRIu64 "]=%lf\n", (void*)nn, i, output->data[i]);
  }
  dbg("NeuralNet_outputs_:-%p\n", (void*)nn);
}

static f64 NeuralNet_adjust_weights(NeuralNet* nn, Pattern* output,
    Pattern* target) {
  dbg("NeuralNet_adjust_weights_:+%p output count=%" PRIu64 " target count=%" PRIu64 "\n",
      (void*)nn, output->count, target->count);

  // Calculate the network error and partial derivative of the error
  // for the output layer
  dbg("\nNeuralNet_adjust_weights_: %p calculate pd_error and total_error\n", (void*)nn);
  nn->error = 0.0;
  if (output->count != target->count) {
      return (f64)NAN;
  }
  for (u64 n = 0; n < output->count; n++) {
    // Compute the error as the difference between target and output
    f64 err = target->data[n] - output->data[n];
    dbg("NeuralNet_adjust_weights_: %" PRIu64 ":%" PRIu64 " err:%lf = target:%lf + output:%lf\n",
            nn->out_layer, n, err, target->data[n], output->data[n]);

    // Compute the partial derivative of the activation w.r.t. error
    f64 pd_err = err * output->data[n] * (1.0 - output->data[n]);
    nn->layers[nn->out_layer].neurons[n].pd_error = pd_err;
    dbg("NeuralNet_adjust_weights_: %" PRIu64 ":%" PRIu64 " pd_err:%lf ="
        " err:%lf * output[%" PRIu64 "]:%lf * (1.0 - output[%" PRIu64 "]:%lf\n",
        nn->out_layer, n, pd_err, err, n, output->data[n], n, output->data[n]);

    // Compute the sub of the square of the error and add to total_error
    f64 sse = 0.5 * err * err;
    dbg("NeuralNet_adjust_weights_: %" PRIu64 ":%" PRIu64 " sse:%lf = 0.5 * err:%lf * err:%lf\n",
            nn->out_layer, n, sse, err, err);

    f64 tmp = nn->error;
    nn->error = tmp + sse;
    dbg("NeuralNet_adjust_weights_: %" PRIu64 ":%" PRIu64 " nn->error:%lf = nn->error:%lf + sse:%lf\n",
        nn->out_layer, n, nn->error, tmp, sse);
  }
  dbg("NeuralNet_adjust_weights_: out_layer:%" PRIu64 " this.error=%lf\n",
          nn->out_layer, nn->error);

  // For all of layers starting at the output layer back propagate the pd_error
  // to the previous layers. The output layers pd_error has been calculated above
  dbg("\nNeuralNet_adjust_weights_: %p backpropagate pd_error to hidden layers\n", (void*)nn);
  u64 first_hidden_layer = 1;
  for (u64 l = nn->out_layer; l > first_hidden_layer; l--) {
    NeuronLayer* cur_layer = &nn->layers[l];
    NeuronLayer* prev_layer = &nn->layers[l-1];
    dbg("NeuralNet_adjust_weights_: %p cur_layer=%" PRIu64 " prev_layer=%" PRIu64 "\n", (void*)nn, l, l-1);

    // Compute the partial derivative of the error for the previous layer
    for (u64 npl = 0; npl < prev_layer->count; npl++) {
      f64 sum_weighted_pd_err = 0.0;
      for (u64 ncl = 0; ncl < cur_layer->count; ncl++) {
        f64 pd_err = cur_layer->neurons[ncl].pd_error;
        dbg("NeuralNet_adjust_weights_: %p cur_layer:%" PRIu64 ":%" PRIu64 " pd_err=%lf\n",
            (void*)nn, l, ncl, pd_err);
        f64 weight = cur_layer->neurons[ncl].weights[npl+1];
        dbg("NeuralNet_adjust_weights_: %p cur_layer:%" PRIu64 ":%" PRIu64 " weights[%" PRIu64 "]=%lf\n",
            (void*)nn, l, ncl, npl+1, weight);
        sum_weighted_pd_err += pd_err * weight;
        dbg("NeuralNet_adjust_weights_: %p cur_layer:%" PRIu64 ":%" PRIu64 " sum_weighted_pd_err:%lf\n",
            (void*)nn, l, ncl, sum_weighted_pd_err);
      }

      f64 prev_out = prev_layer->neurons[npl].output;
      f64 pd_prev_out = prev_out * (1.0 - prev_out);
      dbg("NeuralNet_adjust_weights_: %p prev_layer:%" PRIu64 ":%" PRIu64 " pd_prev_out:%lf = "
          "prev_out:%lf * (1.0 - prev_out:%lf)\n",
        (void*)nn, l-1, npl, pd_prev_out, prev_out, prev_out);
      prev_layer->neurons[npl].pd_error = sum_weighted_pd_err * pd_prev_out;
      dbg("NeuralNet_adjust_weights_: %p prev_layer:%" PRIu64 ":%" PRIu64 " pd_error:%lf ="
          " sum_weighted_pd_err:%lf * pd_prev_out:%lf\n",
          (void*)nn, l-1, npl, prev_layer->neurons[npl].pd_error,
          sum_weighted_pd_err ,pd_prev_out);
    }
  }

  // Update the weights for hidden layers and output layer
  dbg("\nNeuralNet_adjust_weights_: %p update weights learning_rate=%lf"
      " momemutum_factor=%lf\n", (void*)nn, nn->learning_rate, nn->momentum_factor);
  for (u64 l = 1; l <= nn->out_layer; l++) {
    NeuronLayer* layer = &nn->layers[l];
    dbg("NeuralNet_adjust_weights_: %p loop through layer %" PRIu64 "\n", (void*)nn, l);
    for (u64 n = 0; n < layer->count; n++) {
      Neuron* neuron = &layer->neurons[n];
      NeuronLayer* inputs = neuron->inputs;

      // Point weights and mementums input entires at index 1,
      // the bias entries will be at index -1
      f64* weights = &neuron->weights[1];
      f64* momentums = &neuron->momentums[1];

      // Start with bias
      f64 pd_err = neuron->pd_error; 

      // Update the weights for bias
      f64 momentum = nn->momentum_factor * momentums[-1];
      dbg("momentum:%lf = nn->momentum_factor:%lf momentums[%d]:%lf\n",
          momentum, nn->momentum_factor, -1, momentums[-1]);
      momentums[-1] = (nn->learning_rate * pd_err) + momentum;
      dbg("NeuralNet_adjust_weights_: %p %" PRIu64 ":%" PRIu64 " momentums[%d]:%lf ="
          " (eta:%lf * pd_err:%lf) + momentum:%lf bias\n",
          (void*)nn, l, n, -1, momentums[-1], nn->learning_rate, pd_err, momentum);

      f64 w = weights[-1];
      weights[-1] = weights[-1] + momentums[-1];
      dbg("NeuralNet_adjust_weights_: %p %" PRIu64 ":%" PRIu64 " weights[%d]:%lf"
          " = weights[%d]:%lf momentums[%d]=%lf bias\n",
          (void*)nn, l, n, -1, weights[-1], -1, w, -1, momentums[-1]);


      // Loop through this neurons input neurons adjusting the weights and momentums
      dbg("NeuralNet_adjust_weights_: %p loop through neurons for %" PRIu64 ":%" PRIu64 ""
         " update weights pd_err=%lf\n", (void*)nn, l, n, pd_err);
      for (u64 i = 0; i < neuron->inputs->count; i++) {
        // Update the weights
        f64 input = inputs->neurons[i].output;
        momentum = nn->momentum_factor * momentums[i];
        momentums[i]  = (nn->learning_rate * input * pd_err) + momentum;
        dbg("NeuralNet_adjust_weights_: %p %" PRIu64 ":%" PRIu64 " momentums[%" PRIu64 "]:%lf ="
            " (eta:%lf * input:%lf pd_err:%lf) + momentum:%lf\n",
            (void*)nn, l, n, i, momentums[i], nn->learning_rate, input, pd_err, momentum);

        w = weights[i];
        weights[i] = weights[i] + momentums[i];
        dbg("NeuralNet_adjust_weights_: %p %" PRIu64 ":%" PRIu64 " weights[%" PRIu64 "]:%lf ="
            " weights[%" PRIu64 "]:%lf + momentums[%" PRIu64 "]=%lf\n",
            (void*)nn, l, n, i, weights[i], i, w, i, momentums[i]);
      }
    }
  }

  dbg("NeuralNet_adjust_weights_:-%p nn->error=%lf\n", (void*)nn, nn->error);
  return nn->error;
}
