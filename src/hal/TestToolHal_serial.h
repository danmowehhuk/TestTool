#ifndef __test_TestToolHal_serial_h
#define __test_TestToolHal_serial_h

#include "hal_target.h"
#include "TestToolHal_progmem.h"  // for __FlashStringHelper

namespace TestToolHal {

#if defined(HAL_ARDUINO_AVAILABLE)
#include <Arduino.h>

inline void print(const char* s) { ::Serial.print(s); }
inline void print(char c) { ::Serial.print(c); }
inline void print(int v) { ::Serial.print(v); }
inline void print(const __FlashStringHelper* s) { ::Serial.print(s); }
inline void println(const char* s) { ::Serial.println(s); }
inline void println(int v) { ::Serial.println(v); }
inline void println(const __FlashStringHelper* s) { ::Serial.println(s); }

#else  // HAL_AVR: raw USART0 TX, ATmega2560 register-level, 9600 baud @ 16MHz.
       // Compile-checked only in this pass, not verified against real/simulated
       // hardware - see the migration writeup for what's actually been tested.
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdio.h>

inline void serialBeginOnce() {
    static bool started = false;
    if (started) return;
    const uint16_t ubrr = (F_CPU / (9600UL * 16UL)) - 1;
    UBRR0H = (uint8_t)(ubrr >> 8);
    UBRR0L = (uint8_t)ubrr;
    UCSR0B = (1 << TXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
    started = true;
}

inline void writeByte(uint8_t b) {
    serialBeginOnce();
    while (!(UCSR0A & (1 << UDRE0))) { }
    UDR0 = b;
}

inline void print(const char* s) { while (s && *s) writeByte((uint8_t)*s++); }
inline void print(char c) { writeByte((uint8_t)c); }
inline void print(int v) {
    char buf[12];
    snprintf(buf, sizeof(buf), "%d", v);
    print(buf);
}
inline void print(const __FlashStringHelper* s) {
    const char* p = reinterpret_cast<const char*>(s);
    char c;
    while ((c = (char)pgm_read_byte(p++)) != '\0') writeByte((uint8_t)c);
}
inline void println(const char* s) { print(s); print('\r'); print('\n'); }
inline void println(int v) { print(v); print('\r'); print('\n'); }
inline void println(const __FlashStringHelper* s) { print(s); print('\r'); print('\n'); }

#endif

}  // namespace TestToolHal

#endif
