#pragma once

#if defined(ARK_PLATFORM_WINDOWS)

#include "Ark/Core/Platform/Windows/Platform.hpp"

namespace Ark::Concurrency::Platform
{
    /// Non-recursive mutual exclusion (SRWLOCK exclusive mode).
    struct Mutex
    {
    private:
        SRWLOCK srwLock = SRWLOCK_INIT;

    public:
        Mutex() = default;
        ~Mutex() = default;

        void lock()
        {
            AcquireSRWLockExclusive(&srwLock);
        }

        bool tryLock()
        {
            return TryAcquireSRWLockExclusive(&srwLock) != 0;
        }

        void unlock()
        {
            ReleaseSRWLockExclusive(&srwLock);
        }

        SRWLOCK* nativeHandle()
        {
            return &srwLock;
        }
    };

    struct RecursiveMutex
    {
    private:
        CRITICAL_SECTION cs;

    public:
        RecursiveMutex()
        {
            InitializeCriticalSection(&cs);
        }

        ~RecursiveMutex()
        {
            DeleteCriticalSection(&cs);
        }

        void lock()
        {
            EnterCriticalSection(&cs);
        }

        bool tryLock()
        {
            return TryEnterCriticalSection(&cs) != 0;
        }

        void unlock()
        {
            LeaveCriticalSection(&cs);
        }
    };

    struct SharedMutex
    {
    private:
        SRWLOCK srwLock = SRWLOCK_INIT;

    public:
        SharedMutex() = default;
        ~SharedMutex() = default;

        void lock()
        {
            AcquireSRWLockExclusive(&srwLock);
        }

        bool tryLock()
        {
            return TryAcquireSRWLockExclusive(&srwLock) != 0;
        }

        void unlock()
        {
            ReleaseSRWLockExclusive(&srwLock);
        }

        void lockShared()
        {
            AcquireSRWLockShared(&srwLock);
        }

        bool tryLockShared()
        {
            return TryAcquireSRWLockShared(&srwLock) != 0;
        }

        void unlockShared()
        {
            ReleaseSRWLockShared(&srwLock);
        }
    };
}

#endif
