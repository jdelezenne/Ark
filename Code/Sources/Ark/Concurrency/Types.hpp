#pragma once

#include "Ark/Core/Types.hpp"

namespace Ark::Concurrency
{
    template <typename T>
    struct Atomic;

    template <typename LockType>
    struct LockGuard;

    template <typename LockType>
    struct UniqueLock;

    template <typename SharedLockType>
    struct SharedLock;

    struct Thread;
    struct ConditionVariable;
    struct ConditionVariableAny;

    struct Mutex;
    struct RecursiveMutex;
    struct SharedMutex;

    struct SpinLock;

    using MutexLock = LockGuard<Mutex>;
    using RecursiveLock = LockGuard<RecursiveMutex>;
    using SpinLockGuard = LockGuard<SpinLock>;
    using UniqueMutexLock = UniqueLock<Mutex>;
    using SharedMutexLock = SharedLock<SharedMutex>;

    // Common atomic types
    using AtomicBool = Atomic<bool>;
    using AtomicInt8 = Atomic<int8>;
    using AtomicInt16 = Atomic<int16>;
    using AtomicInt32 = Atomic<int32>;
    using AtomicInt64 = Atomic<int64>;
    using AtomicUInt8 = Atomic<uint8>;
    using AtomicUInt16 = Atomic<uint16>;
    using AtomicUInt32 = Atomic<uint32>;
    using AtomicUInt64 = Atomic<uint64>;
    using AtomicUsize = Atomic<usize>;
    using AtomicPtr = Atomic<void*>;

    /// Unique identifier for a thread
    using ThreadId = uint64;

    /// Thread priority levels
    enum class ThreadPriority
    {
        Low,
        Normal,
        High,
        TimeCritical,
    };

    /// Thread execution state
    enum class ThreadState
    {
        Unknown,
        Running,
        Detached,
        Finished,
    };

    /// Memory ordering for atomic operations
    enum class MemoryOrder
    {
        Relaxed,
        Acquire,
        Release,
        AcquireRelease,
        SequentiallyConsistent,
    };

    /// Execution policy for parallel algorithms
    enum class ExecutionPolicy
    {
        Sequential,
        Parallel,
        ParallelUnsequenced,
    };
}
