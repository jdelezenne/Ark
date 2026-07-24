#pragma once

#include "Ark/Core/Platform.hpp"
#include "Ark/Core/Types.hpp"

#if defined(ARK_PLATFORM_WINDOWS)
#include "Ark/Concurrency/Platform/Windows/Mutex.hpp"
#elif defined(ARK_PLATFORM_POSIX)
#include "Ark/Concurrency/Platform/Posix/Mutex.hpp"
#else
#error "Ark::Concurrency::Mutex is not implemented for this platform"
#endif

namespace Ark::Concurrency
{
    /// Mutual exclusion primitive
    struct Mutex final : public Platform::Mutex
    {
    public:
        Mutex() = default;
        ~Mutex() = default;

        Mutex(Mutex const&) = delete;
        Mutex& operator=(Mutex const&) = delete;
        Mutex(Mutex&&) = delete;
        Mutex& operator=(Mutex&&) = delete;

        /// Lock the mutex (blocks until acquired)
        void lock()
        {
            Platform::Mutex::lock();
        }

        /// Try to lock the mutex without blocking
        /// @return True if lock was acquired, false otherwise
        bool tryLock()
        {
            return Platform::Mutex::tryLock();
        }

        /// Unlock the mutex
        void unlock()
        {
            Platform::Mutex::unlock();
        }
    };

    /// Recursive mutual exclusion primitive
    struct RecursiveMutex final : public Platform::RecursiveMutex
    {
    public:
        RecursiveMutex() = default;
        ~RecursiveMutex() = default;

        RecursiveMutex(RecursiveMutex const&) = delete;
        RecursiveMutex& operator=(RecursiveMutex const&) = delete;
        RecursiveMutex(RecursiveMutex&&) = delete;
        RecursiveMutex& operator=(RecursiveMutex&&) = delete;

        void lock()
        {
            Platform::RecursiveMutex::lock();
        }
        bool tryLock()
        {
            return Platform::RecursiveMutex::tryLock();
        }
        void unlock()
        {
            Platform::RecursiveMutex::unlock();
        }
    };

    /// Reader-writer lock for shared/exclusive access
    struct SharedMutex final : public Platform::SharedMutex
    {
    public:
        SharedMutex() = default;
        ~SharedMutex() = default;

        SharedMutex(SharedMutex const&) = delete;
        SharedMutex& operator=(SharedMutex const&) = delete;
        SharedMutex(SharedMutex&&) = delete;
        SharedMutex& operator=(SharedMutex&&) = delete;

        /// Acquire exclusive lock
        void lock()
        {
            Platform::SharedMutex::lock();
        }
        bool tryLock()
        {
            return Platform::SharedMutex::tryLock();
        }
        void unlock()
        {
            Platform::SharedMutex::unlock();
        }

        /// Acquire shared lock
        void lockShared()
        {
            Platform::SharedMutex::lockShared();
        }
        bool tryLockShared()
        {
            return Platform::SharedMutex::tryLockShared();
        }
        void unlockShared()
        {
            Platform::SharedMutex::unlockShared();
        }
    };
}
