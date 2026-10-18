#include "file-mutex/FileMutex.h"

namespace fm
{

LockStatus tryLock(const char* /*lock_path*/) noexcept
{
    return LockStatus::OK;
}

LockStatus releaseLock(const char* /*lock_path*/) noexcept
{
    return LockStatus::OK;
}

} // namespace fm

