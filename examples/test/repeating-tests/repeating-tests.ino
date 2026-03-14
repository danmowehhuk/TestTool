#include <TestTool.h>

#include <stdlib.h>
#include <string.h>

// A token list that parses a comma-separated string into individual tokens.
// It allocates a copy of the input on the heap and frees it in clear().
class TokenList {
  char* _buf;
  char* _tokens[8];
  uint8_t _count;

public:
  TokenList() : _buf(nullptr), _count(0) {
    memset(_tokens, 0, sizeof(_tokens));
  }
  ~TokenList() { clear(); }

  bool parse(const char* csv) {
    clear();
    _buf = strdup(csv);
    if (!_buf) return false;
    char* p = _buf;
    while (*p && _count < 8) {
      _tokens[_count++] = p;
      while (*p && *p != ',') p++;
      if (*p == ',') *p++ = '\0';
    }
    return true;
  }

  uint8_t count() const { return _count; }
  const char* get(uint8_t i) const { return (i < _count) ? _tokens[i] : nullptr; }

  void clear() {
    if (_buf) { free(_buf); _buf = nullptr; }
    _count = 0;
    memset(_tokens, 0, sizeof(_tokens));
  }
};

TokenList tokens;

void after() {
  tokens.clear();
}

void testParseSingle(TestInvocation* t) {
  t->setName(F("Parse single token"));
  tokens.parse("alpha");
  t->assert(tokens.count() == 1, F("Should have 1 token"));
  t->assertEqual(tokens.get(0), "alpha");
}

void testParseMultiple(TestInvocation* t) {
  t->setName(F("Parse three tokens"));
  tokens.parse("one,two,three");
  t->assert(tokens.count() == 3, F("Should have 3 tokens"));
  t->assertEqual(tokens.get(0), "one");
  t->assertEqual(tokens.get(1), "two");
  t->assertEqual(tokens.get(2), "three");
}

void testEmptyString(TestInvocation* t) {
  t->setName(F("Empty string yields zero tokens"));
  tokens.parse("");
  t->assert(tokens.count() == 0, F("Should have 0 tokens"));
}

void setup() {
  Serial.begin(9600);
  while (!Serial);

  TestFunction tests[] = {
    testParseSingle,
    testParseMultiple,
    testEmptyString
  };

  // Running each test 5 times serves two purposes:
  //   1. Amplifies memory leaks: a 2-byte leak per run becomes 10 bytes
  //      after 5 runs, making it clearly visible in the memory readings.
  //   2. Guards against non-deterministic behavior that might only surface
  //      on repeated invocations.
  //
  // after() calls tokens.clear() to free the heap buffer between runs.
  // Stable memory readings across all 5 repetitions of each test confirm
  // that cleanup is correct.
  runTestSuiteShowMem(tests, nullptr, after, 5);
}

void loop() {}
