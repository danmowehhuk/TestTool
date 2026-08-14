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

// All of TestTool's actual hardware access - the UART write and the
// flash-string byte read - now comes from BareMetalHAL. Everything below
// is still generic C++ built on top of those two primitives (int-to-string
// formatting, etc.), same as the Arduino branch above just calling
// BareMetalHAL::Uart0::write() instead of Serial.write().
inline void print(const char* s) { while (s && *s) BareMetalHAL::Uart0::write((uint8_t)*s++); }
inline void print(char c) { BareMetalHAL::Uart0::write((uint8_t)c); }
inline void print(int v) {
  unsigned int uv;
  if (v < 0) {
    BareMetalHAL::Uart0::write((uint8_t)'-');
    uv = (unsigned int)(-(long)v);
  } else {
    uv = (unsigned int)v;
  }
  // sizeof(unsigned int)*3 comfortably covers the max decimal digit count
  // for any int width (log10(256) ~= 2.41 digits/byte) - not hardcoded for
  // AVR's 16-bit int, since this same code will run under a future
  // HAL_ARM/HAL_ESP32 backend with a 32-bit int.
  char digits[sizeof(unsigned int) * 3];
  uint8_t n = 0;
  do {
    digits[n++] = '0' + (uv % 10);
    uv /= 10;
  } while (uv > 0);
  while (n > 0) BareMetalHAL::Uart0::write((uint8_t)digits[--n]);
}
inline void print(const FlashStr* s) {
  const char* p = reinterpret_cast<const char*>(s);
  char c;
  while ((c = BareMetalHAL::readByte(p++)) != '\0') BareMetalHAL::Uart0::write((uint8_t)c);
}
inline void println(const char* s) { print(s); print('\r'); print('\n'); }
inline void println(int v) { print(v); print('\r'); print('\n'); }
inline void println(const FlashStr* s) { print(s); print('\r'); print('\n'); }

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
