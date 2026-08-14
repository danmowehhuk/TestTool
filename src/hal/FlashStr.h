#ifndef TESTTOOL_FLASHSTR_H
#define TESTTOOL_FLASHSTR_H

// A type-tag for "this pointer is to a flash-resident string," used purely
// for overload resolution - never defined/instantiated, only ever seen as
// FlashStr*. On Arduino this is just an alias for the framework's own
// (reserved-name) __FlashStringHelper. Off Arduino we need our own,
// because __FlashStringHelper doesn't exist without Arduino.h - defined
// under a project-owned name rather than reusing that reserved identifier
// (see d8fba9e's header-guard fix for the same category of issue).
//
// F(x) mirrors Arduino's own trick: PSTR places the literal in flash and
// returns a const char*; the cast is only there so overload resolution can
// tell a flash string apart from a RAM one.
//
// Branches on NO_ARDUINO directly - the single flag every library checks,
// same as every other file under hal/. Which non-Arduino backend is active
// (HAL_AVR, HAL_ESP32, ...) is BareMetalHal's concern, not this file's.
//
// The non-Arduino branch below is AVR-specific (PSTR/pgmspace). When a
// second non-Arduino target exists (HAL_ESP32, HAL_ARM), this needs a
// per-target split - flash and RAM aren't actually distinct address spaces
// on those targets (see the BareMetalHal design notes), so F(x) there can
// likely just expand to x unchanged.

#ifndef NO_ARDUINO

#include <Arduino.h>
using FlashStr = __FlashStringHelper;
// F() is already provided by Arduino.h

#else

#include <avr/pgmspace.h>
class FlashStr;
#define F(string_literal) (reinterpret_cast<const FlashStr*>(PSTR(string_literal)))

#endif

#endif
