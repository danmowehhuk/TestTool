// Bare-metal AVR port of ../test-suite/test-suite.ino - same test logic,
// built with -DNO_ARDUINO -DHAL_AVR via avr-g++ directly (see build.sh),
// no Arduino dependency at all: no arduino-cli, no Arduino core, no
// Serial, no String. Uses the exact same verify/verifyEqual/F() calls as
// the Arduino version, unchanged - BareMetalHAL's FlashStr makes F() work
// identically on both branches, so unlike the earlier (abandoned)
// per-library HAL experiment, this file needs no _P-suffixed variants at
// all.

#include <TestTool.h>
#include <BareMetalHAL.h>  // for Uart0::begin() - main() below calls it directly,
                            // so this file should say so, not rely on TestTool.h's
                            // own transitive include of it
#include <string.h>

void before() {
}

void after() {
}

void testThatPasses(TestInvocation* t) {
  t->setName(F("Test something that passes"));
  int myTestInt = 5;
  t->verify((myTestInt >= 0), F("myTestInt must be positive"));
  t->verify((myTestInt <= 100), F("myTestInt must be <= 100"));
}

void testThatFails(TestInvocation* t) {
  t->setName(F("Test something that fails"));
  // No Arduino String off Arduino - plain const char* instead.
  const char* myString = "abc";
  t->verify(myString[0] == 'F', F("myString must start with F"));
  t->verify(strlen(myString) == 3, "nope");
}

void testAdd(TestInvocation* t) {
  t->setName(F("Test adding 1+1"));
  t->verify(( 1 + 1 ) == 2, F("Something wrong with adding!"));
}

void testAllTheThings(TestInvocation* t) {
  t->setName("Test all the strings!");
  t->verifyEqual("abc", "abc", "hi");
  t->verifyEqual("abc", F("abc"), "hi");
  t->verifyEqual("abc", "abc", F("hi"));
  t->verifyEqual("abc", F("abc"), F("hi"));
  t->verifyEqual(F("abc"), "abc", "hi");
  t->verifyEqual(F("abc"), F("abc"), "hi");
  t->verifyEqual(F("abc"), "abc", F("hi"));
  t->verifyEqual(F("abc"), F("abc"), F("hi"));
  t->verifyEqual("abc", "abc");
  t->verifyEqual("abc", F("abc"));
  t->verifyEqual(F("abc"), "abc");
  t->verifyEqual(F("abc"), F("abc"));
}

int main() {
  // The caller's job, not TestToolHal's - see BareMetalHAL/UartHAL.h.
  BareMetalHAL::Uart0::begin(9600);

  TestFunction tests[] = {
    testThatPasses,
    testThatFails,
    testAdd,
    testAllTheThings
  };

  runTestSuite(tests, before, after);

  while (1) {}
  return 0;
}
