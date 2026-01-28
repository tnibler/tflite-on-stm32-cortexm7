# Tensorflow Lite on STM32H7rs/Cortex M7

This is a minimal example showing how to:
 - Create a Keras model, convert it to a quantized `int8` TFLite-Micro (now AI🧠Edge🤳Lite🪶RT🚀) model
 - Execute it with the TFLite-Micro (now AI🧠Edge🤳Lite🪶RT🚀) interpreter
 - Compile and link with ARM [CMSIS-NN](https://github.com/ARM-software/CMSIS-NN) kernels
 - Run it from Rust with [`embassy`](https://github.com/embassy-rs/embassy) (optional)

More at https://tnibler.de/bites/ml-on-stm32/

## Building

Requirements:
 - `arm-none-eabi` toolchain
 - CMake
 - Python (see `pyproject.toml`): `tensorflow`, `ai_edge_litert`, `numpy`

It's easiest to just pull in the entire TFLite-Micro (now AI🧠Edge🤳Lite🪶RT🚀) tree,
which also has scripts to vendor in and patch a few dependencies.
It also downloads a `CMSIS-NN` tarball, but here we're compiling agains the git version.

```shell
git submodule update --init
cd tflite-micro
make -f tensorflow/lite/micro/tools/make/Makefile third_party_downloads OPTIMIZED_KERNEL_DIR=cmsis_nn
cd ..
```

Export the TFLite (now AI🧠Edge🤳Lite🪶RT🚀) model:

```shell
uv run python make_model.py
xxd -i example_quant.tflite > model-lib/model_data.h

# Run the model with the same inputs as in C++
uv run python run_model.py
```

Now build the C++ static library. You should see `libtflm-example.a` in `model-lib/build`.

```shell
cd model-lib
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j8
```

#### Linking

You can link this static library into any project now and call the exported `extern "C"` functions 
at the bottom of `model-lib/tflm_model.cpp`.

Some platform specific functions are not yet defined, you need to provide implementations for:

```cpp
// tflite-micro/tensorflow/lite/micro_debug.h (micro_debug.cc is not included in model-lib/CMakeLists.txt)
extern "C" void DebugLog(const char* format, va_list args);
// tflite-micro/tensorflow/lite/micro_time.h
uint32_t GetCurrentTimeTicks();
uint32_t ticks_per_second();
```

Linker flags are set in `rust-firmware/firmware/.cargo/config.toml`:

```
  -mcpu=cortex-m7
  -mthumb
  -mfpu=vfpv4-d16
  -mfloat-abi=hard

  --specs=nano.specs
  -lc
  -lgcc
```

The last three are to make TFLite-Micro (now AI🧠Edge🤳Lite🪶RT🚀) run as-is,
you might be able to modify it to need fewer symbols from libc and co.


#### Dedicated Region for Large Model Data

In `model-lib/tflm_model.cpp`, the array containing the model data is placed in a dedicated region:

```cpp
extern uint8_t __attribute__ ((section(".model_data"))) example_quant_tflite[];
```

This is mostly for demonstration purposes, the linker script in `rust-firmware/firmware/memory.x` defines a large flash region to hold this static data. 
You can remove or it adjust it to your needs, keeping in mind that you're probably limited by RAM and not flash space for model weights.

```
MEMORY
{
    /* [...] */
    MODEL_DATA : ORIGIN = 0x70400000, LENGTH = 12M
    /* [...] */
}

SECTIONS
{
    .model_data : {
        . = ALIGN(32);
    } > MODEL_DATA
}
```

#### Flashing to Nucleo H7S3L8

This probably won't work on any other board as is. 
The firmware is stored and loaded from the 16MB external flash by the bootloader 
(big thank you to [kevswims](https://github.com/kevswims/) for figuring this out).

```
cd rust-firmware/bootloader
# Flash bootloader (internal flash)
cargo run --release # just ctrl-c after flashing
cd ../firmware
# Flash firmware (external SPI flash)
cargo run --release
```
