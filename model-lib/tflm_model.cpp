/* Copyright 2023 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#include <math.h>

#include "tensorflow/lite/core/c/common.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_log.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/micro_profiler.h"
#include "tensorflow/lite/micro/recording_micro_interpreter.h"
#include "tensorflow/lite/micro/system_setup.h"
#include "tensorflow/lite/schema/schema_generated.h"

// You may want to put large static data in a special region depending on your RAM/flash setup.
// See `rust-firmware/firmware/memory.x` for an example linker script.
extern uint8_t __attribute__ ((section(".model_data"))) example_quant_tflite[];

#include "model_data.h"

namespace {
using MyOpResolver = tflite::MicroMutableOpResolver<18>;

TfLiteStatus RegisterOps(MyOpResolver& op_resolver) {
  TF_LITE_ENSURE_STATUS(op_resolver.AddFullyConnected());
  TF_LITE_ENSURE_STATUS(op_resolver.AddReshape());
  TF_LITE_ENSURE_STATUS(op_resolver.AddMaxPool2D());
  TF_LITE_ENSURE_STATUS(op_resolver.AddSlice());
  TF_LITE_ENSURE_STATUS(op_resolver.AddLogistic());
  TF_LITE_ENSURE_STATUS(op_resolver.AddAdd());
  TF_LITE_ENSURE_STATUS(op_resolver.AddMul());
  TF_LITE_ENSURE_STATUS(op_resolver.AddTanh());
  TF_LITE_ENSURE_STATUS(op_resolver.AddConcatenation());
  TF_LITE_ENSURE_STATUS(op_resolver.AddTranspose());
  TF_LITE_ENSURE_STATUS(op_resolver.AddUnpack());
  TF_LITE_ENSURE_STATUS(op_resolver.AddConv2D());
  TF_LITE_ENSURE_STATUS(op_resolver.AddSplit());
  TF_LITE_ENSURE_STATUS(op_resolver.AddStridedSlice());
  TF_LITE_ENSURE_STATUS(op_resolver.AddPack());
  TF_LITE_ENSURE_STATUS(op_resolver.AddShape());
  TF_LITE_ENSURE_STATUS(op_resolver.AddExpandDims());
  TF_LITE_ENSURE_STATUS(op_resolver.AddSoftmax());
  return kTfLiteOk;
}
}  // namespace

TfLiteStatus ProfileMemoryAndLatency() {
  tflite::MicroProfiler profiler;
  MyOpResolver op_resolver;
  TF_LITE_ENSURE_STATUS(RegisterOps(op_resolver));

  constexpr int kTensorArenaSize = 20000;
  uint8_t tensor_arena[kTensorArenaSize];
  constexpr int kNumResourceVariables = 24;

  tflite::RecordingMicroAllocator* allocator(
      tflite::RecordingMicroAllocator::Create(tensor_arena, kTensorArenaSize));
  tflite::RecordingMicroInterpreter interpreter(
      tflite::GetModel(example_quant_tflite), op_resolver, allocator,
      tflite::MicroResourceVariables::Create(allocator, kNumResourceVariables),
      &profiler);

  TF_LITE_ENSURE_STATUS(interpreter.AllocateTensors());
  TFLITE_CHECK_EQ(interpreter.inputs_size(), 2);
  // interpreter.input(0)->data.f[0] = 1.f;
  TF_LITE_ENSURE_STATUS(interpreter.Invoke());

  profiler.LogTicksPerTagCsv();

  interpreter.GetMicroAllocator().PrintAllocations();
  return kTfLiteOk;
}

TfLiteStatus RunModel(int iters) {
  // Map the model into a usable data structure. This doesn't involve any
  // copying or parsing, it's a very lightweight operation.
  const tflite::Model* model =
      ::tflite::GetModel(example_quant_tflite);
  TFLITE_CHECK_EQ(model->version(), TFLITE_SCHEMA_VERSION);

  MyOpResolver op_resolver;
  TF_LITE_ENSURE_STATUS(RegisterOps(op_resolver));

  // Arena size just a round number. The exact arena usage can be determined
  // using the RecordingMicroInterpreter.
  constexpr int kTensorArenaSize = 40000;
  uint8_t tensor_arena[kTensorArenaSize];

  tflite::MicroInterpreter interpreter(model, op_resolver, tensor_arena,
                                       kTensorArenaSize);

  if (interpreter.AllocateTensors() != kTfLiteOk) {
    MicroPrintf("Failed to allocate tensors, arena is probably too small");
    return kTfLiteError;
  }

  TfLiteTensor* input = interpreter.input(0);
  TFLITE_CHECK_NE(input, nullptr);

  TFLITE_CHECK_EQ(input->dims->size, 3);
  TFLITE_CHECK_EQ(input->dims->data[0], 1);
  TFLITE_CHECK_EQ(input->dims->data[1], 1);

  for (size_t i = 0; i < input->dims->data[2]; i++) {
    input->data.int8[i] = 3;
  }

  TfLiteTensor* hidden = interpreter.input(1);
  TFLITE_CHECK_NE(hidden, nullptr);

  for (size_t i = 0; i < hidden->dims->data[1]; i++) {
    hidden->data.int8[i] = 1;
  }

  if (iters == 1) {
    TF_LITE_ENSURE_STATUS(interpreter.Invoke());
    TfLiteTensor* output = interpreter.output(0);
    TFLITE_CHECK_NE(output, nullptr);

    MicroPrintf("output: %d, %d, %d, %d, %d, %d",
                output->data.int8[0],
                output->data.int8[1],
                output->data.int8[2],
                output->data.int8[3],
                output->data.int8[5],
                output->data.int8[6]
                );
  } else {
    for (int i = 0; i < iters; i++) {
      TF_LITE_ENSURE_STATUS(interpreter.Invoke());
    }
  }

  return kTfLiteOk;
}

extern "C" {

void SayHello() {
  MicroPrintf("Hello from C++. 1+1=%d", 1+1);
}

int RunModelFromRust(int iters) {
  return RunModel(iters);
}

int ProfileModelFromRust() {
  return ProfileMemoryAndLatency();
}

}


