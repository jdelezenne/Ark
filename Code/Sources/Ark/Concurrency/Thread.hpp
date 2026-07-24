#pragma once

#include "Ark/Core/Platform.hpp"

#if defined(ARK_PLATFORM_WINDOWS)
#include "Ark/Concurrency/Platform/Windows/Thread.hpp"
#elif defined(ARK_PLATFORM_POSIX)
#include "Ark/Concurrency/Platform/Posix/Thread.hpp"
#else
#error "Ark::Concurrency::Thread is not implemented for this platform"
#endif

namespace Ark::Concurrency
{
    struct Thread final : public Platform::Thread
    {
        using EntryFunction = typename Platform::Thread::EntryFunction;
        using EntryFunctionPointer = int32 (*)(void*);

        Thread() = default;

        Thread(Platform::Thread&& other)
            : Platform::Thread(Ark::move(other))
        {
        }

        Thread(Thread const&) = delete;
        Thread& operator=(Thread const&) = delete;
        Thread(Thread&&) = default;
        Thread& operator=(Thread&&) = default;

        static Thread create(EntryFunction entry, void* userData)
        {
            return Thread(Platform::Thread::create(Ark::move(entry), userData));
        }

        static Thread create(EntryFunctionPointer entry, void* userData)
        {
            return create(EntryFunction(entry), userData);
        }

        static Thread create(EntryFunction entry, Ark::String const& name, void* userData)
        {
            return Thread(Platform::Thread::create(Ark::move(entry), name, userData));
        }

        static Thread create(EntryFunctionPointer entry, Ark::String const& name, void* userData)
        {
            return create(EntryFunction(entry), name, userData);
        }

        static Thread createWithStackSize(EntryFunction entry, Ark::usize stackSize, void* userData)
        {
            return Thread(Platform::Thread::createWithStackSize(Ark::move(entry), stackSize, userData));
        }

        static Thread createWithStackSize(EntryFunctionPointer entry, Ark::usize stackSize, void* userData)
        {
            return createWithStackSize(EntryFunction(entry), stackSize, userData);
        }

        static Thread createWithStackSize(EntryFunction entry, Ark::usize stackSize, Ark::String const& name, void* userData)
        {
            return Thread(Platform::Thread::createWithStackSize(Ark::move(entry), stackSize, name, userData));
        }

        static Thread createWithStackSize(EntryFunctionPointer entry, Ark::usize stackSize, Ark::String const& name, void* userData)
        {
            return createWithStackSize(EntryFunction(entry), stackSize, name, userData);
        }
    };
}
