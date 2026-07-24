#pragma once

#include "Ark/Concurrency/Atomic.hpp"
#include "Ark/Core/Macros.hpp"
#include "Ark/Core/Types.hpp"

namespace Ark::Concurrency
{
    /// Lightweight spin lock for short critical sections
    struct SpinLock final
    {
        ARK_NOT_COPYABLE(SpinLock);
        ARK_NOT_MOVABLE(SpinLock);

    private:
        Atomic<bool> flag;

    public:
        SpinLock()
            : flag(false)
        {
        }

        void lock()
        {
            while (flag.exchange(true, MemoryOrder::Acquire))
            {
                while (flag.load(MemoryOrder::Relaxed))
                {
                    cpuPause();
                }
            }
        }

        bool tryLock()
        {
            return !flag.exchange(true, MemoryOrder::Acquire);
        }

        void unlock()
        {
            flag.store(false, MemoryOrder::Release);
        }
    };
}
