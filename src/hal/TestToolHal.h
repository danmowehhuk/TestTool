#ifndef TESTTOOL_TESTTOOLHAL_H
#define TESTTOOL_TESTTOOLHAL_H

#include "FlashStr.h"

#ifndef NO_ARDUINO
#include <Arduino.h>
#else
#include <BareMetalHAL.h>
#include <stdint.h>
#include <stddef.h>
#endif

// Declared at file scope deliberately, not inside freeMemory()'s body
// below: a block-scope `extern` declared inside a function nested in a
// namespace binds to that namespace (TestToolHal::__heap_start), not the
// real global symbol avr-libc's linker script defines - verified
// empirically, not assumed (same bug, same fix, as BareMetalHAL's
// MemoryHAL.h). Only actually referenced on the Arduino+__AVR__ branch
// below, but harmless to declare unconditionally.
#if defined(__AVR__)
extern char __heap_start, *__brkval;
#endif

namespace TestToolHal {

#ifndef NO_ARDUINO

inline void print(const char* s) { Serial.print(s); }
inline void print(char c) { Serial.print(c); }
inline void print(int v) { Serial.print(v); }
inline void print(const FlashStr* s) { Serial.print(s); }
inline void println(const char* s) { Serial.println(s); }
inline void println(int v) { Serial.println(v); }
inline void println(const FlashStr* s) { Serial.println(s); }

#else

// The formatting logic (int-to-decimal-ASCII, etc.) moved into
// BareMetalHAL itself - it's generic, reusable infrastructure with no
// hardware dependency, not something specific to TestTool's job of
// reporting test results. A using-declaration pulls the whole overload
// set in under TestToolHal::print/println, so call sites elsewhere in
// this library don't need to change.
using BareMetalHAL::Uart0::print;
using BareMetalHAL::Uart0::println;

#endif

// freeMemory(): on Arduino, reads AVR-libc's heap high-water-mark globals
// directly (gated on __AVR__, not NO_ARDUINO, since these are avr-libc
// internals available on any AVR build) - kept local rather than pulled
// from BareMetalHAL so Arduino-only users of this library never need
// BareMetalHAL installed at all. Off Arduino, delegates to BareMetalHAL's
// copy of the exact same logic.
#ifndef NO_ARDUINO
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
inline int freeMemory() {
  return -1;
}
#endif
#else
inline int freeMemory() { return BareMetalHAL::freeMemory(); }
#endif

}  // namespace TestToolHal

#endif
