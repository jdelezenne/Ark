#pragma once

#include <pthread.h>

namespace Ark::Concurrency::Platform
{
    struct Mutex
    {
    private:
        pthread_mutex_t handle{};

    public:
        Mutex()
        {
            pthread_mutex_init(&handle, nullptr);
        }
        ~Mutex()
        {
            pthread_mutex_destroy(&handle);
        }

        void lock()
        {
            pthread_mutex_lock(&handle);
        }
        bool tryLock()
        {
            return pthread_mutex_trylock(&handle) == 0;
        }
        void unlock()
        {
            pthread_mutex_unlock(&handle);
        }

        pthread_mutex_t* nativeHandle()
        {
            return &handle;
        }
    };

    struct RecursiveMutex
    {
    private:
        pthread_mutex_t handle{};

    public:
        RecursiveMutex()
        {
            pthread_mutexattr_t attr;
            pthread_mutexattr_init(&attr);
            pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
            pthread_mutex_init(&handle, &attr);
            pthread_mutexattr_destroy(&attr);
        }
        ~RecursiveMutex()
        {
            pthread_mutex_destroy(&handle);
        }

        void lock()
        {
            pthread_mutex_lock(&handle);
        }
        bool tryLock()
        {
            return pthread_mutex_trylock(&handle) == 0;
        }
        void unlock()
        {
            pthread_mutex_unlock(&handle);
        }
    };

    struct SharedMutex
    {
    private:
        // Use pthread_rwlock as a shared mutex
        pthread_rwlock_t handle{};

    public:
        SharedMutex()
        {
            pthread_rwlock_init(&handle, nullptr);
        }
        ~SharedMutex()
        {
            pthread_rwlock_destroy(&handle);
        }

        void lock()
        {
            pthread_rwlock_wrlock(&handle);
        }
        bool tryLock()
        {
            return pthread_rwlock_trywrlock(&handle) == 0;
        }
        void unlock()
        {
            pthread_rwlock_unlock(&handle);
        }

        void lockShared()
        {
            pthread_rwlock_rdlock(&handle);
        }
        bool tryLockShared()
        {
            return pthread_rwlock_tryrdlock(&handle) == 0;
        }
        void unlockShared()
        {
            pthread_rwlock_unlock(&handle);
        }
    };
}
