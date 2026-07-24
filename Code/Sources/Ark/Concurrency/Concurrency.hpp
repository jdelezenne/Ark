#pragma once

#include "Ark/Collections/Array.hpp"
#include "Ark/Concurrency/Atomic.hpp"
#include "Ark/Concurrency/ConditionVariable.hpp"
#include "Ark/Concurrency/Locks.hpp"
#include "Ark/Concurrency/Mutex.hpp"
#include "Ark/Concurrency/ScopedLock.hpp"
#include "Ark/Concurrency/ThreadPool.hpp"
#include "Ark/Concurrency/Types.hpp"
#include "Ark/Core/Debug/Assert.hpp"
#include "Ark/Core/Function.hpp"
#include "Ark/Core/Macros.hpp"
#include "Ark/Core/Traits.hpp"
#include "Ark/Core/Utilities.hpp"
#include "Ark/Memory/Memory.hpp"

namespace Ark::Concurrency
{
    /// Barrier for synchronizing multiple threads
    struct Barrier final
    {
    public:
        /// Create barrier for specified number of threads
        /// @param threadCount Number of threads that must reach the barrier
        explicit Barrier(uint32 threadCount);
        ~Barrier();

        // Non-copyable, non-movable
        Barrier(Barrier const&) = delete;
        Barrier& operator=(Barrier const&) = delete;
        Barrier(Barrier&&) = delete;
        Barrier& operator=(Barrier&&) = delete;

        /// Wait at the barrier
        /// @return True for one thread (can be used to designate a leader), false for others
        bool wait();

    private:
        Mutex mutex;
        ConditionVariable condition;
        uint32 const threadCount;
        uint32 currentCount;
        uint32 generation;
    };

    /// Single-use synchronization point
    struct Latch final
    {
    public:
        /// Create latch with initial count
        /// @param count Number of times countDown() must be called
        explicit Latch(uint32 count);
        ~Latch() = default;

        // Non-copyable, non-movable
        Latch(Latch const&) = delete;
        Latch& operator=(Latch const&) = delete;
        Latch(Latch&&) = delete;
        Latch& operator=(Latch&&) = delete;

        /// Decrement the counter
        void countDown();

        /// Wait for counter to reach zero
        void wait();

        /// Wait with timeout
        /// @param timeoutMs Timeout in milliseconds
        /// @return True if counter reached zero, false if timeout
        bool waitFor(uint32 timeoutMs);

        /// Check if counter has reached zero
        bool isReady() const;

    private:
        Mutex mutex;
        ConditionVariable condition;
        Atomic<uint32> count;
    };

    /// Get number of hardware threads
    /// @return Number of concurrent threads supported by implementation
    uint32 getHardwareConcurrency();

    // =========================================================================
    // Memory Barriers and CPU Utilities
    // =========================================================================

    /// Insert a compiler barrier
    void compilerBarrier();

    /// Insert an acquire memory barrier
    void memoryBarrierAcquire();

    /// Insert a release memory barrier
    void memoryBarrierRelease();

    /// Insert a full memory barrier
    void memoryBarrierFull();

    /// CPU pause instruction for spin loops
    void cpuPause();

    // =========================================================================
    // Parallel Algorithms
    // =========================================================================

    /// Parallel for loop
    /// @param begin Start index (inclusive)
    /// @param end End index (exclusive)
    /// @param func Function to execute for each index
    template <typename Func>
    void parallelFor(usize begin, usize end, Func func);

    /// Parallel for loop with execution policy
    /// @param policy Execution policy
    /// @param begin Start index (inclusive)
    /// @param end End index (exclusive)
    /// @param func Function to execute for each index
    template <typename Func>
    void parallelFor(ExecutionPolicy policy, usize begin, usize end, Func func);

    /// Parallel reduction
    /// @param begin Start index (inclusive)
    /// @param end End index (exclusive)
    /// @param identity Identity value for reduction
    /// @param func Function to get value for each index
    /// @param reduce Function to combine two values
    /// @return Reduced result
    template <typename T, typename GetFunc, typename ReduceFunc>
    T parallelReduce(usize begin, usize end, T identity, GetFunc func, ReduceFunc reduce);
}

namespace Ark::Concurrency
{
    inline Barrier::Barrier(uint32 threadCountValue)
        : threadCount(threadCountValue)
        , currentCount(0)
        , generation(0)
    {
        ARK_ASSERT(threadCountValue > 0);
        if (threadCountValue == 0)
        {
            ARK_FATAL_ERROR_MSG("Barrier threadCount must be greater than zero");
        }
    }

    inline Barrier::~Barrier() = default;

    inline bool Barrier::wait()
    {
        UniqueLock<Mutex> lock(mutex);

        uint32 const currentGeneration = generation;
        ++currentCount;

        if (currentCount == threadCount)
        {
            currentCount = 0;
            ++generation;
            condition.notifyAll();
            return true;
        }

        while (currentGeneration == generation)
        {
            condition.wait(lock);
        }

        return false;
    }

    inline Latch::Latch(uint32 countValue)
        : count(countValue)
    {
    }

    inline void Latch::countDown()
    {
        mutex.lock();

        uint32 const currentCount = count.load(MemoryOrder::Acquire);
        if (currentCount > 0)
        {
            uint32 const nextCount = currentCount - 1;
            count.store(nextCount, MemoryOrder::Release);
            if (nextCount == 0)
            {
                condition.notifyAll();
            }
        }

        mutex.unlock();
    }

    inline void Latch::wait()
    {
        UniqueLock<Mutex> lock(mutex);
        while (count.load(MemoryOrder::Acquire) != 0)
        {
            condition.wait(lock);
        }
    }

    inline bool Latch::waitFor(uint32 timeoutMs)
    {
        UniqueLock<Mutex> lock(mutex);
        return condition.waitFor(lock, timeoutMs, [this]()
                                 {
                                     return count.load(MemoryOrder::Acquire) == 0;
                                 });
    }

    inline bool Latch::isReady() const
    {
        return count.load(MemoryOrder::Acquire) == 0;
    }

    template <typename Func>
    void parallelFor(usize begin, usize end, Func func)
    {
        parallelFor(ExecutionPolicy::Parallel, begin, end, func);
    }

    template <typename Func>
    void parallelFor(ExecutionPolicy policy, usize begin, usize end, Func func)
    {
        if (end <= begin)
        {
            return;
        }

        if (policy == ExecutionPolicy::Sequential || end - begin < 1000)
        {
            // Execute sequentially for small ranges
            for (usize i = begin; i < end; ++i)
            {
                func(i);
            }
            return;
        }

        uint32 threadCount = getHardwareConcurrency();
        if (threadCount == 0)
        {
            threadCount = 4;
        }

        usize const rangeSize = (end - begin + threadCount - 1) / threadCount;
        uint32 taskCount = 0;
        for (uint32 t = 0; t < threadCount; ++t)
        {
            usize const rangeStart = begin + t * rangeSize;
            usize const rangeEnd = Ark::min(rangeStart + rangeSize, end);
            if (rangeStart < rangeEnd)
            {
                ++taskCount;
            }
        }

        if (taskCount == 0)
        {
            return;
        }

        ThreadPool& pool = ThreadPool::getDefault();
        Latch latch(taskCount);

        for (uint32 t = 0; t < threadCount; ++t)
        {
            usize const rangeStart = begin + t * rangeSize;
            usize const rangeEnd = Ark::min(rangeStart + rangeSize, end);

            if (rangeStart < rangeEnd)
            {
                bool const submitted = pool.submit([rangeStart, rangeEnd, func, &latch]()
                                                   {
                                                       for (usize i = rangeStart; i < rangeEnd; ++i)
                                                       {
                                                           func(i);
                                                       }
                                                       latch.countDown();
                                                   });
                ARK_ASSERT(submitted);
            }
        }

        latch.wait();
    }

    template <typename T, typename GetFunc, typename ReduceFunc>
    T parallelReduce(usize begin, usize end, T identity, GetFunc func, ReduceFunc reduce)
    {
        if (end <= begin)
        {
            return identity;
        }

        if (end - begin < 1000)
        {
            // Execute sequentially for small ranges
            T result = identity;
            for (usize i = begin; i < end; ++i)
            {
                result = reduce(result, func(i));
            }
            return result;
        }

        uint32 threadCount = getHardwareConcurrency();
        if (threadCount == 0)
        {
            threadCount = 4;
        }

        Collections::Array<T> partialResults(threadCount, identity);
        usize const rangeSize = (end - begin + threadCount - 1) / threadCount;

        uint32 taskCount = 0;
        for (uint32 t = 0; t < threadCount; ++t)
        {
            usize const rangeStart = begin + t * rangeSize;
            usize const rangeEnd = Ark::min(rangeStart + rangeSize, end);
            if (rangeStart < rangeEnd)
            {
                ++taskCount;
            }
        }

        if (taskCount != 0)
        {
            ThreadPool& pool = ThreadPool::getDefault();
            Latch latch(taskCount);

            for (uint32 t = 0; t < threadCount; ++t)
            {
                usize const rangeStart = begin + t * rangeSize;
                usize const rangeEnd = Ark::min(rangeStart + rangeSize, end);

                if (rangeStart < rangeEnd)
                {
                    bool const submitted = pool.submit([t, rangeStart, rangeEnd, &partialResults, func, reduce, identity, &latch]()
                                                       {
                                                           T localResult = identity;
                                                           for (usize i = rangeStart; i < rangeEnd; ++i)
                                                           {
                                                               localResult = reduce(localResult, func(i));
                                                           }
                                                           partialResults[t] = localResult;
                                                           latch.countDown();
                                                       });
                    ARK_ASSERT(submitted);
                }
            }

            latch.wait();
        }

        T finalResult = identity;
        for (T const& partial : partialResults)
        {
            finalResult = reduce(finalResult, partial);
        }

        return finalResult;
    }
}
