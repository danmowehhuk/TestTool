#ifndef __test_hal_target_h
#define __test_hal_target_h

// Derives a single "is Arduino available" macro from the full set of known
// non-Arduino target flags, so call sites never negate target flags directly.
// Adding a new non-Arduino target (e.g. HAL_ESP32) means adding it to the
// condition below - nowhere else.
#if !defined(HAL_AVR)
#define HAL_ARDUINO_AVAILABLE 1
#endif

#endif
