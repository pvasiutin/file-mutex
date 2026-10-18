#include <stdio.h>

#include <thread>

#include "file-mutex/FileMutex.h"

#define EXPECT_BOOL_BASE(condition, expected) \
    do { \
        bool _res = (condition); \
        if (_res != (expected)) { \
            printf("Failure: Expected (%s) to be %s, but it was %s!\n", \
                   #condition, \
                   (expected) ? "true" : "false", \
                   (_res) ? "true" : "false"); \
        } \
    } while(0)

#define EXPECT_TRUE(condition)  EXPECT_BOOL_BASE(condition, true)
#define EXPECT_FALSE(condition) EXPECT_BOOL_BASE(condition, false)

void testRejectsNullArguments()
{
    EXPECT_TRUE(fm::tryLock(nullptr) == fm::LockStatus::PathIsEmpty);
    EXPECT_TRUE(fm::releaseLock(nullptr) == fm::LockStatus::PathIsEmpty);
}

int main()
{
    testRejectsNullArguments();
    return 0;
}
