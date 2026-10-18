#pragma once

#include <stdio.h>

#define ANSI_COLOR_RED   "\033[1;31m"
#define ANSI_COLOR_RESET "\033[0m"

#define EXPECT_BOOL_BASE(condition, expected) \
    do { \
        bool _res = (condition); \
        if (_res != (expected)) { \
            printf(ANSI_COLOR_RED "Failure: Expected (%s) to be %s, but it was %s!" ANSI_COLOR_RESET "\n", \
                   #condition, \
                   (expected) ? "true" : "false", \
                   (_res) ? "true" : "false"); \
        } \
    } while(0)

#define EXPECT_TRUE(condition)  EXPECT_BOOL_BASE(condition, true)
#define EXPECT_FALSE(condition) EXPECT_BOOL_BASE(condition, false)
