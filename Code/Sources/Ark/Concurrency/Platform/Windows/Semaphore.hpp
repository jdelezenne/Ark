#pragma once

#if defined(ARK_PLATFORM_WINDOWS)

#define WIN32_LEAN_AND_MEAN
#include "Ark/Core/Types.hpp"
#include <windows.h>

namespace Ark::Concurrency::Platform
{
    struct Semaphore
    {
    private:
        HANDLE handle = nullptr;

    public:
        explicit Semaphore(uint32 initialCount)
        {
            handle = CreateSemaphoreW(nullptr, static_cast<LONG>(initialCount), LONG_MAX, nullptr);
        }

        ~Semaphore()
        {
            if (handle)
                CloseHandle(handle);
        }

        void acquire()
        {
            WaitForSingleObject(handle, INFINITE);
        }

        bool tryAcquire()
        {
            return WaitForSingleObject(handle, 0) == WAIT_OBJECT_0;
        }

        bool tryAcquireFor(uint32 timeoutMs)
        {
            return WaitForSingleObject(handle, timeoutMs) == WAIT_OBJECT_0;
        }

        void release()
        {
            ReleaseSemaphore(handle, 1, nullptr);
        }

        void release(uint32 count)
        {
            ReleaseSemaphore(handle, static_cast<LONG>(count), nullptr);
        }
    };

    struct BinarySemaphore
    {
    private:
        HANDLE handle = nullptr;

    public:
        explicit BinarySemaphore(bool initiallyAcquired)
        {
            handle = CreateSemaphoreW(nullptr, initiallyAcquired ? 0 : 1, 1, nullptr);
        }

        ~BinarySemaphore()
        {
            if (handle)
                CloseHandle(handle);
        }

        void acquire()
        {
            WaitForSingleObject(handle, INFINITE);
        }

        bool tryAcquire()
        {
            return WaitForSingleObject(handle, 0) == WAIT_OBJECT_0;
        }

        bool tryAcquireFor(uint32 timeoutMs)
        {
            return WaitForSingleObject(handle, timeoutMs) == WAIT_OBJECT_0;
        }

        void release()
        {
            // Max count is 1; ignore failure when already released.
            ReleaseSemaphore(handle, 1, nullptr);
        }
    };
}

#endif
