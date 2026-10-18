#pragma once

#include <stdio.h>

#define ANSI_COLOR_RED   "\033[1;31m"
#define ANSI_COLOR_GREEN "\033[1;32m"
#define ANSI_COLOR_RESET "\033[0m"

#define Stringify(x) #x
#define ToString(x) Stringify(x)

#define ConcatImpl(x, y) x##y
#define Concat(x, y) ConcatImpl(x, y)

#define EXPECT_BOOL_BASE(condition, expected) \
    do { \
        bool _res = (condition); \
        if (_res != (expected)) { \
            printf(ANSI_COLOR_RED "[ FAILURE  ] %s:" ToString(__LINE__) ": Expected (%s) to be %s, actually it is %s!" ANSI_COLOR_RESET "\n", \
                   __func__, \
                   #condition, \
                   (expected) ? "true" : "false", \
                   (_res) ? "true" : "false"); \
        } \
    } while(0)

#define EXPECT_TRUE(condition)  EXPECT_BOOL_BASE(condition, true)
#define EXPECT_FALSE(condition) EXPECT_BOOL_BASE(condition, false)

class TestCase
{
public:
    TestCase(const char* name) : name_(name)
    {
        printf(ANSI_COLOR_GREEN "[ RUN      ] %s" ANSI_COLOR_RESET "\n", name_);
    }

    ~TestCase()
    {
        printf(ANSI_COLOR_GREEN "[     DONE ] %s" ANSI_COLOR_RESET "\n\n", name_);
    }

private:
    const char* name_{nullptr};
};

#define TEST_CASE(case_name)                                       \
    static void Concat(case_name, _body)(TestCase&);            \
    static void case_name(void) {                                  \
        TestCase Concat(test_case_, __LINE__)(#case_name);         \
        Concat(case_name, _body)(Concat(test_case_, __LINE__));    \
    }                                                              \
    static void Concat(case_name, _body)(TestCase&)
