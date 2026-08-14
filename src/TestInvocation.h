#ifndef TESTTOOL_TESTINVOCATION_H
#define TESTTOOL_TESTINVOCATION_H


#include "hal/FlashStr.h"

#ifdef NO_ARDUINO
// On Arduino these come in transitively via Arduino.h. Off Arduino, nothing
// else pulls them in.
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#endif

class TestInvocation {

  public:
    TestInvocation(uint8_t testNum);
    ~TestInvocation();

    /*
     * Override the default "Test case N..." with a custom test
     * name up to 48 characters in length
     */
    void setName(const char* name);
    void setName(const FlashStr* name);

    const char* getName() const;
    uint8_t getNameLength();
    bool isNamePmem();

    /*
     * Verify that 'check' evaluates to true or fail. If the
     * TestInvocation has already failed, short circuit without
     * further checks
     */
    void verify(bool check, const char* message, bool allocate = false);
    void verify(bool check, const FlashStr* message);

    /*
     * Verify that two strings are equal.
     */
    void verifyEqual(const char* actual, const char* expected, const char* message = nullptr, bool allocate = false);
    void verifyEqual(const char* actual, const FlashStr* expected, const char* message = nullptr, bool allocate = false);
    void verifyEqual(const char* actual, const FlashStr* expected, const FlashStr* message);
    void verifyEqual(const char* actual, const char* expected, const FlashStr* message);
    void verifyEqual(const FlashStr* actual, const char* expected, const char* message = nullptr, bool allocate = false);
    void verifyEqual(const FlashStr* actual, const FlashStr* expected, const char* message = nullptr, bool allocate = false);
    void verifyEqual(const FlashStr* actual, const FlashStr* expected, const FlashStr* message);
    void verifyEqual(const FlashStr* actual, const char* expected, const FlashStr* message);

    /*
     * Force the TestInvocation to fail
     */
    void fail();
    void fail(const char* message, bool allocate = false);
    void fail(const FlashStr* message);

    bool passed();

    const char* getMessage() const;
    bool isMessagePmem();

  private:
    TestInvocation() = delete;
    TestInvocation(const TestInvocation&) = delete;
    TestInvocation& operator=(const TestInvocation&) = delete;
    bool flashStringEquals(const FlashStr* str1, const FlashStr* str2);
    char* toRAM(const FlashStr* str_P);
    char* defaultVerifyEqualsMessage(const char* actual, const char* expected);
    char* defaultVerifyEqualsMessage(const char* actual, const FlashStr* expected);
    char* defaultVerifyEqualsMessage(const FlashStr* actual, const char* expected);
    char* defaultVerifyEqualsMessage(const FlashStr* actual, const FlashStr* expected);
    void freeMessage();
    bool _success = true;
    const char* _testName = nullptr;
    const char* _message = nullptr;
    bool _ownsMessage = false;
    bool _isNamePmem = false;
    bool _isMessagePmem = false;

};


#endif
