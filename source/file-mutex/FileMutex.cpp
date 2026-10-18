#include "file-mutex/FileMutex.h"

#include <filesystem>
#include <system_error>

namespace fm
{

namespace fs = std::filesystem;

namespace 
{

fs::path toAbsolute(const char* path)
{
    return fs::absolute(fs::path{path}).lexically_normal();
}

}

LockStatus tryLock(const char* lock_path) noexcept
try
{
    if (!lock_path)
    {
        return LockStatus::PathIsEmpty;
    }

    const auto absolute_path = toAbsolute(lock_path);

    std::error_code ec;
    fs::create_directories(absolute_path.parent_path(), ec);
    if (ec)
    {
        return LockStatus::FailedToCreateDirectory;
    }

    return LockStatus::OK;
}
catch (const std::exception&)
{
    return LockStatus::RuntimeError;
}

LockStatus releaseLock(const char* lock_path) noexcept
try
{
    if (!lock_path)
    {
        return LockStatus::PathIsEmpty;
    }
    return LockStatus::OK;
}
catch (const std::exception&)
{
    return LockStatus::RuntimeError;
}

} // namespace fm

