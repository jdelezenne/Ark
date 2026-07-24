#pragma once

#include "Ark/Concurrency/Types.hpp"
#include "Ark/Core/Platform.hpp"
#include "Ark/Core/Platform/Windows/Platform.hpp"
#include "Ark/Core/Types.hpp"

#include <intrin.h>

#if defined(ARK_ARCHITECTURE_ARM64)
#include <arm64intr.h>
#endif

namespace Ark::Concurrency
{
    namespace Internal
    {
        /// Convert MemoryOrder to Windows memory ordering
        inline LONG convertMemoryOrder(MemoryOrder order)
        {
            switch (order)
            {
                case MemoryOrder::Relaxed:
                    return 0; // No barrier
                case MemoryOrder::Acquire:
                    return 1; // Acquire semantics
                case MemoryOrder::Release:
                    return 2; // Release semantics
                case MemoryOrder::AcquireRelease:
                    return 3; // Both acquire and release
                case MemoryOrder::SequentiallyConsistent:
                default:
                    return 4; // Full barrier
            }
        }

        /// Compiler/CPU barriers for non-Interlocked paths.
        inline void memoryBarrier(MemoryOrder order)
        {
            switch (order)
            {
                case MemoryOrder::Relaxed:
                    break;
                case MemoryOrder::Acquire:
                    _ReadBarrier();
#if defined(ARK_ARCHITECTURE_ARM64)
                    __dmb(_ARM64_BARRIER_ISHLD);
#endif
                    break;
                case MemoryOrder::Release:
#if defined(ARK_ARCHITECTURE_ARM64)
                    __dmb(_ARM64_BARRIER_ISHST);
#endif
                    _WriteBarrier();
                    break;
                case MemoryOrder::AcquireRelease:
#if defined(ARK_ARCHITECTURE_ARM64)
                    __dmb(_ARM64_BARRIER_ISH);
#endif
                    _ReadWriteBarrier();
                    break;
                case MemoryOrder::SequentiallyConsistent:
                default:
                    MemoryBarrier();
                    break;
            }
        }

        inline bool needsAtomicLoadStore(MemoryOrder order)
        {
            return order != MemoryOrder::Relaxed;
        }

        inline char atomicLoad8(char volatile const* value, MemoryOrder order)
        {
            if (!needsAtomicLoadStore(order))
            {
                return *value;
            }
            return _InterlockedOr8(const_cast<char volatile*>(value), 0);
        }

        inline void atomicStore8(char volatile* value, char newValue, MemoryOrder order)
        {
            if (!needsAtomicLoadStore(order))
            {
                *value = newValue;
                return;
            }
            _InterlockedExchange8(value, newValue);
        }

        inline short atomicLoad16(short volatile const* value, MemoryOrder order)
        {
            if (!needsAtomicLoadStore(order))
            {
                return *value;
            }
            return _InterlockedOr16(const_cast<short volatile*>(value), 0);
        }

        inline void atomicStore16(short volatile* value, short newValue, MemoryOrder order)
        {
            if (!needsAtomicLoadStore(order))
            {
                *value = newValue;
                return;
            }
            _InterlockedExchange16(value, newValue);
        }

        inline LONG atomicLoad32(LONG volatile const* value, MemoryOrder order)
        {
            if (!needsAtomicLoadStore(order))
            {
                return *value;
            }
            return _InterlockedOr(const_cast<LONG volatile*>(value), 0);
        }

        inline void atomicStore32(LONG volatile* value, LONG newValue, MemoryOrder order)
        {
            if (!needsAtomicLoadStore(order))
            {
                *value = newValue;
                return;
            }
            _InterlockedExchange(value, newValue);
        }

        inline LONG64 atomicLoad64(LONG64 volatile const* value, MemoryOrder order)
        {
            if (!needsAtomicLoadStore(order))
            {
                return *value;
            }
            return _InterlockedOr64(const_cast<LONG64 volatile*>(value), 0);
        }

        inline void atomicStore64(LONG64 volatile* value, LONG64 newValue, MemoryOrder order)
        {
            if (!needsAtomicLoadStore(order))
            {
                *value = newValue;
                return;
            }
            _InterlockedExchange64(value, newValue);
        }

        inline void* atomicLoadPointer(void* const* value, MemoryOrder order)
        {
            if (!needsAtomicLoadStore(order))
            {
                return *value;
            }
#ifdef _WIN64
            return _InterlockedCompareExchangePointer(const_cast<void**>(value), nullptr, nullptr);
#else
            return reinterpret_cast<void*>(atomicLoad32(reinterpret_cast<LONG volatile const*>(value), order));
#endif
        }

        inline void atomicStorePointer(void** value, void* newValue, MemoryOrder order)
        {
            if (!needsAtomicLoadStore(order))
            {
                *value = newValue;
                return;
            }
#ifdef _WIN64
            _InterlockedExchangePointer(value, newValue);
#else
            atomicStore32(reinterpret_cast<LONG volatile*>(value), reinterpret_cast<LONG>(newValue), order);
#endif
        }
    }

    /// Windows atomic implementation for 8-bit types
    template <>
    struct Atomic<int8>
    {
        ARK_STRUCT(Atomic);

    protected:
        alignas(int8) volatile int8 value;

    public:
        Atomic()
            : value(0)
        {
        }

        Atomic(int8 initialValue)
            : value(initialValue)
        {
        }

        int8 load(MemoryOrder order = MemoryOrder::SequentiallyConsistent) const
        {
            return static_cast<int8>(Internal::atomicLoad8(reinterpret_cast<char volatile const*>(&value), order));
        }

        void store(int8 newValue, MemoryOrder order = MemoryOrder::SequentiallyConsistent)
        {
            Internal::atomicStore8(reinterpret_cast<char volatile*>(&value), static_cast<char>(newValue), order);
        }

        int8 exchange(int8 newValue, MemoryOrder order = MemoryOrder::SequentiallyConsistent)
        {
            Internal::memoryBarrier(order);
            return static_cast<int8>(_InterlockedExchange8(reinterpret_cast<volatile char*>(&value), newValue));
        }

        bool compareExchange(int8& expected, int8 desired, MemoryOrder order = MemoryOrder::SequentiallyConsistent)
        {
            int8 prev = static_cast<int8>(_InterlockedCompareExchange8(
                reinterpret_cast<volatile char*>(&value), desired, expected));
            Internal::memoryBarrier(order);

            if (prev == expected)
            {
                return true;
            }
            expected = prev;
            return false;
        }

        int8 fetchAdd(int8 addValue, MemoryOrder order = MemoryOrder::SequentiallyConsistent)
        {
            Internal::memoryBarrier(order);
            return static_cast<int8>(_InterlockedExchangeAdd8(reinterpret_cast<volatile char*>(&value), addValue));
        }

        int8 fetchSubtract(int8 subValue, MemoryOrder order = MemoryOrder::SequentiallyConsistent)
        {
            return fetchAdd(-subValue, order);
        }
    };

    /// Windows atomic implementation for 16-bit types
    template <>
    struct Atomic<int16>
    {
    protected:
        alignas(int16) volatile int16 value;

    public:
        Atomic()
            : value(0)
        {
        }
        Atomic(int16 initialValue)
            : value(initialValue)
        {
        }

        int16 load(MemoryOrder order = MemoryOrder::SequentiallyConsistent) const
        {
            return static_cast<int16>(Internal::atomicLoad16(reinterpret_cast<short volatile const*>(&value), order));
        }

        void store(int16 newValue, MemoryOrder order = MemoryOrder::SequentiallyConsistent)
        {
            Internal::atomicStore16(reinterpret_cast<short volatile*>(&value), static_cast<short>(newValue), order);
        }

        int16 exchange(int16 newValue, MemoryOrder order = MemoryOrder::SequentiallyConsistent)
        {
            Internal::memoryBarrier(order);
            return static_cast<int16>(_InterlockedExchange16(&value, newValue));
        }

        bool compareExchange(int16& expected, int16 desired, MemoryOrder order = MemoryOrder::SequentiallyConsistent)
        {
            int16 prev = static_cast<int16>(_InterlockedCompareExchange16(&value, desired, expected));
            Internal::memoryBarrier(order);

            if (prev == expected)
            {
                return true;
            }
            expected = prev;
            return false;
        }

        int16 fetchAdd(int16 addValue, MemoryOrder order = MemoryOrder::SequentiallyConsistent)
        {
            Internal::memoryBarrier(order);
            return static_cast<int16>(_InterlockedExchangeAdd16(&value, addValue));
        }

        int16 fetchSubtract(int16 subValue, MemoryOrder order = MemoryOrder::SequentiallyConsistent)
        {
            return fetchAdd(-subValue, order);
        }
    };

    /// Windows atomic implementation for 32-bit types
    template <>
    struct Atomic<int32>
    {
    protected:
        alignas(int32) volatile LONG value;

    public:
        Atomic()
            : value(0)
        {
        }
        Atomic(int32 initialValue)
            : value(initialValue)
        {
        }

        int32 load(MemoryOrder order = MemoryOrder::SequentiallyConsistent) const
        {
            return static_cast<int32>(Internal::atomicLoad32(&value, order));
        }

        void store(int32 newValue, MemoryOrder order = MemoryOrder::SequentiallyConsistent)
        {
            Internal::atomicStore32(&value, static_cast<LONG>(newValue), order);
        }

        int32 exchange(int32 newValue, MemoryOrder order = MemoryOrder::SequentiallyConsistent)
        {
            Internal::memoryBarrier(order);
            return static_cast<int32>(_InterlockedExchange(&value, newValue));
        }

        bool compareExchange(int32& expected, int32 desired, MemoryOrder order = MemoryOrder::SequentiallyConsistent)
        {
            int32 prev = static_cast<int32>(_InterlockedCompareExchange(&value, desired, expected));
            Internal::memoryBarrier(order);

            if (prev == expected)
            {
                return true;
            }
            expected = prev;
            return false;
        }

        int32 fetchAdd(int32 addValue, MemoryOrder order = MemoryOrder::SequentiallyConsistent)
        {
            Internal::memoryBarrier(order);
            return static_cast<int32>(_InterlockedExchangeAdd(&value, addValue));
        }

        int32 fetchSubtract(int32 subValue, MemoryOrder order = MemoryOrder::SequentiallyConsistent)
        {
            return fetchAdd(-subValue, order);
        }
    };

    /// Windows atomic implementation for 64-bit types
    template <>
    struct Atomic<int64>
    {
    protected:
        alignas(int64) volatile LONG64 value;

    public:
        Atomic()
            : value(0)
        {
        }
        Atomic(int64 initialValue)
            : value(initialValue)
        {
        }

        int64 load(MemoryOrder order = MemoryOrder::SequentiallyConsistent) const
        {
            return static_cast<int64>(Internal::atomicLoad64(&value, order));
        }

        void store(int64 newValue, MemoryOrder order = MemoryOrder::SequentiallyConsistent)
        {
            Internal::atomicStore64(&value, static_cast<LONG64>(newValue), order);
        }

        int64 exchange(int64 newValue, MemoryOrder order = MemoryOrder::SequentiallyConsistent)
        {
            Internal::memoryBarrier(order);
            return static_cast<int64>(_InterlockedExchange64(&value, newValue));
        }

        bool compareExchange(int64& expected, int64 desired, MemoryOrder order = MemoryOrder::SequentiallyConsistent)
        {
            int64 prev = static_cast<int64>(_InterlockedCompareExchange64(&value, desired, expected));
            Internal::memoryBarrier(order);

            if (prev == expected)
            {
                return true;
            }
            expected = prev;
            return false;
        }

        int64 fetchAdd(int64 addValue, MemoryOrder order = MemoryOrder::SequentiallyConsistent)
        {
            Internal::memoryBarrier(order);
            return static_cast<int64>(_InterlockedExchangeAdd64(&value, addValue));
        }

        int64 fetchSubtract(int64 subValue, MemoryOrder order = MemoryOrder::SequentiallyConsistent)
        {
            return fetchAdd(-subValue, order);
        }
    };

    /// Unsigned type specializations
    template <>
    struct Atomic<uint8> : public Atomic<int8>
    {
        Atomic()
            : Atomic<int8>()
        {
        }
        Atomic(uint8 initialValue)
            : Atomic<int8>(static_cast<int8>(initialValue))
        {
        }

        uint8 load(MemoryOrder order = MemoryOrder::SequentiallyConsistent) const
        {
            return static_cast<uint8>(Atomic<int8>::load(order));
        }

        void store(uint8 newValue, MemoryOrder order = MemoryOrder::SequentiallyConsistent)
        {
            Atomic<int8>::store(static_cast<int8>(newValue), order);
        }

        uint8 exchange(uint8 newValue, MemoryOrder order = MemoryOrder::SequentiallyConsistent)
        {
            return static_cast<uint8>(Atomic<int8>::exchange(static_cast<int8>(newValue), order));
        }

        bool compareExchange(uint8& expected, uint8 desired, MemoryOrder order = MemoryOrder::SequentiallyConsistent)
        {
            int8 exp = static_cast<int8>(expected);
            bool result = Atomic<int8>::compareExchange(exp, static_cast<int8>(desired), order);
            expected = static_cast<uint8>(exp);
            return result;
        }

        uint8 fetchAdd(uint8 addValue, MemoryOrder order = MemoryOrder::SequentiallyConsistent)
        {
            return static_cast<uint8>(Atomic<int8>::fetchAdd(static_cast<int8>(addValue), order));
        }

        uint8 fetchSubtract(uint8 subValue, MemoryOrder order = MemoryOrder::SequentiallyConsistent)
        {
            return static_cast<uint8>(Atomic<int8>::fetchSubtract(static_cast<int8>(subValue), order));
        }
    };

    template <>
    struct Atomic<uint16> : public Atomic<int16>
    {
        Atomic()
            : Atomic<int16>()
        {
        }
        Atomic(uint16 initialValue)
            : Atomic<int16>(static_cast<int16>(initialValue))
        {
        }

        uint16 load(MemoryOrder order = MemoryOrder::SequentiallyConsistent) const
        {
            return static_cast<uint16>(Atomic<int16>::load(order));
        }

        void store(uint16 newValue, MemoryOrder order = MemoryOrder::SequentiallyConsistent)
        {
            Atomic<int16>::store(static_cast<int16>(newValue), order);
        }

        uint16 exchange(uint16 newValue, MemoryOrder order = MemoryOrder::SequentiallyConsistent)
        {
            return static_cast<uint16>(Atomic<int16>::exchange(static_cast<int16>(newValue), order));
        }

        bool compareExchange(uint16& expected, uint16 desired, MemoryOrder order = MemoryOrder::SequentiallyConsistent)
        {
            int16 exp = static_cast<int16>(expected);
            bool result = Atomic<int16>::compareExchange(exp, static_cast<int16>(desired), order);
            expected = static_cast<uint16>(exp);
            return result;
        }

        uint16 fetchAdd(uint16 addValue, MemoryOrder order = MemoryOrder::SequentiallyConsistent)
        {
            return static_cast<uint16>(Atomic<int16>::fetchAdd(static_cast<int16>(addValue), order));
        }

        uint16 fetchSubtract(uint16 subValue, MemoryOrder order = MemoryOrder::SequentiallyConsistent)
        {
            return static_cast<uint16>(Atomic<int16>::fetchSubtract(static_cast<int16>(subValue), order));
        }
    };

    template <>
    struct Atomic<uint32> : public Atomic<int32>
    {
        Atomic()
            : Atomic<int32>()
        {
        }
        Atomic(uint32 initialValue)
            : Atomic<int32>(static_cast<int32>(initialValue))
        {
        }

        uint32 load(MemoryOrder order = MemoryOrder::SequentiallyConsistent) const
        {
            return static_cast<uint32>(Atomic<int32>::load(order));
        }

        void store(uint32 newValue, MemoryOrder order = MemoryOrder::SequentiallyConsistent)
        {
            Atomic<int32>::store(static_cast<int32>(newValue), order);
        }

        uint32 exchange(uint32 newValue, MemoryOrder order = MemoryOrder::SequentiallyConsistent)
        {
            return static_cast<uint32>(Atomic<int32>::exchange(static_cast<int32>(newValue), order));
        }

        bool compareExchange(uint32& expected, uint32 desired, MemoryOrder order = MemoryOrder::SequentiallyConsistent)
        {
            int32 exp = static_cast<int32>(expected);
            bool result = Atomic<int32>::compareExchange(exp, static_cast<int32>(desired), order);
            expected = static_cast<uint32>(exp);
            return result;
        }

        uint32 fetchAdd(uint32 addValue, MemoryOrder order = MemoryOrder::SequentiallyConsistent)
        {
            return static_cast<uint32>(Atomic<int32>::fetchAdd(static_cast<int32>(addValue), order));
        }

        uint32 fetchSubtract(uint32 subValue, MemoryOrder order = MemoryOrder::SequentiallyConsistent)
        {
            return static_cast<uint32>(Atomic<int32>::fetchSubtract(static_cast<int32>(subValue), order));
        }
    };

    template <>
    struct Atomic<uint64> : public Atomic<int64>
    {
        Atomic()
            : Atomic<int64>()
        {
        }
        Atomic(uint64 initialValue)
            : Atomic<int64>(static_cast<int64>(initialValue))
        {
        }

        uint64 load(MemoryOrder order = MemoryOrder::SequentiallyConsistent) const
        {
            return static_cast<uint64>(Atomic<int64>::load(order));
        }

        void store(uint64 newValue, MemoryOrder order = MemoryOrder::SequentiallyConsistent)
        {
            Atomic<int64>::store(static_cast<int64>(newValue), order);
        }

        uint64 exchange(uint64 newValue, MemoryOrder order = MemoryOrder::SequentiallyConsistent)
        {
            return static_cast<uint64>(Atomic<int64>::exchange(static_cast<int64>(newValue), order));
        }

        bool compareExchange(uint64& expected, uint64 desired, MemoryOrder order = MemoryOrder::SequentiallyConsistent)
        {
            int64 exp = static_cast<int64>(expected);
            bool result = Atomic<int64>::compareExchange(exp, static_cast<int64>(desired), order);
            expected = static_cast<uint64>(exp);
            return result;
        }

        uint64 fetchAdd(uint64 addValue, MemoryOrder order = MemoryOrder::SequentiallyConsistent)
        {
            return static_cast<uint64>(Atomic<int64>::fetchAdd(static_cast<int64>(addValue), order));
        }

        uint64 fetchSubtract(uint64 subValue, MemoryOrder order = MemoryOrder::SequentiallyConsistent)
        {
            return static_cast<uint64>(Atomic<int64>::fetchSubtract(static_cast<int64>(subValue), order));
        }
    };

    /// Boolean specialization
    template <>
    struct Atomic<bool>
    {
    protected:
        alignas(int32) volatile LONG value;

    public:
        Atomic()
            : value(0)
        {
        }
        Atomic(bool initialValue)
            : value(initialValue ? 1 : 0)
        {
        }

        bool load(MemoryOrder order = MemoryOrder::SequentiallyConsistent) const
        {
            return Internal::atomicLoad32(&value, order) != 0;
        }

        void store(bool newValue, MemoryOrder order = MemoryOrder::SequentiallyConsistent)
        {
            Internal::atomicStore32(&value, newValue ? 1 : 0, order);
        }

        bool exchange(bool newValue, MemoryOrder order = MemoryOrder::SequentiallyConsistent)
        {
            Internal::memoryBarrier(order);
            LONG prev = _InterlockedExchange(&value, newValue ? 1 : 0);
            return prev != 0;
        }

        bool compareExchange(bool& expected, bool desired, MemoryOrder order = MemoryOrder::SequentiallyConsistent)
        {
            LONG exp = expected ? 1 : 0;
            LONG prev = _InterlockedCompareExchange(&value, desired ? 1 : 0, exp);
            Internal::memoryBarrier(order);

            if (prev == exp)
            {
                return true;
            }
            expected = prev != 0;
            return false;
        }
    };

    /// Pointer specialization
    template <typename T>
    struct Atomic<T*>
    {
    protected:
        alignas(void*) volatile void* value;

    public:
        Atomic()
            : value(nullptr)
        {
        }
        Atomic(T* initialValue)
            : value(initialValue)
        {
        }

        T* load(MemoryOrder order = MemoryOrder::SequentiallyConsistent) const
        {
            return static_cast<T*>(Internal::atomicLoadPointer(const_cast<void**>(const_cast<void* volatile*>(&value)), order));
        }

        void store(T* newValue, MemoryOrder order = MemoryOrder::SequentiallyConsistent)
        {
            Internal::atomicStorePointer(const_cast<void**>(const_cast<void* volatile*>(&value)), newValue, order);
        }

        T* exchange(T* newValue, MemoryOrder order = MemoryOrder::SequentiallyConsistent)
        {
            Internal::memoryBarrier(order);
#ifdef _WIN64
            return static_cast<T*>(_InterlockedExchangePointer(const_cast<void**>(&value), newValue));
#else
            return static_cast<T*>(_InterlockedExchange(
                reinterpret_cast<volatile LONG*>(&value),
                reinterpret_cast<LONG>(newValue)));
#endif
        }

        bool compareExchange(T*& expected, T* desired, MemoryOrder order = MemoryOrder::SequentiallyConsistent)
        {
#ifdef _WIN64
            void* prev = _InterlockedCompareExchangePointer(
                const_cast<void**>(&value), desired, expected);
#else
            void* prev = reinterpret_cast<void*>(_InterlockedCompareExchange(
                reinterpret_cast<volatile LONG*>(&value),
                reinterpret_cast<LONG>(desired),
                reinterpret_cast<LONG>(expected)));
#endif
            Internal::memoryBarrier(order);

            if (prev == expected)
            {
                return true;
            }
            expected = static_cast<T*>(prev);
            return false;
        }
    };

    /// Floating point specializations using compare-exchange loops
    template <>
    struct Atomic<float32>
    {
    protected:
        Atomic<uint32> storage;

    public:
        Atomic()
            : storage(0)
        {
        }
        Atomic(float32 initialValue)
        {
            uint32 bits;
            memcpy(&bits, &initialValue, sizeof(float32));
            storage.store(bits);
        }

        float32 load(MemoryOrder order = MemoryOrder::SequentiallyConsistent) const
        {
            uint32 bits = storage.load(order);
            float32 result;
            memcpy(&result, &bits, sizeof(float32));
            return result;
        }

        void store(float32 newValue, MemoryOrder order = MemoryOrder::SequentiallyConsistent)
        {
            uint32 bits;
            memcpy(&bits, &newValue, sizeof(float32));
            storage.store(bits, order);
        }

        float32 exchange(float32 newValue, MemoryOrder order = MemoryOrder::SequentiallyConsistent)
        {
            uint32 newBits;
            memcpy(&newBits, &newValue, sizeof(float32));
            uint32 oldBits = storage.exchange(newBits, order);
            float32 result;
            memcpy(&result, &oldBits, sizeof(float32));
            return result;
        }

        bool compareExchange(float32& expected, float32 desired, MemoryOrder order = MemoryOrder::SequentiallyConsistent)
        {
            uint32 expectedBits, desiredBits;
            memcpy(&expectedBits, &expected, sizeof(float32));
            memcpy(&desiredBits, &desired, sizeof(float32));

            bool result = storage.compareExchange(expectedBits, desiredBits, order);
            if (!result)
            {
                memcpy(&expected, &expectedBits, sizeof(float32));
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
    protected:
        Atomic<uint64> storage;

    public:
        Atomic()
            : storage(0)
        {
        }

        Atomic(float64 initialValue)
        {
            uint64 bits;
            memcpy(&bits, &initialValue, sizeof(float64));
            storage.store(bits);
        }

        float64 load(MemoryOrder order = MemoryOrder::SequentiallyConsistent) const
        {
            uint64 bits = storage.load(order);
            float64 result;
            memcpy(&result, &bits, sizeof(float64));
            return result;
        }

        void store(float64 newValue, MemoryOrder order = MemoryOrder::SequentiallyConsistent)
        {
            uint64 bits;
            memcpy(&bits, &newValue, sizeof(float64));
            storage.store(bits, order);
        }

        float64 exchange(float64 newValue, MemoryOrder order = MemoryOrder::SequentiallyConsistent)
        {
            uint64 newBits;
            memcpy(&newBits, &newValue, sizeof(float64));
            uint64 oldBits = storage.exchange(newBits, order);
            float64 result;
            memcpy(&result, &oldBits, sizeof(float64));
            return result;
        }

        bool compareExchange(float64& expected, float64 desired, MemoryOrder order = MemoryOrder::SequentiallyConsistent)
        {
            uint64 expectedBits, desiredBits;
            memcpy(&expectedBits, &expected, sizeof(float64));
            memcpy(&desiredBits, &desired, sizeof(float64));

            bool result = storage.compareExchange(expectedBits, desiredBits, order);
            if (!result)
            {
                memcpy(&expected, &expectedBits, sizeof(float64));
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

    /// Memory barrier functions
    inline void compilerBarrier()
    {
        _ReadWriteBarrier();
    }

    inline void memoryBarrierAcquire()
    {
        _ReadBarrier();
    }

    inline void memoryBarrierRelease()
    {
        _WriteBarrier();
    }

    inline void memoryBarrierFull()
    {
        MemoryBarrier();
    }

    inline void cpuPause()
    {
        YieldProcessor();
    }

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
}
