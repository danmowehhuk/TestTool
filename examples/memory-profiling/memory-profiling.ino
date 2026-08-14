#include <TestTool.h>

#include <stdlib.h>
#include <string.h>

// An append-only string cache: each store() call allocates a new buffer and
// keeps it, without freeing any earlier entry. clear() frees everything
// accumulated so far. This deliberately mirrors a common real-world leak
// shape - something new is allocated each iteration, and nothing frees the
// previous iteration's allocation until an explicit cleanup happens. That's
// exactly what after() is for.
class StringCache {
  static const uint8_t MAX_ENTRIES = 8;
  char* _entries[MAX_ENTRIES];
  uint8_t _count;

public:
  StringCache() : _count(0) {
    for (uint8_t i = 0; i < MAX_ENTRIES; i++) _entries[i] = nullptr;
  }
  ~StringCache() { clear(); }

  void store(const char* s) {
    if (_count >= MAX_ENTRIES) return;
    size_t len = strlen(s);
    _entries[_count] = (char*)malloc(len + 1);
    if (_entries[_count]) strcpy(_entries[_count], s);
    _count++;
  }

  const char* retrieve() const {
    return (_count > 0 && _entries[_count - 1]) ? _entries[_count - 1] : "";
  }

  void clear() {
    for (uint8_t i = 0; i < _count; i++) {
      free(_entries[i]);
      _entries[i] = nullptr;
    }
    _count = 0;
  }
};

// The fixture is a global so it is shared across all tests.
StringCache cache;

// after() runs after each test. It frees every allocation the test made so
// the post-test free-memory reading returns to the same baseline as before
// the test ran. Without this cleanup, free memory would keep decreasing
// after every test that calls cache.store(), and runTestSuiteShowMem would
// report a leak.
void after() {
  cache.clear();
}

void testStoreHello(TestInvocation* t) {
  t->setName(F("Store 'hello'"));
  cache.store("hello");
  t->verifyEqual(cache.retrieve(), "hello");
}

void testStoreWorld(TestInvocation* t) {
  t->setName(F("Store 'world'"));
  cache.store("world");
  t->verifyEqual(cache.retrieve(), "world");
}

void testStoreBang(TestInvocation* t) {
  t->setName(F("Store '!!!'"));
  cache.store("!!!");
  t->verifyEqual(cache.retrieve(), "!!!");
}

void testClearEmptiesTheCache(TestInvocation* t) {
  t->setName(F("clear() frees everything and empties the cache"));
  // Uses its own local instance so this test's cleanup doesn't mask the
  // shared cache's accumulation in the tests above, whether or not after()
  // is wired up below.
  StringCache local;
  local.store("temporary");
  local.clear();
  t->verifyEqual(local.retrieve(), "");
}

void setup() {
  Serial.begin(9600);
  while (!Serial);

  TestFunction tests[] = {
    testStoreHello,
    testStoreWorld,
    testStoreBang,
    testClearEmptiesTheCache
  };

  // runTestSuiteShowMem prints free memory before and after each test.
  // Stable readings (before == after) confirm that after() is correctly
  // freeing every allocation made during the test.
  //
  // To observe a leak in action, change the call below to:
  //   runTestSuiteShowMem(tests);
  // and notice that free memory keeps decreasing after each of the first
  // three tests - each store() call keeps its own allocation alive, and
  // with after() gone, nothing frees the ones from earlier tests either.
  runTestSuiteShowMem(tests, nullptr, after);
}

void loop() {}
