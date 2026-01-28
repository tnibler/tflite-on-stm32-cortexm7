#include <cstdint>

extern "C" {
  uint32_t rust_ticks_per_second();
  uint32_t rust_current_time_ticks();
}

namespace tflite {

uint32_t ticks_per_second() { 
  return rust_ticks_per_second();
}
uint32_t GetCurrentTimeTicks() { 
  return rust_current_time_ticks(); 
}

}  // namespace tflite
