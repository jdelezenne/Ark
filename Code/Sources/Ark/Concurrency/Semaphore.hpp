#pragma once

#include "Ark/Core/Platform.hpp"
#include "Ark/Core/Types.hpp"

#if defined(ARK_PLATFORM_WINDOWS)
#include "Ark/Concurrency/Platform/Windows/Semaphore.hpp"
#elif defined(ARK_PLATFORM_POSIX)
#include "Ark/Concurrency/Platform/Posix/Semaphore.hpp"
#else
#error "Ark::Concurrency::Semaphore is not implemented for this platform"
#endif

namespace Ark::Concurrency
{
    /// Counting semaphore
    struct Semaphore final : public Platform::Semaphore
    {
    public:
        /// Create a semaphore with initial count
        /// @param initialCount Initial number of resources
        explicit Semaphore(uint32 initialCount = 0)
            : Platform::Semaphore(initialCount)
        {
        }

        ~Semaphore() = default;

        Semaphore(Semaphore const&) = delete;
        Semaphore& operator=(Semaphore const&) = delete;
        Semaphore(Semaphore&&) = delete;
        Semaphore& operator=(Semaphore&&) = delete;

        /// Acquire a resource (blocks if none available)
        void acquire()
        {
            Platform::Semaphore::acquire();
        }

        /// Try to acquire a resource without blocking
        /// @return True if resource was acquired, false otherwise
        bool tryAcquire()
        {
            return Platform::Semaphore::tryAcquire();
        }

        /// Try to acquire a resource with timeout
        /// @param timeoutMs Timeout in milliseconds
        /// @return True if resource was acquired, false if timeout
        bool tryAcquireFor(uint32 timeoutMs)
        {
            return Platform::Semaphore::tryAcquireFor(timeoutMs);
        }

        /// Release a resource
        void release()
        {
            Platform::Semaphore::release();
        }

        /// Release multiple resources
        /// @param count Number of resources to release
        void release(uint32 count)
        {
            Platform::Semaphore::release(count);
        }
    };

    /// Binary semaphore (mutex alternative)
    struct BinarySemaphore final : public Platform::BinarySemaphore
    {
    public:
        /// Create binary semaphore
        /// @param initiallyAcquired If true, semaphore starts in acquired state
        explicit BinarySemaphore(bool initiallyAcquired = false)
            : Platform::BinarySemaphore(initiallyAcquired)
        {
        }

        ~BinarySemaphore() = default;

        BinarySemaphore(BinarySemaphore const&) = delete;
        BinarySemaphore& operator=(BinarySemaphore const&) = delete;
        BinarySemaphore(BinarySemaphore&&) = delete;
        BinarySemaphore& operator=(BinarySemaphore&&) = delete;

        void acquire()
        {
            Platform::BinarySemaphore::acquire();
        }

        bool tryAcquire()
        {
            return Platform::BinarySemaphore::tryAcquire();
        }

        bool tryAcquireFor(uint32 timeoutMs)
        {
            return Platform::BinarySemaphore::tryAcquireFor(timeoutMs);
        }

        void release()
        {
            Platform::BinarySemaphore::release();
        }
    };
}
