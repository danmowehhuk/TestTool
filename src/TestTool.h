#ifndef __test_TestTool_h
#define __test_TestTool_h


#include <stdint.h>
#include <stddef.h>
#include "hal/hal_target.h"
#include "hal/TestToolHal_serial.h"
#include "hal/TestToolHal_progmem.h"
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

  TestToolHal::print(TESTTOOL_HAL_FLASH_STR("  "));
  for (size_t i = 0; i < nameWidth; i++) {
    if (i < nameLength) {
      if (t->isNamePmem()) {
        TestToolHal::print((char)pgm_read_byte(&name[i]));
      } else {
        TestToolHal::print(name[i]);
      }
    } else if (i == nameLength) {
      TestToolHal::print(TESTTOOL_HAL_FLASH_STR("..."));
      i += 2;
    } else {
      TestToolHal::print(TESTTOOL_HAL_FLASH_STR(" "));
    }
  }
  TestToolHal::print(TESTTOOL_HAL_FLASH_STR(" "));
  if (t->passed()) {
    TestToolHal::println(TESTTOOL_HAL_FLASH_STR("PASSED"));
  } else {
    TestToolHal::println(TESTTOOL_HAL_FLASH_STR("FAILED"));
    const char* message = t->getMessage();
    if (message) {
      TestToolHal::print(TESTTOOL_HAL_FLASH_STR("    FAILED - "));
      if (t->isMessagePmem()) {
        TestToolHal::println(reinterpret_cast<const __FlashStringHelper*>(message));
      } else {
        TestToolHal::println(message);
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
  TestToolHal::println(TESTTOOL_HAL_FLASH_STR("Running test suite..."));
  bool success = true;
  for (int i = 0; i < N; i++) {
    for (int r = 0; r < repeats; r++) {
      int memBefore = 0;
      if (showMem) memBefore = freeMemory();
      success &= invokeTest(tests[i], i, before, after);
      if (showMem) {
        TestToolHal::print(TESTTOOL_HAL_FLASH_STR("          Free mem before: "));
        TestToolHal::print(memBefore);
        TestToolHal::print(TESTTOOL_HAL_FLASH_STR(" after: "));
        TestToolHal::println(freeMemory());
      }
    }
  }
  if (success) {
    TestToolHal::println(TESTTOOL_HAL_FLASH_STR("All tests passed!"));
  } else {
    TestToolHal::println(TESTTOOL_HAL_FLASH_STR("Test suite failed!"));
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
