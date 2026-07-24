#pragma once

#if defined(ARK_PLATFORM_WINDOWS)

#include "Ark/Core/Platform/Windows/Platform.hpp"
#include "Ark/Core/Types.hpp"

namespace Ark::Concurrency::Platform
{
    struct Mutex;

    struct ConditionVariable
    {
    private:
        CONDITION_VARIABLE cv;

    public:
        ConditionVariable()
        {
            InitializeConditionVariable(&cv);
        }

        ~ConditionVariable() = default;

        void wait(Mutex& mutex);
        bool waitFor(Mutex& mutex, uint32 timeoutMs);

        void notifyOne()
        {
            WakeConditionVariable(&cv);
        }
        void notifyAll()
        {
            WakeAllConditionVariable(&cv);
        }
    };
}

#endif
