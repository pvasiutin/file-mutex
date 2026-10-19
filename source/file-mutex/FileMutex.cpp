#include "file-mutex/FileMutex.h"

#ifdef _WIN32
    #include <windows.h>
#else
#endif

#include <stdio.h>

#include <map>
#include <mutex>
#include <filesystem>
#include <system_error>

namespace fm
{

namespace fs = std::filesystem;

namespace 
{

#ifdef _WIN32
class UniqueHandle
{
public:

    UniqueHandle() = default;
    explicit UniqueHandle(HANDLE h) : handle_(h)
    {}
    UniqueHandle(UniqueHandle&& other) noexcept : handle_(std::exchange(other.handle_, nullptr))
    {}
    UniqueHandle(const UniqueHandle&) = delete;
    UniqueHandle& operator=(const UniqueHandle&) = delete;

    ~UniqueHandle()
    {
        if (isClosable())
        {
            ::CloseHandle(handle_);
        }
        handle_ = nullptr;
    }

    UniqueHandle& operator=(UniqueHandle&& other) noexcept
    {
        if (this != &other)
        {
            if (isClosable())
            {
                ::CloseHandle(handle_);
            }
            handle_ = std::exchange(other.handle_, nullptr);
        }
        return *this;
    }

    explicit operator bool() const noexcept
    {
        return isClosable();
    }

    HANDLE get() const noexcept
    {
        return handle_;
    }

private:
    bool isClosable() const noexcept
    {
        return handle_ != nullptr && handle_ != INVALID_HANDLE_VALUE;
    }

private:
    HANDLE handle_{nullptr};
};

OVERLAPPED lockedByte()
{
    // Lock a single beyond the end of a file instead of the whole file, some
    // algorithms may want to read contents of the file while it's locked
    constexpr DWORD LockedByteOffsetHigh = 0x40000000;

    OVERLAPPED overlapped{};
    overlapped.OffsetHigh = LockedByteOffsetHigh;
    return overlapped;
}
#else
using UniqueHandle = int;
constexpr int NoLockFile = -1;
#endif

std::mutex RegistryLockMutex;
std::map<std::string, UniqueHandle, std::less<>> HeldRegistryLocks;

fs::path toAbsolute(const char* path)
{
    return fs::absolute(fs::path{path}).lexically_normal();
}

bool isLocked(const UniqueHandle& lock_file)
{
#ifdef _WIN32
    return static_cast<bool>(lock_file);
#else
    return lock_file != NoLockFile;
#endif
}

UniqueHandle lockFile(const fs::path& path)
{
#ifdef _WIN32
    UniqueHandle file{::CreateFileW(path.wstring().c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr)};
    if (!file)
    {
        return {};
    }

    auto overlapped = lockedByte();
    if (!::LockFileEx(file.get(), LOCKFILE_EXCLUSIVE_LOCK | LOCKFILE_FAIL_IMMEDIATELY, 0, 1, 0, &overlapped))
    {
        if (const auto error = ::GetLastError(); error != ERROR_LOCK_VIOLATION)
        {
            printf("Failed to lock file '%s': error %ull", path.string().c_str(), error);
        }
        return {};
    }

    return file;
#else
    return NoLockFile;
#endif
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
    const auto key = absolute_path.string();

    const std::lock_guard lock(RegistryLockMutex);
    if (HeldRegistryLocks.find(key) != HeldRegistryLocks.end())
    {
        return LockStatus::AlreadyLocked;
    }

    std::error_code ec;
    fs::create_directories(absolute_path.parent_path(), ec);
    if (ec)
    {
        return LockStatus::FailedToCreateDirectory;
    }

    auto file_lock = lockFile(absolute_path);
    if (!isLocked(file_lock))
    {
        return LockStatus::AlreadyLocked;
    }

    HeldRegistryLocks.emplace(std::move(key), std::move(file_lock));
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

