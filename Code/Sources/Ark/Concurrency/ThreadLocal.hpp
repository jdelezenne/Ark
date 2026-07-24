#pragma once

#include "Ark/Core/Debug/Assert.hpp"
#include "Ark/Core/Function.hpp"
#include "Ark/Core/Macros.hpp"
#include "Ark/Core/Platform.hpp"
#include "Ark/Core/Types.hpp"
#include "Ark/Core/Utilities.hpp"

#if defined(ARK_PLATFORM_WINDOWS)
#include "Ark/Core/Platform/Windows/Platform.hpp"
#elif defined(ARK_PLATFORM_POSIX)
#include <pthread.h>
#else
#error "Ark::Concurrency::ThreadLocal is not implemented for this platform"
#endif

namespace Ark::Concurrency
{
    /// Type-safe thread-local storage wrapper.
    /// Each ThreadLocal instance owns a platform TLS key. Values created on a thread
    /// are destroyed when that thread exits (or when replaced via set()).
    template <typename T>
    struct ThreadLocal final
    {
        ARK_NOT_COPYABLE(ThreadLocal);
        ARK_NOT_MOVABLE(ThreadLocal);

    private:
        Function<T()> valueFactory;

#if defined(ARK_PLATFORM_WINDOWS)
        DWORD tlsIndex = TLS_OUT_OF_INDEXES;
#elif defined(ARK_PLATFORM_POSIX)
        pthread_key_t key{};
        bool keyValid = false;
#endif

        static void destroyValue(void* pointer)
        {
            delete static_cast<T*>(pointer);
        }

    public:
        ThreadLocal()
        {
            createKey();
        }

        /// Constructor with initial value factory
        /// @param factory Function to create initial value for each thread
        template <typename Factory>
        explicit ThreadLocal(Factory factory)
            : valueFactory(Ark::move(factory))
        {
            createKey();
        }

        ~ThreadLocal()
        {
#if defined(ARK_PLATFORM_WINDOWS)
            if (tlsIndex != TLS_OUT_OF_INDEXES)
            {
                void* pointer = TlsGetValue(tlsIndex);
                if (pointer != nullptr)
                {
                    destroyValue(pointer);
                }
                TlsFree(tlsIndex);
                tlsIndex = TLS_OUT_OF_INDEXES;
            }
#elif defined(ARK_PLATFORM_POSIX)
            if (keyValid)
            {
                void* pointer = pthread_getspecific(key);
                if (pointer != nullptr)
                {
                    destroyValue(pointer);
                    pthread_setspecific(key, nullptr);
                }
                pthread_key_delete(key);
                keyValid = false;
            }
#endif
        }

        /// Get reference to thread-local value
        /// @return Reference to thread-local value
        T& get()
        {
            T* ptr = getPtr();
            if (ptr == nullptr)
            {
                if (valueFactory)
                {
                    set(valueFactory());
                }
                else
                {
                    set(T{});
                }
                ptr = getPtr();
            }

            ARK_ASSERT(ptr != nullptr);
            return *ptr;
        }

        /// Get pointer to thread-local value
        /// @return Pointer to thread-local value, or nullptr if not set
        T* getPtr()
        {
#if defined(ARK_PLATFORM_WINDOWS)
            return static_cast<T*>(TlsGetValue(tlsIndex));
#elif defined(ARK_PLATFORM_POSIX)
            return static_cast<T*>(pthread_getspecific(key));
#else
            return nullptr;
#endif
        }

        /// Set thread-local value
        /// @param value Value to set
        void set(T const& value)
        {
            setPointer(new T(value));
        }

        /// Set thread-local value (move version)
        /// @param value Value to set
        void set(T&& value)
        {
            setPointer(new T(Ark::move(value)));
        }

        T const& operator*() const
        {
            return const_cast<ThreadLocal*>(this)->get();
        }

        T& operator*()
        {
            return get();
        }

        T const* operator->() const
        {
            return &const_cast<ThreadLocal*>(this)->get();
        }

        T* operator->()
        {
            return &get();
        }

    private:
        void createKey()
        {
#if defined(ARK_PLATFORM_WINDOWS)
            tlsIndex = TlsAlloc();
            ARK_ASSERT(tlsIndex != TLS_OUT_OF_INDEXES);
#elif defined(ARK_PLATFORM_POSIX)
            keyValid = pthread_key_create(&key, &ThreadLocal::destroyValue) == 0;
            ARK_ASSERT(keyValid);
#endif
        }

        void setPointer(T* pointer)
        {
#if defined(ARK_PLATFORM_WINDOWS)
            void* previous = TlsGetValue(tlsIndex);
            if (previous != nullptr)
            {
                destroyValue(previous);
            }
            BOOL const ok = TlsSetValue(tlsIndex, pointer);
            ARK_ASSERT(ok != 0);
            if (!ok)
            {
                destroyValue(pointer);
            }
#elif defined(ARK_PLATFORM_POSIX)
            void* previous = pthread_getspecific(key);
            if (previous != nullptr)
            {
                destroyValue(previous);
            }
            int const rc = pthread_setspecific(key, pointer);
            ARK_ASSERT(rc == 0);
            if (rc != 0)
            {
                destroyValue(pointer);
            }
#else
            destroyValue(pointer);
#endif
        }
    };
}
