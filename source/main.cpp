#include <thread>
#include <string>
#include <filesystem>

#include "testing/Expectation.h"
#include "file-mutex/FileMutex.h"

inline std::string getLockPath(const char* test_name)
{
    return (std::filesystem::current_path() / "Locks" / test_name).string();
}

TEST_CASE(RejectsNullArgumentsTest)
{
    EXPECT_TRUE(fm::tryLock(nullptr) == fm::LockStatus::PathIsEmpty);
    EXPECT_TRUE(fm::releaseLock(nullptr) == fm::LockStatus::PathIsEmpty);
}

TEST_CASE(PathResolveTest)
{
    const auto lock_path = getLockPath("ResolveMe.lock");
    const auto indirect_lock_path = (std::filesystem::path{lock_path}.parent_path() / "." / std::filesystem::path{lock_path}.filename()).string();
    EXPECT_TRUE(fm::tryLock(lock_path.c_str()) == fm::LockStatus::OK);
    EXPECT_TRUE(fm::tryLock(indirect_lock_path.c_str()) == fm::LockStatus::AlreadyLocked);
}

int main()
{
    RejectsNullArgumentsTest();
    PathResolveTest();

    return 0;
}
