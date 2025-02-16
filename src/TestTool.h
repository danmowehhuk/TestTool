#ifndef __test_TestTool_h
#define __test_TestTool_h


#include <Arduino.h>
#include "TestInvocation.h"

extern char __heap_start, *__brkval;

int freeMemory() {
    char top;
    if (__brkval == 0) {
        return &top - &__heap_start;
    } else {
        return &top - __brkval;
    }
}

typedef void (*TestFunction)(TestInvocation*);
typedef void (*PreOrPostFunction)();

bool printAndCheckResult(TestInvocation* t) {
  const char* name = t->getName();
  size_t nameLength = t->getNameLength();
  const uint8_t nameWidth = 48;

  Serial.print(F("  "));
  for (size_t i = 0; i < nameWidth; i++) {
    if (i < nameLength) {
      if (t->isNamePmem()) {
        Serial.print((char)pgm_read_byte(&name[i]));
      } else {
        Serial.print(name[i]);
      }
    } else if (i == nameLength) {
      Serial.print(F("..."));
      i += 2;
    } else {
      Serial.print(F(" "));
    }
  }
  Serial.print(F(" "));
  if (t->passed()) {
    Serial.println(F("PASSED"));
  } else {
    Serial.println(F("FAILED"));
    char* message = t->getMessage();
    if (message) {
      Serial.print(F("    FAILED - "));
      if (t->isMessagePmem()) {
        Serial.println(reinterpret_cast<const __FlashStringHelper*>(message));
      } else {
        Serial.println(message);
      }
    }
  }
  return t->passed();
}

bool invokeTest(TestFunction test, uint8_t testNum, PreOrPostFunction before, PreOrPostFunction after) {
  TestInvocation t(testNum);
  if (before) before();
  test(&t);
  if (after) after();
  return printAndCheckResult(&t);
}

template <size_t N>
void runTestSuite(TestFunction (&tests)[N], PreOrPostFunction before, 
          PreOrPostFunction after, bool showMem, uint8_t repeats = 1) {
  Serial.println(F("Running test suite..."));
  bool success = true;
  for (int i = 0; i < N; i++) {
    for (int r = 0; r < repeats; r++) {
      int memBefore = 0;
      if (showMem) memBefore = freeMemory();
      success &= invokeTest(tests[i], i, before, after);
      if (showMem) {
        Serial.print(F("          Free mem before: "));
        Serial.print(memBefore);
        Serial.print(F(" after: "));
        Serial.println(freeMemory());
      }
    }
  }
  if (success) {
    Serial.println(F("All tests passed!"));
  } else {
    Serial.println(F("Test suite failed!"));
  }
}

template <size_t N>
void runTestSuite(TestFunction (&tests)[N], uint8_t repeats = 1) {
  runTestSuite(tests, nullptr, nullptr, false, repeats);
}

template <size_t N>
void runTestSuite(TestFunction (&tests)[N], PreOrPostFunction before, 
          PreOrPostFunction after, uint8_t repeats = 1) {
  runTestSuite(tests, before, after, false, repeats);
}

template <size_t N>
void runTestSuiteShowMem(TestFunction (&tests)[N], uint8_t repeats = 1) {
  runTestSuite(tests, nullptr, nullptr, true, repeats);
}

template <size_t N>
void runTestSuiteShowMem(TestFunction (&tests)[N], PreOrPostFunction before, 
          PreOrPostFunction after, uint8_t repeats = 1) {
  runTestSuite(tests, before, after, true, repeats);
}



#endif
