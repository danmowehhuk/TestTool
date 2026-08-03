#ifndef TESTTOOL_TESTTOOLHAL_MEMORY_H
#define TESTTOOL_TESTTOOLHAL_MEMORY_H

#include "hal_target.h"

// Both HAL_ARDUINO_AVAILABLE and HAL_AVR currently target avr-libc, so the
// same heap high-water-mark trick applies to both. A future non-AVR target
// added to hal_target.h will need its own freeMemory() implementation here,
// the same way TestToolHal_serial.h needs one per target for print/println.
#if defined(HAL_ARDUINO_AVAILABLE) || defined(HAL_AVR)
extern char __heap_start, *__brkval;
#endif

namespace TestToolHal {

#if defined(HAL_ARDUINO_AVAILABLE) || defined(HAL_AVR)
inline int freeMemory() {
  char top;
  if (__brkval == 0) {
    return &top - &__heap_start;
  } else {
    return &top - __brkval;
  }
}
#endif

}  // namespace TestToolHal

#endif
