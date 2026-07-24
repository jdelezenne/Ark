#pragma once

#include "Ark/Concurrency/Types.hpp"
#include "Ark/Core/Traits.hpp"

namespace Ark::Concurrency
{
    namespace Internal
    {
        inline int toBuiltinOrder(MemoryOrder order)
        {
            switch (order)
            {
                case MemoryOrder::Relaxed:
                    return __ATOMIC_RELAXED;
                case MemoryOrder::Acquire:
                    return __ATOMIC_ACQUIRE;
                case MemoryOrder::Release:
                    return __ATOMIC_RELEASE;
                case MemoryOrder::AcquireRelease:
                    return __ATOMIC_ACQ_REL;
                case MemoryOrder::SequentiallyConsistent:
                default:
                    return __ATOMIC_SEQ_CST;
            }
        }
    }

    template <typename T>
    struct Atomic
    {
    private:
        alignas(T) volatile T value{};

    public:
        Atomic() = default;
        Atomic(T initialValue)
            : value(initialValue)
        {
        }

        T load(MemoryOrder order = MemoryOrder::SequentiallyConsistent) const
        {
            return __atomic_load_n(&value, Internal::toBuiltinOrder(order));
        }

        void store(T newValue, MemoryOrder order = MemoryOrder::SequentiallyConsistent)
        {
            __atomic_store_n(&value, newValue, Internal::toBuiltinOrder(order));
        }

        T exchange(T newValue, MemoryOrder order = MemoryOrder::SequentiallyConsistent)
        {
            return __atomic_exchange_n(&value, newValue, Internal::toBuiltinOrder(order));
        }

        bool compareExchange(T& expected, T desired, MemoryOrder order = MemoryOrder::SequentiallyConsistent)
        {
            int const successOrder = Internal::toBuiltinOrder(order);
            int failureOrder = successOrder;
            if (failureOrder == __ATOMIC_RELEASE)
            {
                failureOrder = __ATOMIC_RELAXED;
            }
            else if (failureOrder == __ATOMIC_ACQ_REL)
            {
                failureOrder = __ATOMIC_ACQUIRE;
            }

            return __atomic_compare_exchange_n(&value, &expected, desired, false, successOrder, failureOrder);
        }

        template <typename U = T>
        typename Traits::EnableIfType<Traits::isArithmetic<U>, U>
        fetchAdd(T addValue, MemoryOrder order = MemoryOrder::SequentiallyConsistent)
        {
            return __atomic_fetch_add(&value, addValue, Internal::toBuiltinOrder(order));
        }

        template <typename U = T>
        typename Traits::EnableIfType<Traits::isArithmetic<U>, U>
        fetchSubtract(T subValue, MemoryOrder order = MemoryOrder::SequentiallyConsistent)
        {
            return __atomic_fetch_sub(&value, subValue, Internal::toBuiltinOrder(order));
        }
    };

    /// Floating point specializations using compare-exchange loops
    template <>
    struct Atomic<float32>
    {
    private:
        Atomic<uint32> storage;

    public:
        Atomic()
            : storage(0)
        {
        }
        Atomic(float32 initialValue)
        {
            uint32 bits;
            __builtin_memcpy(&bits, &initialValue, sizeof(float32));
            storage.store(bits);
        }

        float32 load(MemoryOrder order = MemoryOrder::SequentiallyConsistent) const
        {
            uint32 bits = storage.load(order);
            float32 result;
            __builtin_memcpy(&result, &bits, sizeof(float32));
            return result;
        }

        void store(float32 newValue, MemoryOrder order = MemoryOrder::SequentiallyConsistent)
        {
            uint32 bits;
            __builtin_memcpy(&bits, &newValue, sizeof(float32));
            storage.store(bits, order);
        }

        float32 exchange(float32 newValue, MemoryOrder order = MemoryOrder::SequentiallyConsistent)
        {
            uint32 newBits;
            __builtin_memcpy(&newBits, &newValue, sizeof(float32));
            uint32 oldBits = storage.exchange(newBits, order);
            float32 result;
            __builtin_memcpy(&result, &oldBits, sizeof(float32));
            return result;
        }

        bool compareExchange(float32& expected, float32 desired, MemoryOrder order = MemoryOrder::SequentiallyConsistent)
        {
            uint32 expectedBits, desiredBits;
            __builtin_memcpy(&expectedBits, &expected, sizeof(float32));
            __builtin_memcpy(&desiredBits, &desired, sizeof(float32));

            bool result = storage.compareExchange(expectedBits, desiredBits, order);
            if (!result)
            {
                __builtin_memcpy(&expected, &expectedBits, sizeof(float32));
            }
            return result;
        }

        float32 fetchAdd(float32 addValue, MemoryOrder order = MemoryOrder::SequentiallyConsistent)
        {
            float32 expected = load(order);
            float32 desired;
            do
            {
                desired = expected + addValue;
            }
            while (!compareExchange(expected, desired, order));
            return expected;
        }

        float32 fetchSubtract(float32 subValue, MemoryOrder order = MemoryOrder::SequentiallyConsistent)
        {
            return fetchAdd(-subValue, order);
        }
    };

    template <>
    struct Atomic<float64>
    {
    private:
        Atomic<uint64> storage;

    public:
        Atomic()
            : storage(0)
        {
        }

        Atomic(float64 initialValue)
        {
            uint64 bits;
            __builtin_memcpy(&bits, &initialValue, sizeof(float64));
            storage.store(bits);
        }

        float64 load(MemoryOrder order = MemoryOrder::SequentiallyConsistent) const
        {
            uint64 bits = storage.load(order);
            float64 result;
            __builtin_memcpy(&result, &bits, sizeof(float64));
            return result;
        }

        void store(float64 newValue, MemoryOrder order = MemoryOrder::SequentiallyConsistent)
        {
            uint64 bits;
            __builtin_memcpy(&bits, &newValue, sizeof(float64));
            storage.store(bits, order);
        }

        float64 exchange(float64 newValue, MemoryOrder order = MemoryOrder::SequentiallyConsistent)
        {
            uint64 newBits;
            __builtin_memcpy(&newBits, &newValue, sizeof(float64));
            uint64 oldBits = storage.exchange(newBits, order);
            float64 result;
            __builtin_memcpy(&result, &oldBits, sizeof(float64));
            return result;
        }

        bool compareExchange(float64& expected, float64 desired, MemoryOrder order = MemoryOrder::SequentiallyConsistent)
        {
            uint64 expectedBits, desiredBits;
            __builtin_memcpy(&expectedBits, &expected, sizeof(float64));
            __builtin_memcpy(&desiredBits, &desired, sizeof(float64));

            bool result = storage.compareExchange(expectedBits, desiredBits, order);
            if (!result)
            {
                __builtin_memcpy(&expected, &expectedBits, sizeof(float64));
            }
            return result;
        }

        float64 fetchAdd(float64 addValue, MemoryOrder order = MemoryOrder::SequentiallyConsistent)
        {
            float64 expected = load(order);
            float64 desired;
            do
            {
                desired = expected + addValue;
            }
            while (!compareExchange(expected, desired, order));
            return expected;
        }

        float64 fetchSubtract(float64 subValue, MemoryOrder order = MemoryOrder::SequentiallyConsistent)
        {
            return fetchAdd(-subValue, order);
        }
    };

    struct AtomicFlag
    {
    private:
        Atomic<bool> flag{false};

    public:
        bool testAndSet(MemoryOrder order = MemoryOrder::SequentiallyConsistent)
        {
            return flag.exchange(true, order);
        }

        void clear(MemoryOrder order = MemoryOrder::SequentiallyConsistent)
        {
            flag.store(false, order);
        }

        bool test(MemoryOrder order = MemoryOrder::SequentiallyConsistent) const
        {
            return flag.load(order);
        }
    };

    inline void compilerBarrier()
    {
        __asm__ __volatile__("" ::: "memory");
    }

    inline void memoryBarrierAcquire()
    {
        __atomic_thread_fence(__ATOMIC_ACQUIRE);
    }

    inline void memoryBarrierRelease()
    {
        __atomic_thread_fence(__ATOMIC_RELEASE);
    }

    inline void memoryBarrierFull()
    {
        __atomic_thread_fence(__ATOMIC_SEQ_CST);
    }

    inline void cpuPause()
    {
#if defined(__aarch64__) || defined(__arm__)
        __asm__ __volatile__("yield");
#elif defined(__x86_64__) || defined(__i386__)
        __builtin_ia32_pause();
#else
        __asm__ __volatile__("" ::: "memory");
#endif
    }
}
