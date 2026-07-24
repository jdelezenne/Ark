#include "Ark/Concurrency/Platform/Windows/ConditionVariable.hpp"
#include "Ark/Concurrency/Platform/Windows/Mutex.hpp"

namespace Ark::Concurrency::Platform
{
    void ConditionVariable::wait(Mutex& mutex)
    {
        SleepConditionVariableSRW(&cv, mutex.nativeHandle(), INFINITE, 0);
    }

    bool ConditionVariable::waitFor(Mutex& mutex, uint32 timeoutMs)
    {
        return SleepConditionVariableSRW(&cv, mutex.nativeHandle(), timeoutMs, 0) != 0;
    }
}
