#pragma once

#include "Ark/Concurrency/Platform/Posix/Mutex.hpp"
#include "Ark/Core/Types.hpp"

#include <pthread.h>
#include <time.h>

namespace Ark::Concurrency::Platform
{
    struct ConditionVariable
    {
    private:
        pthread_cond_t cond{};

#if !defined(ARK_PLATFORM_APPLE)
        static void addMilliseconds(timespec& ts, uint32 timeoutMs)
        {
            ts.tv_sec += static_cast<time_t>(timeoutMs / 1000);
            ts.tv_nsec += static_cast<long>((timeoutMs % 1000) * 1000000L);
            if (ts.tv_nsec >= 1000000000L)
            {
                ts.tv_sec += 1;
                ts.tv_nsec -= 1000000000L;
            }
        }
#endif

    public:
        ConditionVariable()
        {
#if defined(ARK_PLATFORM_APPLE)
            pthread_cond_init(&cond, nullptr);
#else
            pthread_condattr_t attr;
            pthread_condattr_init(&attr);
            pthread_condattr_setclock(&attr, CLOCK_MONOTONIC);
            pthread_cond_init(&cond, &attr);
            pthread_condattr_destroy(&attr);
#endif
        }

        ~ConditionVariable()
        {
            pthread_cond_destroy(&cond);
        }

        void wait(Mutex& mutex)
        {
            pthread_cond_wait(&cond, mutex.nativeHandle());
        }

        bool waitFor(Mutex& mutex, uint32 timeoutMs)
        {
#if defined(ARK_PLATFORM_APPLE)
            timespec ts{};
            ts.tv_sec = static_cast<time_t>(timeoutMs / 1000);
            ts.tv_nsec = static_cast<long>((timeoutMs % 1000) * 1000000L);
            int const rc = pthread_cond_timedwait_relative_np(&cond, mutex.nativeHandle(), &ts);
            return rc == 0;
#else
            timespec ts{};
            clock_gettime(CLOCK_MONOTONIC, &ts);
            addMilliseconds(ts, timeoutMs);
            int const rc = pthread_cond_timedwait(&cond, mutex.nativeHandle(), &ts);
            return rc == 0;
#endif
        }

        void notifyOne()
        {
            pthread_cond_signal(&cond);
        }

        void notifyAll()
        {
            pthread_cond_broadcast(&cond);
        }
    };
}
