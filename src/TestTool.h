#ifndef TESTTOOL_TESTTOOL_H
#define TESTTOOL_TESTTOOL_H


#include "hal/FlashStr.h"
#include "hal/TestToolHal.h"
#include "TestInvocation.h"

#ifndef NO_ARDUINO
#include <Arduino.h>
#else
#include <avr/pgmspace.h>
#endif

typedef void (*TestFunction)(TestInvocation*);
typedef void (*PreOrPostFunction)();

inline bool printAndCheckResult(TestInvocation* t) {
  const char* name = t->getName();
  size_t nameLength = t->getNameLength();
  const uint8_t nameWidth = 48;

  TestToolHal::print(F("  "));
  for (size_t i = 0; i < nameWidth; i++) {
    if (i < nameLength) {
      if (t->isNamePmem()) {
        TestToolHal::print((char)pgm_read_byte(&name[i]));
      } else {
        TestToolHal::print(name[i]);
      }
    } else if (i == nameLength) {
      TestToolHal::print(F("..."));
      i += 2;
    } else {
      TestToolHal::print(F(" "));
    }
  }
  TestToolHal::print(F(" "));
  if (t->passed()) {
    TestToolHal::println(F("PASSED"));
  } else {
    TestToolHal::println(F("FAILED"));
    const char* message = t->getMessage();
    if (message) {
      TestToolHal::print(F("    FAILED - "));
      if (t->isMessagePmem()) {
        TestToolHal::println(reinterpret_cast<const FlashStr*>(message));
      } else {
        TestToolHal::println(message);
      }
    }
  }
  return t->passed();
}

inline bool invokeTest(TestFunction test, uint8_t testNum, PreOrPostFunction before, PreOrPostFunction after) {
  TestInvocation t(testNum);
  if (before) before();
  test(&t);
  if (after) after();
  return printAndCheckResult(&t);
}

template <size_t N>
void runTestSuite(TestFunction (&tests)[N], PreOrPostFunction before = nullptr,
          PreOrPostFunction after = nullptr, uint8_t repeats = 1, bool showMem = false) {
  TestToolHal::println(F("Running test suite..."));
  bool success = true;
  for (int i = 0; i < N; i++) {
    for (int r = 0; r < repeats; r++) {
      int memBefore = 0;
      if (showMem) memBefore = TestToolHal::freeMemory();
      success &= invokeTest(tests[i], i, before, after);
      if (showMem) {
        TestToolHal::print(F("          Free mem before: "));
        TestToolHal::print(memBefore);
        TestToolHal::print(F(" after: "));
        TestToolHal::println(TestToolHal::freeMemory());
      }
    }
  }
  if (success) {
    TestToolHal::println(F("All tests passed!"));
  } else {
    TestToolHal::println(F("Test suite failed!"));
  }
}

template <size_t N>
void runTestSuiteShowMem(TestFunction (&tests)[N], PreOrPostFunction before = nullptr,
          PreOrPostFunction after = nullptr, uint8_t repeats = 1) {
  runTestSuite(tests, before, after, repeats, true);
}



#endif
