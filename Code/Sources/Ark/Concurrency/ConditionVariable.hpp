#pragma once

#include "Ark/Concurrency/Locks.hpp"
#include "Ark/Concurrency/Mutex.hpp"
#include "Ark/Concurrency/Types.hpp"
#include "Ark/Core/Duration.hpp"
#include "Ark/Core/Macros.hpp"
#include "Ark/Core/Platform.hpp"
#include "Ark/System/Time.hpp"

#if defined(ARK_PLATFORM_WINDOWS)
#include "Ark/Concurrency/Platform/Windows/ConditionVariable.hpp"
#elif defined(ARK_PLATFORM_POSIX)
#include "Ark/Concurrency/Platform/Posix/ConditionVariable.hpp"
#else
#error "Ark::Concurrency::ConditionVariable is not implemented for this platform"
#endif

namespace Ark::Concurrency
{
    namespace Internal
    {
        struct ConditionVariableTiming final
        {
            static uint64 getCurrentTimeMilliseconds()
            {
                Result<uint64> currentTimeResult = System::Time::getCurrentTimeMilliseconds();
                ARK_ASSERT(currentTimeResult.isOk());
                return currentTimeResult.getValueOr(0ULL);
            }

            static uint32 clampTimeoutMilliseconds(float64 timeoutMs)
            {
                if (timeoutMs <= 0.0)
                {
                    return 0;
                }

                constexpr float64 maxTimeout = static_cast<float64>(~uint32{0});
                if (timeoutMs >= maxTimeout)
                {
                    return ~uint32{0};
                }

                return static_cast<uint32>(timeoutMs);
            }

            static uint32 getRemainingTimeoutMilliseconds(uint64 deadlineMs)
            {
                uint64 const currentMs = getCurrentTimeMilliseconds();
                if (currentMs >= deadlineMs)
                {
                    return 0;
                }

                uint64 const remainingMs = deadlineMs - currentMs;
                if (remainingMs >= static_cast<uint64>(~uint32{0}))
                {
                    return ~uint32{0};
                }

                return static_cast<uint32>(remainingMs);
            }
        };
    }

    /// Condition variable for thread synchronization
    struct ConditionVariable final : public Platform::ConditionVariable
    {
        ARK_STRUCT(ConditionVariable);

    public:
        ConditionVariable() = default;
        ~ConditionVariable() = default;

        /// Wait for notification
        /// @param lock Lock that must own the mutex on entry
        void wait(UniqueLock<Mutex>& lock)
        {
            ARK_ASSERT(lock.mutex() != nullptr);
            ARK_ASSERT(lock.ownsLock());
            Platform::ConditionVariable::wait(static_cast<Platform::Mutex&>(*lock.mutex()));
        }

        /// Wait with predicate (loops until predicate returns true)
        /// @param lock Lock that must own the mutex on entry
        /// @param predicate Function that returns true when condition is met
        template <typename Predicate>
        void wait(UniqueLock<Mutex>& lock, Predicate&& predicate)
        {
            while (!predicate())
            {
                wait(lock);
            }
        }

        /// Wait with timeout
        /// @param lock Lock that must own the mutex on entry
        /// @param timeoutMs Timeout in milliseconds
        /// @return True if awakened before timeout, false if timeout elapsed
        bool waitFor(UniqueLock<Mutex>& lock, uint32 timeoutMs)
        {
            ARK_ASSERT(lock.mutex() != nullptr);
            ARK_ASSERT(lock.ownsLock());
            return Platform::ConditionVariable::waitFor(static_cast<Platform::Mutex&>(*lock.mutex()), timeoutMs);
        }

        /// Wait with timeout duration.
        /// @param lock Lock that must own the mutex on entry
        /// @param timeout Duration to wait
        /// @return True if awakened before timeout, false if timeout elapsed
        bool waitFor(UniqueLock<Mutex>& lock, Duration const& timeout)
        {
            return waitFor(lock, Internal::ConditionVariableTiming::clampTimeoutMilliseconds(timeout.getTotalMilliseconds()));
        }

        /// Wait with timeout and predicate
        /// @param lock Lock that must own the mutex on entry
        /// @param timeoutMs Timeout in milliseconds
        /// @param predicate Function that returns true when condition is met
        /// @return True if predicate satisfied, false if timeout
        template <typename Predicate>
        bool waitFor(UniqueLock<Mutex>& lock, uint32 timeoutMs, Predicate&& predicate)
        {
            uint64 const deadlineMs = Internal::ConditionVariableTiming::getCurrentTimeMilliseconds() + timeoutMs;
            while (!predicate())
            {
                uint32 const remainingTimeoutMs = Internal::ConditionVariableTiming::getRemainingTimeoutMilliseconds(deadlineMs);
                if (remainingTimeoutMs == 0)
                {
                    return predicate();
                }

                if (!waitFor(lock, remainingTimeoutMs) && !predicate())
                {
                    return false;
                }
            }
            return true;
        }

        /// Wait with timeout duration and predicate.
        /// @param lock Lock that must own the mutex on entry
        /// @param timeout Duration to wait
        /// @param predicate Function that returns true when condition is met
        /// @return True if predicate satisfied, false if timeout elapsed
        template <typename Predicate>
        bool waitFor(UniqueLock<Mutex>& lock, Duration const& timeout, Predicate&& predicate)
        {
            return waitFor(lock, Internal::ConditionVariableTiming::clampTimeoutMilliseconds(timeout.getTotalMilliseconds()), Ark::forward<Predicate>(predicate));
        }

        /// Notify one waiting thread
        void notifyOne()
        {
            Platform::ConditionVariable::notifyOne();
        }

        /// Notify all waiting threads
        void notifyAll()
        {
            Platform::ConditionVariable::notifyAll();
        }
    };

    /// Condition variable that can wait on any lock type exposing `lock()` and `unlock()`.
    struct ConditionVariableAny final
    {
        ARK_NOT_COPYABLE(ConditionVariableAny);
        ARK_NOT_MOVABLE(ConditionVariableAny);

    private:
        Mutex waitMutex;
        ConditionVariable condition;

    public:
        ConditionVariableAny() = default;
        ~ConditionVariableAny() = default;

        template <typename LockType>
        void wait(LockType& lock)
        {
            UniqueLock<Mutex> waitLock(waitMutex);
            lock.unlock();
            condition.wait(waitLock);
            waitLock.unlock();
            lock.lock();
        }

        template <typename LockType, typename Predicate>
        void wait(LockType& lock, Predicate&& predicate)
        {
            while (!predicate())
            {
                wait(lock);
            }
        }

        template <typename LockType>
        bool waitFor(LockType& lock, uint32 timeoutMs)
        {
            UniqueLock<Mutex> waitLock(waitMutex);
            lock.unlock();
            bool const awakened = condition.waitFor(waitLock, timeoutMs);
            waitLock.unlock();
            lock.lock();
            return awakened;
        }

        template <typename LockType>
        bool waitFor(LockType& lock, Duration const& timeout)
        {
            return waitFor(lock, Internal::ConditionVariableTiming::clampTimeoutMilliseconds(timeout.getTotalMilliseconds()));
        }

        template <typename LockType, typename Predicate>
        bool waitFor(LockType& lock, uint32 timeoutMs, Predicate&& predicate)
        {
            uint64 const deadlineMs = Internal::ConditionVariableTiming::getCurrentTimeMilliseconds() + timeoutMs;
            while (!predicate())
            {
                uint32 const remainingTimeoutMs = Internal::ConditionVariableTiming::getRemainingTimeoutMilliseconds(deadlineMs);
                if (remainingTimeoutMs == 0)
                {
                    return predicate();
                }

                if (!waitFor(lock, remainingTimeoutMs) && !predicate())
                {
                    return false;
                }
            }

            return true;
        }

        template <typename LockType, typename Predicate>
        bool waitFor(LockType& lock, Duration const& timeout, Predicate&& predicate)
        {
            return waitFor(lock, Internal::ConditionVariableTiming::clampTimeoutMilliseconds(timeout.getTotalMilliseconds()), Ark::forward<Predicate>(predicate));
        }

        void notifyOne()
        {
            UniqueLock<Mutex> waitLock(waitMutex);
            condition.notifyOne();
        }

        void notifyAll()
        {
            UniqueLock<Mutex> waitLock(waitMutex);
            condition.notifyAll();
        }
    };
}
