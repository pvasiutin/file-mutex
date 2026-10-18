#include <thread>

#include "testing/Expectation.h"
#include "file-mutex/FileMutex.h"

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
