#include "TestInvocation.h"

TestInvocation::TestInvocation(uint8_t testNum) {
  static char defaultNameBuffer[32];
  const char* format = reinterpret_cast<const char*>(F("Test case %d"));
  snprintf_P(defaultNameBuffer, sizeof(defaultNameBuffer), format, testNum);
  _testName = defaultNameBuffer;
}

TestInvocation::~TestInvocation() {
  freeMessage();
}

void TestInvocation::setName(const char* name) {
  _testName = name;
  _isNamePmem = false;
}

void TestInvocation::setName(const FlashStr* name) {
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

void TestInvocation::freeMessage() {
  if (_message && _ownsMessage) free(const_cast<char*>(_message));
  _message = nullptr;
  _ownsMessage = false;
}

void TestInvocation::verify(bool check, const char* message, bool allocate) {
  if (!_success) return; // TestInvocation already failed
  freeMessage();
  if (check) return; // passing check - don't hold on to a message for a later bare fail()
  bool owns = allocate && message;
  _message = owns ? strdup(message) : message;
  _ownsMessage = owns;
  _isMessagePmem = false;
  fail();
}

void TestInvocation::verify(bool check, const FlashStr* message) {
  if (!_success) return; // TestInvocation already failed
  freeMessage();
  if (check) return; // passing check - don't hold on to a message for a later bare fail()
  _message = reinterpret_cast<const char*>(message);
  _isMessagePmem = true;
  fail();
}

void TestInvocation::verifyEqual(const char* actual, const char* expected, const char* message, bool allocate) {
  if (!_success) return; // TestInvocation already failed
  bool eq = (actual == expected) || (actual && expected && strcmp(actual, expected) == 0);
  if (!eq && !message) {
    // defaultVerifyEqualsMessage returns a pointer to a reused static buffer -
    // force allocate so verify() copies it out before anything else can
    // overwrite it.
    message = defaultVerifyEqualsMessage(actual, expected);
    allocate = true;
  }
  verify(eq, message, allocate);
}

void TestInvocation::verifyEqual(const char* actual, const FlashStr* expected, const char* message, bool allocate) {
  if (!_success) return; // TestInvocation already failed
  bool eq = actual && strcmp_P(actual, reinterpret_cast<PGM_P>(expected)) == 0;
  if (!eq && !message) {
    message = defaultVerifyEqualsMessage(actual, expected);
    allocate = true;
  }
  verify(eq, message, allocate);
}

void TestInvocation::verifyEqual(const char* actual, const FlashStr* expected, const FlashStr* message) {
  if (!_success) return; // TestInvocation already failed
  bool eq = actual && strcmp_P(actual, reinterpret_cast<PGM_P>(expected)) == 0;
  verify(eq, message);
}

void TestInvocation::verifyEqual(const char* actual, const char* expected, const FlashStr* message) {
  if (!_success) return; // TestInvocation already failed
  bool eq = (actual == expected) || (actual && expected && strcmp(actual, expected) == 0);
  verify(eq, message);
}

void TestInvocation::verifyEqual(const FlashStr* actual, const char* expected, const char* message, bool allocate) {
  if (!_success) return; // TestInvocation already failed
  bool eq = expected && strcmp_P(expected, reinterpret_cast<PGM_P>(actual)) == 0;
  if (!eq && !message) {
    message = defaultVerifyEqualsMessage(actual, expected);
    allocate = true;
  }
  verify(eq, message, allocate);
}

void TestInvocation::verifyEqual(const FlashStr* actual, const FlashStr* expected, const char* message, bool allocate) {
  if (!_success) return; // TestInvocation already failed
  bool eq = flashStringEquals(actual, expected);
  if (!eq && !message) {
    message = defaultVerifyEqualsMessage(actual, expected);
    allocate = true;
  }
  verify(eq, message, allocate);
}

void TestInvocation::verifyEqual(const FlashStr* actual, const FlashStr* expected, const FlashStr* message) {
  if (!_success) return; // TestInvocation already failed
  verify(flashStringEquals(actual, expected), message);
}

void TestInvocation::verifyEqual(const FlashStr* actual, const char* expected, const FlashStr* message) {
  if (!_success) return; // TestInvocation already failed
  bool eq = expected && strcmp_P(expected, reinterpret_cast<PGM_P>(actual)) == 0;
  verify(eq, message);
}

char* TestInvocation::defaultVerifyEqualsMessage(const char* actual, const char* expected) {
  char* prefix = toRAM(F("Expected '"));
  char* middle = toRAM(F("' but got '"));
  char* end = toRAM(F("'"));
  static char message[64];
  const char* format = reinterpret_cast<const char*>(F("%s%s%s%s%s"));
  snprintf_P(message, sizeof(message), format, prefix, expected, middle, actual, end);
  free(prefix);
  free(middle);
  free(end);
  return message;
}

char* TestInvocation::defaultVerifyEqualsMessage(const char* actual, const FlashStr* expected) {
  char* expectedRAM = toRAM(expected);
  char* message = defaultVerifyEqualsMessage(actual, expectedRAM);
  free(expectedRAM);
  return message;
}

char* TestInvocation::defaultVerifyEqualsMessage(const FlashStr* actual, const char* expected) {
  char* actualRAM = toRAM(actual);
  char* message = defaultVerifyEqualsMessage(actualRAM, expected);
  free(actualRAM);
  return message;
}

char* TestInvocation::defaultVerifyEqualsMessage(const FlashStr* actual, const FlashStr* expected) {
  char* actualRAM = toRAM(actual);
  char* expectedRAM = toRAM(expected);
  char* message = defaultVerifyEqualsMessage(actualRAM, expectedRAM);
  free(actualRAM);
  free(expectedRAM);
  return message;
}

bool TestInvocation::flashStringEquals(const FlashStr* str1, const FlashStr* str2) {
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

char* TestInvocation::toRAM(const FlashStr* str_P) {
  if (!str_P) return nullptr;
  size_t len = strlen_P(reinterpret_cast<const char*>(str_P));
  char* str = static_cast<char*>(malloc(len + 1));
  if (!str) return nullptr; // -fno-exceptions: allocation failure returns null, not a thrown exception
  strncpy_P(str, reinterpret_cast<const char*>(str_P), len);
  str[len] = '\0';
  return str;
}

void TestInvocation::fail() {
  _success = false;
}

void TestInvocation::fail(const char* message, bool allocate) {
  freeMessage();
  bool owns = allocate && message;
  _message = owns ? strdup(message) : message;
  _ownsMessage = owns;
  _isMessagePmem = false;
  fail();
}

void TestInvocation::fail(const FlashStr* message) {
  freeMessage();
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
