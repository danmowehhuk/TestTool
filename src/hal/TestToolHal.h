#ifndef TESTTOOL_TESTTOOLHAL_H
#define TESTTOOL_TESTTOOLHAL_H

#include "FlashStr.h"

#ifndef NO_ARDUINO
#include <Arduino.h>
#else
#include <avr/pgmspace.h>
#include <stdint.h>
#include <stddef.h>
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

// The one true hardware primitive this file needs from a HAL backend:
// writing a single byte to the UART. Declared, not defined - a HAL backend
// (e.g. AvrHal::Uart0::write()) needs to provide this. Everything below is
// generic C++ built on top of it, doing the same job Serial.print/println
// do above, just without Serial to lean on.
void write(uint8_t b);

inline void print(const char* s) { while (s && *s) write((uint8_t)*s++); }
inline void print(char c) { write((uint8_t)c); }
inline void print(int v) {
  unsigned int uv;
  if (v < 0) {
    write((uint8_t)'-');
    uv = (unsigned int)(-(long)v);
  } else {
    uv = (unsigned int)v;
  }
  char digits[5];  // max "32768" (5 digits) for a 16-bit int magnitude
  uint8_t n = 0;
  do {
    digits[n++] = '0' + (uv % 10);
    uv /= 10;
  } while (uv > 0);
  while (n > 0) write((uint8_t)digits[--n]);
}
inline void print(const FlashStr* s) {
  const char* p = reinterpret_cast<const char*>(s);
  char c;
  while ((c = (char)pgm_read_byte(p++)) != '\0') write((uint8_t)c);
}
inline void println(const char* s) { print(s); print('\r'); print('\n'); }
inline void println(int v) { print(v); print('\r'); print('\n'); }
inline void println(const FlashStr* s) { print(s); print('\r'); print('\n'); }

#endif

// freeMemory() reads AVR-libc's heap high-water-mark globals directly, so
// it works identically on Arduino or bare-metal AVR - it's gated on
// __AVR__ (set by the compiler itself), not NO_ARDUINO, and needs nothing
// from a HAL backend. Stubbed on any other target until there's a real
// equivalent to provide.
#if defined(__AVR__)
inline int freeMemory() {
  extern char __heap_start, *__brkval;
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

}  // namespace TestToolHal

#endif
