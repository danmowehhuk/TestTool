#include "TestInvocation.h"
#include <stdlib.h>
#include <string.h>

TestInvocation::TestInvocation(uint8_t testNum) {
  static char defaultNameBuffer[32];
  const char* format = reinterpret_cast<const char*>(TESTTOOL_HAL_FLASH_STR("Test case %d"));
  snprintf_P(defaultNameBuffer, sizeof(defaultNameBuffer), format, testNum);
  _testName = defaultNameBuffer;
}

TestInvocation::~TestInvocation() {
  freeMessage();
  _message = nullptr;
}

void TestInvocation::freeMessage() {
  // _ownsMessage is only ever set true when _message was allocated via
  // malloc/strdup (see verify/fail overloads with allocate=true), so this
  // const_cast is the one place in the class where freeing through the
  // const-qualified _message pointer is actually safe.
  if (_message && _ownsMessage) {
    free(const_cast<char*>(_message));
  }
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

bool TestInvocation::verify(bool check, const char* message, bool allocate) {
  if (!_success) return false; // TestInvocation already failed
  if (!check) {
    freeMessage();
    _message = allocate ? strdup(message) : message;
    _ownsMessage = allocate;
    _isMessagePmem = false;
    fail();
  }
  return check;
}

bool TestInvocation::verify(bool check, const __FlashStringHelper* message) {
  if (!_success) return false; // TestInvocation already failed
  if (!check) {
    freeMessage();
    _message = reinterpret_cast<const char*>(message);
    _ownsMessage = false;
    _isMessagePmem = true;
    fail();
  }
  return check;
}

bool TestInvocation::ramStringsEqual(const char* a, const char* b) {
  // Both null is treated as equal; exactly one null is never equal to a
  // real string. Avoids undefined behavior from strcmp(nullptr, ...).
  if (!a || !b) return a == b;
  return strcmp(a, b) == 0;
}

bool TestInvocation::ramFlashEqual(const char* ramStr, const __FlashStringHelper* flashStr) {
  if (!ramStr || !flashStr) return false;
  return strcmp_P(ramStr, reinterpret_cast<PGM_P>(flashStr)) == 0;
}

bool TestInvocation::verifyEqual(const char* actual, const char* expected, const char* message, bool allocate) {
  if (!_success) return false; // TestInvocation already failed
  bool eq = ramStringsEqual(actual, expected);
  if (!eq && !message) message = defaultVerifyEqualsMessage(actual, expected);
  return verify(eq, message, allocate);
}

bool TestInvocation::verifyEqual(const char* actual, const __FlashStringHelper* expected, const char* message, bool allocate) {
  if (!_success) return false; // TestInvocation already failed
  bool eq = ramFlashEqual(actual, expected);
  if (!eq && !message) message = defaultVerifyEqualsMessage(actual, expected);
  return verify(eq, message, allocate);
}

bool TestInvocation::verifyEqual(const char* actual, const __FlashStringHelper* expected, const __FlashStringHelper* message) {
  if (!_success) return false; // TestInvocation already failed
  return verify(ramFlashEqual(actual, expected), message);
}

bool TestInvocation::verifyEqual(const char* actual, const char* expected, const __FlashStringHelper* message) {
  if (!_success) return false; // TestInvocation already failed
  return verify(ramStringsEqual(actual, expected), message);
}

bool TestInvocation::verifyEqual(const __FlashStringHelper* actual, const char* expected, const char* message, bool allocate) {
  if (!_success) return false; // TestInvocation already failed
  bool eq = ramFlashEqual(expected, actual);
  if (!eq && !message) message = defaultVerifyEqualsMessage(actual, expected);
  return verify(eq, message, allocate);
}

bool TestInvocation::verifyEqual(const __FlashStringHelper* actual, const __FlashStringHelper* expected, const char* message, bool allocate) {
  if (!_success) return false; // TestInvocation already failed
  bool eq = flashStringEquals(actual, expected);
  if (!eq && !message) message = defaultVerifyEqualsMessage(actual, expected);
  return verify(eq, message, allocate);
}

bool TestInvocation::verifyEqual(const __FlashStringHelper* actual, const __FlashStringHelper* expected, const __FlashStringHelper* message) {
  if (!_success) return false; // TestInvocation already failed
  return verify(flashStringEquals(actual, expected), message);
}

bool TestInvocation::verifyEqual(const __FlashStringHelper* actual, const char* expected, const __FlashStringHelper* message) {
  if (!_success) return false; // TestInvocation already failed
  return verify(ramFlashEqual(expected, actual), message);
}

char* TestInvocation::defaultVerifyEqualsMessage(const char* actual, const char* expected) {
  static char message[64];
  const char* format = reinterpret_cast<const char*>(TESTTOOL_HAL_FLASH_STR("Expected '%s' but got '%s'"));
  snprintf_P(message, sizeof(message), format, expected ? expected : "<null>", actual ? actual : "<null>");
  return message;
}

char* TestInvocation::defaultVerifyEqualsMessage(const char* actual, const __FlashStringHelper* expected) {
  char* expectedRAM = toRAM(expected);
  char* message = defaultVerifyEqualsMessage(actual, expectedRAM);
  free(expectedRAM);
  return message;
}

char* TestInvocation::defaultVerifyEqualsMessage(const __FlashStringHelper* actual, const char* expected) {
  char* actualRAM = toRAM(actual);
  char* message = defaultVerifyEqualsMessage(actualRAM, expected);
  free(actualRAM);
  return message;
}

char* TestInvocation::defaultVerifyEqualsMessage(const __FlashStringHelper* actual, const __FlashStringHelper* expected) {
  char* actualRAM = toRAM(actual);
  char* expectedRAM = toRAM(expected);
  char* message = defaultVerifyEqualsMessage(actualRAM, expectedRAM);
  free(actualRAM);
  free(expectedRAM);
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
  char* str = static_cast<char*>(malloc(len + 1));
  if (!str) return nullptr;
  strncpy_P(str, reinterpret_cast<const char*>(str_P), len);
  str[len] = '\0';
  return str;
}

void TestInvocation::fail() {
  _success = false;
}

void TestInvocation::fail(const char* message, bool allocate) {
  freeMessage();
  _message = allocate ? strdup(message) : message;
  _ownsMessage = allocate;
  _isMessagePmem = false;
  fail();
}

void TestInvocation::fail(const __FlashStringHelper* message) {
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
