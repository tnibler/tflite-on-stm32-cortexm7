cmake_minimum_required(VERSION 3.14)

set(CMAKE_TOOLCHAIN_FILE arm-none-eabi-gcc.cmake)

project(tflm-example)
set(THIRD_PARTY_DIR ../tflite-micro/tensorflow/lite/micro/tools/make/downloads)

add_library(tflm-example STATIC)


add_subdirectory(third_party/CMSIS-NN)

target_include_directories(tflm-example SYSTEM PRIVATE
		${THIRD_PARTY_DIR}/flatbuffers/include
		${THIRD_PARTY_DIR}/gemmlowp
		${THIRD_PARTY_DIR}/ruy
		${THIRD_PARTY_DIR}/kissfft
		third_party/CMSIS-NN
		third_party/CMSIS-NN/Include
)

target_include_directories(tflm-example PRIVATE 
		../tflite-micro/
)

target_link_libraries(tflm-example PRIVATE cmsis-nn)

set(TFLITE_SOURCES
	# Implementations for these will be provided in Rust in this case, but you can add them here
	# tflite-micro/tensorflow/lite/micro/cortex_m_generic/debug_log.cc
	# ../tflite-micro/tensorflow/lite/micro/cortex_m_generic/micro_time.cc

	../tflite-micro/tensorflow/lite/micro/kernels/cmsis_nn/add.cc
	../tflite-micro/tensorflow/lite/micro/kernels/cmsis_nn/batch_matmul.cc
	../tflite-micro/tensorflow/lite/micro/kernels/cmsis_nn/conv.cc
	../tflite-micro/tensorflow/lite/micro/kernels/cmsis_nn/depthwise_conv.cc
	../tflite-micro/tensorflow/lite/micro/kernels/cmsis_nn/fully_connected.cc
	../tflite-micro/tensorflow/lite/micro/kernels/cmsis_nn/maximum_minimum.cc
	../tflite-micro/tensorflow/lite/micro/kernels/cmsis_nn/mul.cc
	../tflite-micro/tensorflow/lite/micro/kernels/cmsis_nn/pad.cc
	../tflite-micro/tensorflow/lite/micro/kernels/cmsis_nn/pooling.cc
	../tflite-micro/tensorflow/lite/micro/kernels/cmsis_nn/softmax.cc
	../tflite-micro/tensorflow/lite/micro/kernels/cmsis_nn/svdf.cc
	../tflite-micro/tensorflow/lite/micro/kernels/cmsis_nn/transpose.cc
	../tflite-micro/tensorflow/lite/micro/kernels/cmsis_nn/transpose_conv.cc
	../tflite-micro/tensorflow/lite/micro/kernels/cmsis_nn/unidirectional_sequence_lstm.cc

	../tflite-micro/tensorflow/compiler/mlir/lite/core/api/error_reporter.cc
	../tflite-micro/tensorflow/compiler/mlir/lite/schema/schema_utils.cc
	../tflite-micro/tensorflow/compiler/mlir/lite/core/api/error_reporter.cc
	../tflite-micro/tensorflow/compiler/mlir/lite/schema/schema_utils.cc
	../tflite-micro/tensorflow/lite/core/api/flatbuffer_conversions.cc
	../tflite-micro/tensorflow/lite/core/api/tensor_utils.cc
	../tflite-micro/tensorflow/lite/core/c/common.cc
	../tflite-micro/tensorflow/lite/kernels/internal/common.cc
	../tflite-micro/tensorflow/lite/kernels/internal/portable_tensor_utils.cc
	../tflite-micro/tensorflow/lite/kernels/internal/quantization_util.cc
	../tflite-micro/tensorflow/lite/kernels/internal/reference/comparisons.cc
	../tflite-micro/tensorflow/lite/kernels/internal/reference/portable_tensor_utils.cc
	../tflite-micro/tensorflow/lite/kernels/internal/runtime_shape.cc
	../tflite-micro/tensorflow/lite/kernels/internal/tensor_ctypes.cc
	../tflite-micro/tensorflow/lite/kernels/internal/tensor_utils.cc
	../tflite-micro/tensorflow/lite/kernels/kernel_util.cc
	../tflite-micro/tensorflow/lite/micro/arena_allocator/non_persistent_arena_buffer_allocator.cc
	../tflite-micro/tensorflow/lite/micro/arena_allocator/persistent_arena_buffer_allocator.cc
	../tflite-micro/tensorflow/lite/micro/arena_allocator/recording_single_arena_buffer_allocator.cc
	../tflite-micro/tensorflow/lite/micro/arena_allocator/single_arena_buffer_allocator.cc
	../tflite-micro/tensorflow/lite/micro/fake_micro_context.cc
	../tflite-micro/tensorflow/lite/micro/flatbuffer_utils.cc
	../tflite-micro/tensorflow/lite/micro/hexdump.cc
	../tflite-micro/tensorflow/lite/micro/kernels/activations.cc
	../tflite-micro/tensorflow/lite/micro/kernels/activations_common.cc
	../tflite-micro/tensorflow/lite/micro/kernels/add_common.cc
	../tflite-micro/tensorflow/lite/micro/kernels/add_n.cc
	../tflite-micro/tensorflow/lite/micro/kernels/arg_min_max.cc
	../tflite-micro/tensorflow/lite/micro/kernels/assign_variable.cc
	../tflite-micro/tensorflow/lite/micro/kernels/batch_matmul_common.cc
	../tflite-micro/tensorflow/lite/micro/kernels/batch_to_space_nd.cc
	../tflite-micro/tensorflow/lite/micro/kernels/broadcast_args.cc
	../tflite-micro/tensorflow/lite/micro/kernels/broadcast_to.cc
	../tflite-micro/tensorflow/lite/micro/kernels/call_once.cc
	../tflite-micro/tensorflow/lite/micro/kernels/cast.cc
	../tflite-micro/tensorflow/lite/micro/kernels/ceil.cc
	../tflite-micro/tensorflow/lite/micro/kernels/circular_buffer.cc
	../tflite-micro/tensorflow/lite/micro/kernels/circular_buffer_common.cc
	../tflite-micro/tensorflow/lite/micro/kernels/comparisons.cc
	../tflite-micro/tensorflow/lite/micro/kernels/concatenation.cc
	../tflite-micro/tensorflow/lite/micro/kernels/conv_common.cc
	../tflite-micro/tensorflow/lite/micro/kernels/cumsum.cc
	../tflite-micro/tensorflow/lite/micro/kernels/decode.cc
	../tflite-micro/tensorflow/lite/micro/kernels/decode_state.cc
	../tflite-micro/tensorflow/lite/micro/kernels/decode_state_huffman.cc
	../tflite-micro/tensorflow/lite/micro/kernels/decode_state_lut.cc
	../tflite-micro/tensorflow/lite/micro/kernels/decode_state_prune.cc
	../tflite-micro/tensorflow/lite/micro/kernels/decompress.cc
	../tflite-micro/tensorflow/lite/micro/kernels/decompress_common.cc
	../tflite-micro/tensorflow/lite/micro/kernels/depth_to_space.cc
	../tflite-micro/tensorflow/lite/micro/kernels/depthwise_conv_common.cc
	../tflite-micro/tensorflow/lite/micro/kernels/dequantize.cc
	../tflite-micro/tensorflow/lite/micro/kernels/dequantize_common.cc
	../tflite-micro/tensorflow/lite/micro/kernels/detection_postprocess.cc
	../tflite-micro/tensorflow/lite/micro/kernels/div.cc
	../tflite-micro/tensorflow/lite/micro/kernels/dynamic_update_slice.cc
	../tflite-micro/tensorflow/lite/micro/kernels/elementwise.cc
	../tflite-micro/tensorflow/lite/micro/kernels/elu.cc
	../tflite-micro/tensorflow/lite/micro/kernels/embedding_lookup.cc
	../tflite-micro/tensorflow/lite/micro/kernels/ethosu.cc
	../tflite-micro/tensorflow/lite/micro/kernels/exp.cc
	../tflite-micro/tensorflow/lite/micro/kernels/expand_dims.cc
	../tflite-micro/tensorflow/lite/micro/kernels/fill.cc
	../tflite-micro/tensorflow/lite/micro/kernels/floor.cc
	../tflite-micro/tensorflow/lite/micro/kernels/floor_div.cc
	../tflite-micro/tensorflow/lite/micro/kernels/floor_mod.cc
	../tflite-micro/tensorflow/lite/micro/kernels/fully_connected_common.cc
	../tflite-micro/tensorflow/lite/micro/kernels/gather.cc
	../tflite-micro/tensorflow/lite/micro/kernels/gather_nd.cc
	../tflite-micro/tensorflow/lite/micro/kernels/hard_swish.cc
	../tflite-micro/tensorflow/lite/micro/kernels/hard_swish_common.cc
	../tflite-micro/tensorflow/lite/micro/kernels/if.cc
	../tflite-micro/tensorflow/lite/micro/kernels/kernel_runner.cc
	../tflite-micro/tensorflow/lite/micro/kernels/kernel_util.cc
	../tflite-micro/tensorflow/lite/micro/kernels/l2_pool_2d.cc
	../tflite-micro/tensorflow/lite/micro/kernels/l2norm.cc
	../tflite-micro/tensorflow/lite/micro/kernels/leaky_relu.cc
	../tflite-micro/tensorflow/lite/micro/kernels/leaky_relu_common.cc
	../tflite-micro/tensorflow/lite/micro/kernels/log_softmax.cc
	../tflite-micro/tensorflow/lite/micro/kernels/logical.cc
	../tflite-micro/tensorflow/lite/micro/kernels/logical_common.cc
	../tflite-micro/tensorflow/lite/micro/kernels/logistic.cc
	../tflite-micro/tensorflow/lite/micro/kernels/logistic_common.cc
	../tflite-micro/tensorflow/lite/micro/kernels/lstm_eval.cc
	../tflite-micro/tensorflow/lite/micro/kernels/lstm_eval_common.cc
	../tflite-micro/tensorflow/lite/micro/kernels/micro_tensor_utils.cc
	../tflite-micro/tensorflow/lite/micro/kernels/mirror_pad.cc
	../tflite-micro/tensorflow/lite/micro/kernels/mul_common.cc
	../tflite-micro/tensorflow/lite/micro/kernels/neg.cc
	../tflite-micro/tensorflow/lite/micro/kernels/pack.cc
	../tflite-micro/tensorflow/lite/micro/kernels/pad_common.cc
	../tflite-micro/tensorflow/lite/micro/kernels/pooling_common.cc
	../tflite-micro/tensorflow/lite/micro/kernels/prelu.cc
	../tflite-micro/tensorflow/lite/micro/kernels/prelu_common.cc
	../tflite-micro/tensorflow/lite/micro/kernels/quantize.cc
	../tflite-micro/tensorflow/lite/micro/kernels/quantize_common.cc
	../tflite-micro/tensorflow/lite/micro/kernels/read_variable.cc
	../tflite-micro/tensorflow/lite/micro/kernels/reduce.cc
	../tflite-micro/tensorflow/lite/micro/kernels/reduce_common.cc
	../tflite-micro/tensorflow/lite/micro/kernels/reshape.cc
	../tflite-micro/tensorflow/lite/micro/kernels/reshape_common.cc
	../tflite-micro/tensorflow/lite/micro/kernels/resize_bilinear.cc
	../tflite-micro/tensorflow/lite/micro/kernels/resize_nearest_neighbor.cc
	../tflite-micro/tensorflow/lite/micro/kernels/reverse.cc
	../tflite-micro/tensorflow/lite/micro/kernels/round.cc
	../tflite-micro/tensorflow/lite/micro/kernels/select.cc
	../tflite-micro/tensorflow/lite/micro/kernels/shape.cc
	../tflite-micro/tensorflow/lite/micro/kernels/slice.cc
	../tflite-micro/tensorflow/lite/micro/kernels/softmax_common.cc
	../tflite-micro/tensorflow/lite/micro/kernels/space_to_batch_nd.cc
	../tflite-micro/tensorflow/lite/micro/kernels/space_to_depth.cc
	../tflite-micro/tensorflow/lite/micro/kernels/split.cc
	../tflite-micro/tensorflow/lite/micro/kernels/split_v.cc
	../tflite-micro/tensorflow/lite/micro/kernels/squared_difference.cc
	../tflite-micro/tensorflow/lite/micro/kernels/squeeze.cc
	../tflite-micro/tensorflow/lite/micro/kernels/strided_slice.cc
	../tflite-micro/tensorflow/lite/micro/kernels/strided_slice_common.cc
	../tflite-micro/tensorflow/lite/micro/kernels/sub.cc
	../tflite-micro/tensorflow/lite/micro/kernels/sub_common.cc
	../tflite-micro/tensorflow/lite/micro/kernels/svdf_common.cc
	../tflite-micro/tensorflow/lite/micro/kernels/tanh.cc
	../tflite-micro/tensorflow/lite/micro/kernels/transpose_common.cc
	../tflite-micro/tensorflow/lite/micro/kernels/unpack.cc
	../tflite-micro/tensorflow/lite/micro/kernels/var_handle.cc
	../tflite-micro/tensorflow/lite/micro/kernels/while.cc
	../tflite-micro/tensorflow/lite/micro/kernels/zeros_like.cc
	../tflite-micro/tensorflow/lite/micro/memory_helpers.cc
	../tflite-micro/tensorflow/lite/micro/memory_planner/greedy_memory_planner.cc
	../tflite-micro/tensorflow/lite/micro/memory_planner/linear_memory_planner.cc
	../tflite-micro/tensorflow/lite/micro/memory_planner/non_persistent_buffer_planner_shim.cc
	../tflite-micro/tensorflow/lite/micro/micro_allocation_info.cc
	../tflite-micro/tensorflow/lite/micro/micro_allocator.cc
	../tflite-micro/tensorflow/lite/micro/micro_context.cc
	../tflite-micro/tensorflow/lite/micro/micro_interpreter.cc
	../tflite-micro/tensorflow/lite/micro/micro_interpreter_context.cc
	../tflite-micro/tensorflow/lite/micro/micro_interpreter_graph.cc
	../tflite-micro/tensorflow/lite/micro/micro_op_resolver.cc
	../tflite-micro/tensorflow/lite/micro/micro_profiler.cc
	../tflite-micro/tensorflow/lite/micro/micro_resource_variable.cc
	../tflite-micro/tensorflow/lite/micro/micro_utils.cc
	../tflite-micro/tensorflow/lite/micro/mock_micro_graph.cc
	../tflite-micro/tensorflow/lite/micro/recording_micro_allocator.cc
	../tflite-micro/tensorflow/lite/micro/system_setup.cc
	../tflite-micro/tensorflow/lite/micro/test_helper_custom_ops.cc
	../tflite-micro/tensorflow/lite/micro/test_helpers.cc
	../tflite-micro/tensorflow/lite/micro/tflite_bridge/flatbuffer_conversions_bridge.cc
	../tflite-micro/tensorflow/lite/micro/tflite_bridge/micro_error_reporter.cc
	../tflite-micro/tensorflow/lite/micro/micro_log.cc
)

FILE(GLOB CMSIS_NN_SOURCES ${THIRD_PARTY_DIR}/)

target_compile_options(tflm-example PRIVATE
  -fno-unwind-tables
  -ffunction-sections
  -fdata-sections
  -funsigned-char
  -fno-delete-null-pointer-checks
  # -fomit-frame-pointer # if you want
  -fno-common
  -fno-builtin-printf

  # Without these there's an error about virtual operator delete being private
  -fno-rtti
  -fno-exceptions 
  -fno-threadsafe-statics
  -mfloat-abi=hard
  -mcpu=cortex-m7
  -mfpu=vfpv4-d16
  -mthumb

  -Wno-unused-parameter
)

target_compile_definitions(tflm-example PRIVATE 
	CMSIS_NN=1
	TF_LITE_STATIC_MEMORY=1
	TF_LITE_MCU_DEBUG_LOG=1
	OPTIONAL_RESTRICT_KEYWORD=__restrict
	ARM_MATH_DSP=1
	# NDEBUG=1 
)

target_sources(tflm-example PUBLIC 
	${TFLITE_SOURCES}
	tflm_model.cpp
	micro_time.cpp
)
