#ifndef TESTTOOL_TESTTOOLHAL_MEMORY_H
#define TESTTOOL_TESTTOOLHAL_MEMORY_H

#include "hal_target.h"

// freeMemory() reads AVR-libc's heap high-water-mark globals directly, so it
// only has a real implementation where __AVR__ is defined by the compiler
// itself (true for both HAL_ARDUINO_AVAILABLE on an AVR board like the Mega,
// and for HAL_AVR bare-metal builds - both compile with avr-gcc). Checking
// __AVR__ instead of HAL_ARDUINO_AVAILABLE/HAL_AVR is deliberate: those two
// macros are defined so that exactly one of them is always true (see
// hal_target.h), so a guard built from them can never actually catch an
// unsupported target - it would always evaluate true. __AVR__ is the
// compiler's own, always-accurate signal for "this really is an AVR chip",
// which is what freeMemory() actually needs.
#if defined(__AVR__)
extern char __heap_start, *__brkval;
#endif

namespace TestToolHal {

#if defined(__AVR__)
inline int freeMemory() {
  char top;
  if (__brkval == 0) {
    return &top - &__heap_start;
  } else {
    return &top - __brkval;
  }
}
#else
#error "TestToolHal::freeMemory() has no implementation for this target. This library currently only supports AVR (see library.properties: architectures=avr). Add a target-specific implementation here to support a new architecture."
#endif

}  // namespace TestToolHal

#endif
