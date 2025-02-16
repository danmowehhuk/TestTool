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
  t->assert((myTestInt >= 0), F("myTestInt must be positive"));
  t->assert((myTestInt <= 100), F("myTestInt must be <= 100"));
}

void testThatFails(TestInvocation* t) {
  t->setName(F("Test something that fails"));
  String myString("abc");
  t->assert(myString.startsWith("F"), F("myString must start with F"));
  t->assert(myString.length() == 3, "nope");
}

void testAdd(TestInvocation* t) {
  t->setName(F("Test adding 1+1"));
  t->assert(( 1 + 1 ) == 2, F("Something wrong with adding!"));
}

void testAllTheThings(TestInvocation* t) {
  t->setName("Test all the strings!");
  t->assertEqual("abc", "abc", "hi");
  t->assertEqual("abc", F("abc"), "hi");
  t->assertEqual("abc", "abc", F("hi"));
  t->assertEqual("abc", F("abc"), F("hi"));
  t->assertEqual(F("abc"), "abc", "hi");
  t->assertEqual(F("abc"), F("abc"), "hi");
  t->assertEqual(F("abc"), "abc", F("hi"));
  t->assertEqual(F("abc"), F("abc"), F("hi"));
  t->assertEqual("abc", "abc");
  t->assertEqual("abc", F("abc"));
  t->assertEqual(F("abc"), "abc");
  t->assertEqual(F("abc"), F("abc"));
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
