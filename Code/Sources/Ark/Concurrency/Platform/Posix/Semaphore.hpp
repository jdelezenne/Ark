#pragma once

#include "Ark/Core/Types.hpp"

#include <pthread.h>
#include <time.h>

namespace Ark::Concurrency::Platform
{
    namespace Internal
    {
        inline uint64 getMonotonicMilliseconds()
        {
            timespec ts{};
            clock_gettime(CLOCK_MONOTONIC, &ts);
            return static_cast<uint64>(ts.tv_sec) * 1000ull + static_cast<uint64>(ts.tv_nsec) / 1000000ull;
        }

        inline timespec makeRelativeTimeout(uint32 timeoutMs)
        {
            timespec ts{};
            ts.tv_sec = static_cast<time_t>(timeoutMs / 1000);
            ts.tv_nsec = static_cast<long>((timeoutMs % 1000) * 1000000L);
            return ts;
        }
    }

    struct Semaphore
    {
    private:
        pthread_mutex_t mutex{};
        pthread_cond_t condition{};
        uint32 count = 0;

    public:
        explicit Semaphore(uint32 initialCount)
            : count(initialCount)
        {
            pthread_mutex_init(&mutex, nullptr);
            pthread_cond_init(&condition, nullptr);
        }

        ~Semaphore()
        {
            pthread_cond_destroy(&condition);
            pthread_mutex_destroy(&mutex);
        }

        void acquire()
        {
            pthread_mutex_lock(&mutex);
            while (count == 0)
            {
                pthread_cond_wait(&condition, &mutex);
            }
            --count;
            pthread_mutex_unlock(&mutex);
        }

        bool tryAcquire()
        {
            pthread_mutex_lock(&mutex);
            if (count == 0)
            {
                pthread_mutex_unlock(&mutex);
                return false;
            }
            --count;
            pthread_mutex_unlock(&mutex);
            return true;
        }

        bool tryAcquireFor(uint32 timeoutMs)
        {
            uint64 const deadlineMs = Internal::getMonotonicMilliseconds() + timeoutMs;

            pthread_mutex_lock(&mutex);
            while (count == 0)
            {
                uint64 const nowMs = Internal::getMonotonicMilliseconds();
                if (nowMs >= deadlineMs)
                {
                    pthread_mutex_unlock(&mutex);
                    return false;
                }

                timespec const relative = Internal::makeRelativeTimeout(static_cast<uint32>(deadlineMs - nowMs));
                int const rc = pthread_cond_timedwait_relative_np(&condition, &mutex, &relative);
                if (rc != 0)
                {
                    pthread_mutex_unlock(&mutex);
                    return false;
                }
            }
            --count;
            pthread_mutex_unlock(&mutex);
            return true;
        }

        void release()
        {
            release(1);
        }

        void release(uint32 releaseCount)
        {
            if (releaseCount == 0)
            {
                return;
            }

            pthread_mutex_lock(&mutex);
            count += releaseCount;
            if (releaseCount == 1)
            {
                pthread_cond_signal(&condition);
            }
            else
            {
                pthread_cond_broadcast(&condition);
            }
            pthread_mutex_unlock(&mutex);
        }
    };

    struct BinarySemaphore
    {
    private:
        pthread_mutex_t mutex{};
        pthread_cond_t condition{};
        bool available = false;

    public:
        explicit BinarySemaphore(bool initiallyAcquired)
            : available(!initiallyAcquired)
        {
            pthread_mutex_init(&mutex, nullptr);
            pthread_cond_init(&condition, nullptr);
        }

        ~BinarySemaphore()
        {
            pthread_cond_destroy(&condition);
            pthread_mutex_destroy(&mutex);
        }

        void acquire()
        {
            pthread_mutex_lock(&mutex);
            while (!available)
            {
                pthread_cond_wait(&condition, &mutex);
            }
            available = false;
            pthread_mutex_unlock(&mutex);
        }

        bool tryAcquire()
        {
            pthread_mutex_lock(&mutex);
            if (!available)
            {
                pthread_mutex_unlock(&mutex);
                return false;
            }
            available = false;
            pthread_mutex_unlock(&mutex);
            return true;
        }

        bool tryAcquireFor(uint32 timeoutMs)
        {
            uint64 const deadlineMs = Internal::getMonotonicMilliseconds() + timeoutMs;

            pthread_mutex_lock(&mutex);
            while (!available)
            {
                uint64 const nowMs = Internal::getMonotonicMilliseconds();
                if (nowMs >= deadlineMs)
                {
                    pthread_mutex_unlock(&mutex);
                    return false;
                }

                timespec const relative = Internal::makeRelativeTimeout(static_cast<uint32>(deadlineMs - nowMs));
                int const rc = pthread_cond_timedwait_relative_np(&condition, &mutex, &relative);
                if (rc != 0)
                {
                    pthread_mutex_unlock(&mutex);
                    return false;
                }
            }
            available = false;
            pthread_mutex_unlock(&mutex);
            return true;
        }

        void release()
        {
            pthread_mutex_lock(&mutex);
            if (!available)
            {
                available = true;
                pthread_cond_signal(&condition);
            }
            pthread_mutex_unlock(&mutex);
        }
    };
}
