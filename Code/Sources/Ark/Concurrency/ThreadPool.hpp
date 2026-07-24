#pragma once

#include "Ark/Collections/Array.hpp"
#include "Ark/Concurrency/ConditionVariable.hpp"
#include "Ark/Concurrency/Mutex.hpp"
#include "Ark/Concurrency/Thread.hpp"
#include "Ark/Concurrency/Types.hpp"
#include "Ark/Core/Function.hpp"

namespace Ark::Concurrency
{

    /// Work item for thread pool
    struct WorkItem final
    {
    public:
        using WorkFunction = Function<void()>;

        WorkItem() = default;
        explicit WorkItem(WorkFunction work)
            : workFunction(Ark::move(work))
        {
        }

        void execute()
        {
            if (workFunction)
                workFunction();
        }

        bool isValid() const
        {
            return static_cast<bool>(workFunction);
        }

    private:
        WorkFunction workFunction;
    };

    /// Thread-safe work queue
    struct WorkQueue final
    {
    public:
        WorkQueue();
        ~WorkQueue();

        /// Add work item to the queue
        /// @param item Work item to add
        /// @return True when the item was accepted, false when the queue is finalized
        bool enqueue(WorkItem item);

        /// Try to dequeue a work item
        /// @param item Reference to receive the work item
        /// @return True if item was dequeued, false if queue is empty
        bool tryDequeue(WorkItem& item);

        /// Block until work item is available
        /// @param item Reference to receive the work item
        /// @return True if item was dequeued, false if queue is being destroyed
        bool waitDequeue(WorkItem& item);

        /// Get current queue size
        usize size() const;

        /// Check if queue is empty
        bool isEmpty() const;

        /// Signal finalize (wakes up waiting threads)
        void finalize();

    private:
        mutable Mutex mutex;
        ConditionVariable condition;
        Collections::Array<WorkItem> queue;
        bool isShutdown = false;
    };

    /// Thread pool for parallel execution
    struct ThreadPool final
    {
    public:
        /// Create thread pool with specified number of threads
        /// @param threadCount Number of worker threads (0 = hardware concurrency)
        explicit ThreadPool(uint32 threadCount = 0);
        ~ThreadPool();

        // Non-copyable, non-movable
        ThreadPool(ThreadPool const&) = delete;
        ThreadPool& operator=(ThreadPool const&) = delete;
        ThreadPool(ThreadPool&&) = delete;
        ThreadPool& operator=(ThreadPool&&) = delete;

        /// Submit work to the thread pool
        /// @param work Function to execute
        /// @return True when work was accepted, false when the pool is finalized
        template <typename F>
        bool submit(F&& work)
        {
            return workQueue.enqueue(WorkItem(Function<void()>(Ark::forward<F>(work))));
        }

        /// Get number of worker threads
        uint32 getThreadCount() const
        {
            return threadCount;
        }

        /// Get number of pending work items
        usize getPendingWork() const
        {
            return workQueue.size();
        }

        /// Shutdown the thread pool
        void finalize();

        /// Returns true after finalize() has been requested
        bool isFinalized() const
        {
            return isShutdown;
        }

        /// Shared process-wide pool used by parallel algorithms.
        static ThreadPool& getDefault();

    private:
        Collections::Array<Thread> workers;
        WorkQueue workQueue;
        uint32 threadCount;
        bool isShutdown = false;

        void workerLoop();
    };
}
