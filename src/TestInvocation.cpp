#include "TestInvocation.h"

TestInvocation::TestInvocation(uint8_t testNum) {
  static char defaultNameBuffer[32];
  char* format = reinterpret_cast<const char*>(F("Test case %d"));
  snprintf_P(defaultNameBuffer, sizeof(defaultNameBuffer), format, testNum);
  _testName = defaultNameBuffer;
}

TestInvocation::~TestInvocation() {
  if (_message && _ownsMessage) {
    free(_message);
  }
  _message = nullptr;
}

void TestInvocation::setName(const char* name) {
  _testName = name;
  _isNamePmem = false;
}

void TestInvocation::setName(const __FlashStringHelper* name) {
  _testName = reinterpret_cast<const char*>(name);
  _isNamePmem = true;
}

bool TestInvocation::isNamePmem() {
  return _isNamePmem;
}

uint8_t TestInvocation::getNameLength() {
  if (_testName) {
    if (isNamePmem()) {
      return strlen_P(_testName);
    } else {
      return strlen(_testName);
    }
  }
  return 0;
}

const char* TestInvocation::getName() const {
  return _testName;
}

void TestInvocation::assert(bool check, const char* message, bool allocate = false) {
  if (!_success) return; // TestInvocation already failed
  if (_message && _ownsMessage) free(_message);
  _message = message;
  _ownsMessage = allocate;
  _isMessagePmem = false;
  if (!check) fail();
}

void TestInvocation::assert(bool check, const __FlashStringHelper* message) {
  if (!_success) return; // TestInvocation already failed
  if (_message && _ownsMessage) free(_message);
  _message = reinterpret_cast<const char*>(message);
  _ownsMessage = false;
  _isMessagePmem = true;
  if (!check) fail();
}

void TestInvocation::assertEqual(const char* actual, const char* expected, const char* message = nullptr, bool allocate = false) {
  if (!_success) return; // TestInvocation already failed
  message = message ? message : defaultAssertEqualsMessage(actual, expected);
  assert(strcmp(actual, expected) == 0, message, allocate);
}

void TestInvocation::assertEqual(const char* actual, const __FlashStringHelper* expected, const char* message = nullptr, bool allocate = false) {
  if (!_success) return; // TestInvocation already failed
  message = message ? message : defaultAssertEqualsMessage(actual, expected);
  assert(strcmp_P(actual, (PGM_P)expected) == 0, message, allocate);
}

void TestInvocation::assertEqual(const char* actual, const __FlashStringHelper* expected, __FlashStringHelper* message) {
  if (!_success) return; // TestInvocation already failed
  assert(strcmp_P(actual, (PGM_P)expected) == 0, message);
}

void TestInvocation::assertEqual(const char* actual, const char* expected, const __FlashStringHelper* message) {
  if (!_success) return; // TestInvocation already failed
  assert(strcmp(actual, expected) == 0, message);
}

void TestInvocation::assertEqual(const __FlashStringHelper* actual, const char* expected, const char* message = nullptr, bool allocate = false) {
  if (!_success) return; // TestInvocation already failed
  message = message ? message : defaultAssertEqualsMessage(actual, expected);
  assert(strcmp_P(expected, (PGM_P)actual) == 0, message, allocate);
}

void TestInvocation::assertEqual(const __FlashStringHelper* actual, const __FlashStringHelper* expected, const char* message = nullptr, bool allocate = false) {
  if (!_success) return; // TestInvocation already failed
  message = message ? message : defaultAssertEqualsMessage(actual, expected);
  assert(flashStringEquals(actual, expected), message, allocate);
}

void TestInvocation::assertEqual(const __FlashStringHelper* actual, const __FlashStringHelper* expected, __FlashStringHelper* message) {
  if (!_success) return; // TestInvocation already failed
  assert(flashStringEquals(actual, expected), message);
}

void TestInvocation::assertEqual(const __FlashStringHelper* actual, const char* expected, const __FlashStringHelper* message) {
  assert(strcmp_P(expected, (PGM_P)actual) == 0, message);
  if (!_success) return; // TestInvocation already failed
}

char* TestInvocation::defaultAssertEqualsMessage(const char* actual, const char* expected) {
  const char* prefix = toRAM(F("Expected '"));
  const char* middle = toRAM(F("' but got '"));
  const char* end = toRAM(F("'"));
  static char message[64];
  char* format = reinterpret_cast<const char*>(F("%s%s%s%s%s"));
  snprintf_P(message, sizeof(message), format, prefix, expected, middle, actual, end);
  delete[] prefix;
  delete[] middle;
  delete[] end;
  return message;
}

char* TestInvocation::defaultAssertEqualsMessage(const char* actual, const __FlashStringHelper* expected) {
  char* expectedRAM = toRAM(expected);
  char* message = defaultAssertEqualsMessage(actual, expectedRAM);
  delete[] expectedRAM;
  return message;
}

char* TestInvocation::defaultAssertEqualsMessage(const __FlashStringHelper* actual, const char* expected) {
  char* actualRAM = toRAM(actual);
  char* message = defaultAssertEqualsMessage(actualRAM, expected);
  delete[] actualRAM;
  return message;
}

char* TestInvocation::defaultAssertEqualsMessage(const __FlashStringHelper* actual, const __FlashStringHelper* expected) {
  char* actualRAM = toRAM(actual);
  char* expectedRAM = toRAM(expected);
  char* message = defaultAssertEqualsMessage(actualRAM, expectedRAM);
  delete[] actualRAM;
  delete[] expectedRAM;
  return message;
}

bool TestInvocation::flashStringEquals(const __FlashStringHelper* str1, const __FlashStringHelper* str2) {
  if (str1 == str2) return true;
  if (!str1 || !str2) return false;

  const char* p1 = reinterpret_cast<const char*>(str1);
  const char* p2 = reinterpret_cast<const char*>(str2);

  while (true) {
    char c1 = pgm_read_byte(p1++);
    char c2 = pgm_read_byte(p2++);

    if (c1 != c2) return false;
    if (c1 == '\0') return true;
  }
}

char* TestInvocation::toRAM(const __FlashStringHelper* str_P) {
  if (!str_P) return nullptr;
  size_t len = strlen_P(reinterpret_cast<const char*>(str_P));
  char* str = new char[len + 1];
  strncpy_P(str, reinterpret_cast<const char*>(str_P), len);
  str[len] = '\0'; 
  return str;
}

void TestInvocation::fail() {
  _success = false;  
}

void TestInvocation::fail(const char* message, bool allocate = false) {
  if (_message && _ownsMessage) free(_message);
  _message = allocate ? strdup(message) : message;
  _ownsMessage = allocate;
  _isMessagePmem = false;
  fail();
}

void TestInvocation::fail(const __FlashStringHelper* message) {
  if (_message && _ownsMessage) free(_message);
  _message = reinterpret_cast<const char*>(message);
  _isMessagePmem = true;
  _ownsMessage = false;
  fail();
}

bool TestInvocation::passed() {
  return _success;
}

const char* TestInvocation::getMessage() const {
  return _message;
}

bool TestInvocation::isMessagePmem() {
  return _isMessagePmem;
}
