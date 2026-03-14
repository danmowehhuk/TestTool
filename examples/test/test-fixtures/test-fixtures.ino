#include <TestTool.h>

// A simple stateful accumulator. In a real test suite, this would be
// a class from the library under test.
class Accumulator {
  int _sum;
  int _count;

public:
  Accumulator() : _sum(0), _count(0) {}

  void add(int value) { _sum += value; _count++; }
  int getSum() const { return _sum; }
  int getCount() const { return _count; }
  void reset() { _sum = 0; _count = 0; }
};

// Global fixture shared across all tests.
// Because it is global, any state accumulated during one test persists
// into the next — unless the fixture is explicitly reset between tests.
Accumulator acc;

// before() runs before each test. It resets the fixture to a known,
// clean state, making each test independent of the others. Without this,
// test 2 would see whatever state test 1 left behind, and tests would
// only pass when run in a specific order.
void before() {
  acc.reset();
}

void testStartsAtZero(TestInvocation* t) {
  t->setName(F("Fresh fixture starts at zero"));
  // before() guarantees this even if a prior test added values
  t->assert(acc.getSum() == 0, F("Sum should be 0"));
  t->assert(acc.getCount() == 0, F("Count should be 0"));
}

void testSingleAdd(TestInvocation* t) {
  t->setName(F("Adding one value"));
  acc.add(5);
  t->assert(acc.getSum() == 5, F("Sum should be 5"));
  t->assert(acc.getCount() == 1, F("Count should be 1"));
}

void testMultipleAdds(TestInvocation* t) {
  t->setName(F("Adding multiple values"));
  acc.add(10);
  acc.add(20);
  acc.add(30);
  t->assert(acc.getSum() == 60, F("Sum should be 60"));
  t->assert(acc.getCount() == 3, F("Count should be 3"));
}

void testNegativeValues(TestInvocation* t) {
  t->setName(F("Adding negative values"));
  acc.add(100);
  acc.add(-40);
  t->assert(acc.getSum() == 60, F("Sum should be 60"));
  t->assert(acc.getCount() == 2, F("Count should be 2"));
}

void setup() {
  Serial.begin(9600);
  while (!Serial);

  TestFunction tests[] = {
    testStartsAtZero,
    testSingleAdd,
    testMultipleAdds,
    testNegativeValues
  };

  // Pass before as the setup callback. nullptr means no after() teardown
  // is needed — this fixture has no heap allocations to free.
  runTestSuite(tests, before, nullptr);
}

void loop() {}
