#include <TestTool.h>

#include <stdlib.h>
#include <string.h>

// A simple string cache that allocates memory on the heap.
// It properly frees its buffer in clear() and in its destructor.
class StringCache {
  char* _buf;

public:
  StringCache() : _buf(nullptr) {}
  ~StringCache() { clear(); }

  void store(const char* s) {
    clear();
    size_t len = strlen(s);
    _buf = (char*)malloc(len + 1);
    if (_buf) strcpy(_buf, s);
  }

  const char* retrieve() const { return _buf ? _buf : ""; }

  void clear() {
    if (_buf) {
      free(_buf);
      _buf = nullptr;
    }
  }
};

// The fixture is a global so it is shared across all tests.
StringCache cache;

// after() runs after each test. It frees the heap allocation so that the
// post-test free-memory reading returns to the same baseline as before
// the test ran. Without this cleanup, free memory would decrease after
// every test that calls cache.store(), and runTestSuiteShowMem would
// report a leak.
void after() {
  cache.clear();
}

void testStoreAndRetrieve(TestInvocation* t) {
  t->setName(F("Store and retrieve a string"));
  cache.store("hello");
  t->assertEqual(cache.retrieve(), "hello");
}

void testOverwrite(TestInvocation* t) {
  t->setName(F("Overwriting frees the previous allocation"));
  cache.store("first");
  cache.store("second");
  t->assertEqual(cache.retrieve(), "second");
}

void testClear(TestInvocation* t) {
  t->setName(F("Clear makes retrieve return empty string"));
  cache.store("something");
  cache.clear();
  t->assertEqual(cache.retrieve(), "");
}

void setup() {
  Serial.begin(9600);
  while (!Serial);

  TestFunction tests[] = {
    testStoreAndRetrieve,
    testOverwrite,
    testClear
  };

  // runTestSuiteShowMem prints free memory before and after each test.
  // Stable readings (same before and after) confirm that after() is
  // correctly freeing every allocation made during the test.
  //
  // To observe a leak in action, change the call below to:
  //   runTestSuiteShowMem(tests);
  // and notice that free memory decreases after each test that calls
  // cache.store(), because nothing frees the buffer between tests.
  runTestSuiteShowMem(tests, nullptr, after);
}

void loop() {}
