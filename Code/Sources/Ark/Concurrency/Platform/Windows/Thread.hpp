#pragma once

#if defined(ARK_PLATFORM_WINDOWS)

#define WIN32_LEAN_AND_MEAN
#include "Ark/Concurrency/Types.hpp"
#include "Ark/Core/Debug/Assert.hpp"
#include "Ark/Core/Function.hpp"
#include "Ark/Core/Types.hpp"
#include "Ark/Strings/String.hpp"

#include <windows.h>

namespace Ark::Concurrency::Platform
{
    struct Thread
    {
    public:
        using EntryFunction = Ark::Function<int32(void*)>;

    private:
        HANDLE handle = nullptr;
        DWORD threadId = 0;
        bool valid = false;

        struct StartData
        {
            EntryFunction func;
            void* userData;
            wchar_t name[64]{};
        };

        static void applyThreadName(wchar_t const* name)
        {
            if (name == nullptr || name[0] == L'\0')
            {
                return;
            }

            using SetThreadDescriptionFn = HRESULT(WINAPI*)(HANDLE, PCWSTR);
            HMODULE kernel = GetModuleHandleW(L"kernel32.dll");
            if (kernel == nullptr)
            {
                return;
            }

            auto setThreadDescription = reinterpret_cast<SetThreadDescriptionFn>(
                GetProcAddress(kernel, "SetThreadDescription"));
            if (setThreadDescription != nullptr)
            {
                setThreadDescription(GetCurrentThread(), name);
            }
        }

        static DWORD WINAPI trampoline(LPVOID arg)
        {
            StartData* data = static_cast<StartData*>(arg);
            applyThreadName(data->name);
            int32 rc = data->func(data->userData);
            delete data;
            return static_cast<DWORD>(rc);
        }

        static void copyThreadName(wchar_t (&destination)[64], Ark::String const& name)
        {
            destination[0] = L'\0';
            if (name.isEmpty())
            {
                return;
            }

            int const written = MultiByteToWideChar(
                CP_UTF8,
                0,
                name.asPointer(),
                static_cast<int>(name.getLength()),
                destination,
                63);
            if (written > 0)
            {
                destination[written] = L'\0';
            }
            else
            {
                destination[0] = L'\0';
            }
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
            , threadId(other.threadId)
            , valid(other.valid)
        {
            other.handle = nullptr;
            other.threadId = 0;
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
                threadId = other.threadId;
                valid = other.valid;

                other.handle = nullptr;
                other.threadId = 0;
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
            return static_cast<Ark::Concurrency::ThreadId>(threadId);
        }

        void join(int32* exitCode)
        {
            if (valid)
            {
                WaitForSingleObject(handle, INFINITE);
                DWORD code = 0;
                GetExitCodeThread(handle, &code);
                if (exitCode)
                {
                    *exitCode = static_cast<int32>(code);
                }
                CloseHandle(handle);
                handle = nullptr;
                valid = false;
            }
        }

        void detach()
        {
            if (valid)
            {
                CloseHandle(handle);
                handle = nullptr;
                valid = false;
            }
        }

        static Ark::Concurrency::ThreadId getCurrentId()
        {
            return static_cast<Ark::Concurrency::ThreadId>(GetCurrentThreadId());
        }

        static bool setCurrentPriority(Ark::Concurrency::ThreadPriority priority)
        {
            int winPrio = THREAD_PRIORITY_NORMAL;
            switch (priority)
            {
                case Ark::Concurrency::ThreadPriority::Low:
                    winPrio = THREAD_PRIORITY_BELOW_NORMAL;
                    break;
                case Ark::Concurrency::ThreadPriority::Normal:
                    winPrio = THREAD_PRIORITY_NORMAL;
                    break;
                case Ark::Concurrency::ThreadPriority::High:
                    winPrio = THREAD_PRIORITY_ABOVE_NORMAL;
                    break;
                case Ark::Concurrency::ThreadPriority::TimeCritical:
                    winPrio = THREAD_PRIORITY_HIGHEST;
                    break;
                default:
                    break;
            }
            return SetThreadPriority(GetCurrentThread(), winPrio) != 0;
        }

        static void yield()
        {
            SwitchToThread();
        }

        static void sleep(Ark::uint32 ms)
        {
            Sleep(ms);
        }

    private:
        void start(EntryFunction entry, void* userData, Ark::String const& name, Ark::usize stackSize)
        {
            SIZE_T sz = stackSize == 0 ? 0 : static_cast<SIZE_T>(stackSize);
            StartData* data = new StartData{};
            data->func = Ark::move(entry);
            data->userData = userData;
            copyThreadName(data->name, name);

            handle = CreateThread(nullptr, sz, &trampoline, data, 0, &threadId);
            valid = (handle != nullptr);
            if (!valid)
            {
                delete data;
                threadId = 0;
            }
        }
    };
}

#endif
