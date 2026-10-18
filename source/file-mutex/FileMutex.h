#pragma once

namespace fm
{

enum class LockStatus
{
    OK,
    PathIsEmpty,
    AlreadyLocked,
    AlreadyReleased,
    FailedToCreateDirectory,
    FailedToCreateFile,
    RuntimeError,
};

LockStatus tryLock(const char* lock_path) noexcept;
LockStatus releaseLock(const char* lock_path) noexcept;

}
