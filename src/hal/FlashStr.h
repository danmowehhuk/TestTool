#ifndef TESTTOOL_FLASHSTR_H
#define TESTTOOL_FLASHSTR_H

// A type-tag for "this pointer is to a flash-resident string," used purely
// for overload resolution - never defined/instantiated, only ever seen as
// FlashStr*. On Arduino this is just an alias for the framework's own
// (reserved-name) __FlashStringHelper. Off Arduino, the real type and F()
// macro live in BareMetalHAL (shared across every library, since the
// implementation is target-specific, not TestTool-specific) - this is
// just a local alias so TestTool's own code can keep saying bare
// `FlashStr` either way.
//
// Branches on NO_ARDUINO directly - the single flag every library checks.
// Which non-Arduino backend is active (HAL_AVR, HAL_ESP32, ...) is
// BareMetalHAL's concern, never this file's.

#ifndef NO_ARDUINO

#include <Arduino.h>
using FlashStr = __FlashStringHelper;
// F() is already provided by Arduino.h

#else

#include <BareMetalHAL.h>
using FlashStr = BareMetalHAL::FlashStr;
// F() is already provided by BareMetalHAL.h

#endif

#endif
