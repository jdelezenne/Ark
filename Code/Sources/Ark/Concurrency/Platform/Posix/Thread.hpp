#pragma once

#include "Ark/Concurrency/Types.hpp"
#include "Ark/Core/Debug/Assert.hpp"
#include "Ark/Core/Function.hpp"
#include "Ark/Core/Types.hpp"
#include "Ark/Memory/Functions.hpp"
#include "Ark/Strings/String.hpp"

#include <pthread.h>
#include <sched.h>
#include <unistd.h>

#if defined(ARK_PLATFORM_LINUX)
#include <sys/resource.h>
#endif

namespace Ark::Concurrency::Platform
{
    struct Thread
    {
    public:
        using EntryFunction = Ark::Function<int32(void*)>;

    private:
        pthread_t handle{};
        bool valid = false;

        struct StartData
        {
            EntryFunction func;
            void* userData;
            char name[16]{};
        };

        static void applyThreadName(char const* name)
        {
            if (name == nullptr || name[0] == '\0')
            {
                return;
            }

#if defined(ARK_PLATFORM_APPLE)
            pthread_setname_np(name);
#elif defined(ARK_PLATFORM_LINUX)
            pthread_setname_np(pthread_self(), name);
#endif
        }

        static void* trampoline(void* arg)
        {
            auto* data = static_cast<StartData*>(arg);
            applyThreadName(data->name);
            int32 rc = data->func(data->userData);
            delete data;
            return reinterpret_cast<void*>(static_cast<intptr>(rc));
        }

        static void copyThreadName(char (&destination)[16], Ark::String const& name)
        {
            destination[0] = '\0';
            if (name.isEmpty())
            {
                return;
            }

            // pthread names are capped at 15 characters on Linux.
            usize const length = name.getLength() < 15 ? name.getLength() : 15;
            Memory::copy(name.asPointer(), destination, static_cast<uint>(length));
            destination[length] = '\0';
        }

    public:
        Thread() = default;

        ~Thread()
        {
            if (valid)
            {
                ARK_DEBUG_ASSERT_MSG(false, "Thread destroyed while still joinable; detaching");
                detach();
            }
        }

        Thread(Thread const&) = delete;
        Thread& operator=(Thread const&) = delete;

        Thread(Thread&& other) noexcept
            : handle(other.handle)
            , valid(other.valid)
        {
            other.handle = {};
            other.valid = false;
        }

        Thread& operator=(Thread&& other) noexcept
        {
            if (this != &other)
            {
                if (valid)
                {
                    detach();
                }

                handle = other.handle;
                valid = other.valid;

                other.handle = {};
                other.valid = false;
            }

            return *this;
        }

        static Thread create(EntryFunction entry, Ark::String const& name, void* userData)
        {
            Thread t;
            t.start(Ark::move(entry), userData, name, 0);
            return t;
        }

        static Thread createWithStackSize(EntryFunction entry, Ark::usize stackSize, Ark::String const& name, void* userData)
        {
            Thread t;
            t.start(Ark::move(entry), userData, name, stackSize);
            return t;
        }

        static Thread create(EntryFunction entry, void* userData)
        {
            Thread t;
            t.start(Ark::move(entry), userData, Ark::String{}, 0);
            return t;
        }

        static Thread createWithStackSize(EntryFunction entry, Ark::usize stackSize, void* userData)
        {
            Thread t;
            t.start(Ark::move(entry), userData, Ark::String{}, stackSize);
            return t;
        }

        bool isValid() const
        {
            return valid;
        }

        Ark::Concurrency::ThreadId getId() const
        {
            return static_cast<Ark::Concurrency::ThreadId>(reinterpret_cast<uintptr>(handle));
        }

        void join(int32* exitCode)
        {
            void* ret = nullptr;
            if (valid)
            {
                pthread_join(handle, &ret);
                valid = false;
                if (exitCode)
                {
                    *exitCode = static_cast<int32>(reinterpret_cast<intptr>(ret));
                }
            }
        }

        void detach()
        {
            if (valid)
            {
                pthread_detach(handle);
                valid = false;
            }
        }

        static Ark::Concurrency::ThreadId getCurrentId()
        {
            pthread_t self = pthread_self();
            return static_cast<Ark::Concurrency::ThreadId>(reinterpret_cast<uintptr>(self));
        }

        static bool setCurrentPriority(Ark::Concurrency::ThreadPriority priority)
        {
#if defined(ARK_PLATFORM_APPLE)
            qos_class_t qos = QOS_CLASS_DEFAULT;
            switch (priority)
            {
                case Ark::Concurrency::ThreadPriority::Low:
                    qos = QOS_CLASS_UTILITY;
                    break;
                case Ark::Concurrency::ThreadPriority::Normal:
                    qos = QOS_CLASS_DEFAULT;
                    break;
                case Ark::Concurrency::ThreadPriority::High:
                    qos = QOS_CLASS_USER_INITIATED;
                    break;
                case Ark::Concurrency::ThreadPriority::TimeCritical:
                    qos = QOS_CLASS_USER_INTERACTIVE;
                    break;
            }
            return pthread_set_qos_class_self_np(qos, 0) == 0;
#elif defined(ARK_PLATFORM_LINUX)
            int niceValue = 0;
            switch (priority)
            {
                case Ark::Concurrency::ThreadPriority::Low:
                    niceValue = 5;
                    break;
                case Ark::Concurrency::ThreadPriority::Normal:
                    niceValue = 0;
                    break;
                case Ark::Concurrency::ThreadPriority::High:
                    niceValue = -5;
                    break;
                case Ark::Concurrency::ThreadPriority::TimeCritical:
                    niceValue = -10;
                    break;
            }
            return setpriority(PRIO_PROCESS, 0, niceValue) == 0;
#else
            (void)priority;
            return false;
#endif
        }

        static void yield()
        {
            sched_yield();
        }

        static void sleep(Ark::uint32 ms)
        {
            usleep(ms * 1000);
        }

    private:
        void start(EntryFunction entry, void* userData, Ark::String const& name, Ark::usize stackSize)
        {
            pthread_attr_t attr;
            pthread_attr_init(&attr);
            if (stackSize != 0)
            {
                pthread_attr_setstacksize(&attr, stackSize);
            }

            auto* data = new StartData{};
            data->func = Ark::move(entry);
            data->userData = userData;
            copyThreadName(data->name, name);

            valid = (pthread_create(&handle, &attr, &trampoline, data) == 0);
            if (!valid)
            {
                delete data;
            }
            pthread_attr_destroy(&attr);
        }
    };
}
