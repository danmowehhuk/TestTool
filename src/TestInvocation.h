#ifndef TESTTOOL_TESTINVOCATION_H
#define TESTTOOL_TESTINVOCATION_H


#include <Arduino.h>

class TestInvocation {

  public:
    TestInvocation(uint8_t testNum);
    ~TestInvocation();

    /*
     * Override the default "Test case N..." with a custom test
     * name up to 48 characters in length
     */
    void setName(const char* name);
    void setName(const __FlashStringHelper* name);

    const char* getName() const;
    uint8_t getNameLength();
    bool isNamePmem();

    /*
     * Verify that 'check' evaluates to true or fail. If the
     * TestInvocation has already failed, short circuit without
     * further checks
     */
    void verify(bool check, const char* message, bool allocate = false);
    void verify(bool check, const __FlashStringHelper* message);

    /*
     * Verify that two strings are equal.
     */
    void verifyEqual(const char* actual, const char* expected, const char* message = nullptr, bool allocate = false);
    void verifyEqual(const char* actual, const __FlashStringHelper* expected, const char* message = nullptr, bool allocate = false);
    void verifyEqual(const char* actual, const __FlashStringHelper* expected, const __FlashStringHelper* message);
    void verifyEqual(const char* actual, const char* expected, const __FlashStringHelper* message);
    void verifyEqual(const __FlashStringHelper* actual, const char* expected, const char* message = nullptr, bool allocate = false);
    void verifyEqual(const __FlashStringHelper* actual, const __FlashStringHelper* expected, const char* message = nullptr, bool allocate = false);
    void verifyEqual(const __FlashStringHelper* actual, const __FlashStringHelper* expected, const __FlashStringHelper* message);
    void verifyEqual(const __FlashStringHelper* actual, const char* expected, const __FlashStringHelper* message);

    /*
     * Force the TestInvocation to fail
     */
    void fail();
    void fail(const char* message, bool allocate = false);
    void fail(const __FlashStringHelper* message);

    bool passed();

    const char* getMessage() const;
    bool isMessagePmem();

  private:
    TestInvocation() = delete;
    TestInvocation(const TestInvocation&) = delete;
    TestInvocation& operator=(const TestInvocation&) = delete;
    bool flashStringEquals(const __FlashStringHelper* str1, const __FlashStringHelper* str2);
    char* toRAM(const __FlashStringHelper* str_P);
    char* defaultVerifyEqualsMessage(const char* actual, const char* expected);
    char* defaultVerifyEqualsMessage(const char* actual, const __FlashStringHelper* expected);
    char* defaultVerifyEqualsMessage(const __FlashStringHelper* actual, const char* expected);
    char* defaultVerifyEqualsMessage(const __FlashStringHelper* actual, const __FlashStringHelper* expected);
    void freeMessage();
    bool _success = true;
    const char* _testName = nullptr;
    const char* _message = nullptr;
    bool _ownsMessage = false;
    bool _isNamePmem = false;
    bool _isMessagePmem = false;

};


#endif
