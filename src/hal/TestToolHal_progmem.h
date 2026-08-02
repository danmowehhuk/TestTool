#ifndef __test_TestToolHal_progmem_h
#define __test_TestToolHal_progmem_h

#include "hal_target.h"

#if defined(HAL_ARDUINO_AVAILABLE)
#include <Arduino.h>
#define TESTTOOL_HAL_FLASH_STR(s) F(s)
// __FlashStringHelper is already declared by Arduino.h.

#else
#include <avr/pgmspace.h>
#include <stdio.h>  // snprintf_P/vsnprintf_P live here, not in avr/pgmspace.h

// Arduino.h isn't available here, so there's no built-in "pointer to a flash
// string" type distinct from a RAM `const char*`. __FlashStringHelper is only
// ever used as an opaque pointer tag - even inside Arduino itself it's never
// defined, only forward-declared and reinterpret_cast to/from `const char*`.
// Reproducing exactly that trick here means it stays a distinct type and
// TestInvocation's existing overloads keep disambiguating flash strings from
// RAM strings correctly, without changing a single one of those signatures.
class __FlashStringHelper;
#define TESTTOOL_HAL_FLASH_STR(s) (reinterpret_cast<const __FlashStringHelper*>(PSTR(s)))

#endif

#endif
