#include <TestTool.h>

// optional, runs before each test
void before() {
  // do something
}

// optional, runs after each test
void after() {
  // do something
}

void testThatPasses(TestInvocation* t) {
  t->setName(F("Test something that passes"));
  int myTestInt = 5;  
  t->verify((myTestInt >= 0), F("myTestInt must be positive"));
  t->verify((myTestInt <= 100), F("myTestInt must be <= 100"));
}

void testThatFails(TestInvocation* t) {
  t->setName(F("Test something that fails"));
  String myString("abc");
  t->verify(myString.startsWith("F"), F("myString must start with F"));
  t->verify(myString.length() == 3, "nope");
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

void setup() {
  Serial.begin(9600);
  while (!Serial);

  TestFunction tests[] = {
    testThatPasses,
    testThatFails,
    testAdd,
    testAllTheThings
  };

  // just pass nullptr for before or after if you only want one
  runTestSuite(tests, before, after);

  // or to show memory usage before and after each test:
  // runTestSuiteShowMem(tests);

}

void loop() {}
