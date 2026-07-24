#include "Ark/Concurrency/ThreadPool.hpp"
#include "Ark/Concurrency/Concurrency.hpp"
#include "Ark/Core/Debug/Assert.hpp"
#include "Ark/Core/Macros.hpp"
#include "Ark/System/CpuInfo.hpp"

namespace Ark::Concurrency
{
    uint32 getHardwareConcurrency()
    {
        int32 count = System::CpuInfo::getNumLogicalCpuCores();
        return count > 0 ? static_cast<uint32>(count) : 4;
    }

    ThreadPool& ThreadPool::getDefault()
    {
        static ThreadPool pool(0);
        return pool;
    }

    WorkQueue::WorkQueue() = default;

    WorkQueue::~WorkQueue()
    {
        finalize();
    }

    bool WorkQueue::enqueue(WorkItem item)
    {
        mutex.lock();
        if (isShutdown)
        {
            mutex.unlock();
            return false;
        }

        queue.append(Ark::move(item));
        condition.notifyOne();
        mutex.unlock();
        return true;
    }

    bool WorkQueue::tryDequeue(WorkItem& item)
    {
        mutex.lock();
        if (queue.isEmpty())
        {
            mutex.unlock();
            return false;
        }
        item = Ark::move(queue.getFirst());
        queue.removeAt(queue.getStartIterator());
        mutex.unlock();
        return true;
    }

    bool WorkQueue::waitDequeue(WorkItem& item)
    {
        UniqueLock<Mutex> lock(mutex);
        while (queue.isEmpty() && !isShutdown)
        {
            condition.wait(lock);
        }

        if (queue.isEmpty())
        {
            return false;
        }

        item = Ark::move(queue.getFirst());
        queue.removeAt(queue.getStartIterator());
        return true;
    }

    usize WorkQueue::size() const
    {
        mutex.lock();
        usize s = queue.getCount();
        mutex.unlock();
        return s;
    }

    bool WorkQueue::isEmpty() const
    {
        mutex.lock();
        bool e = queue.isEmpty();
        mutex.unlock();
        return e;
    }

    void WorkQueue::finalize()
    {
        mutex.lock();
        isShutdown = true;
        condition.notifyAll();
        mutex.unlock();
    }

    ThreadPool::ThreadPool(uint32 desiredThreadCount)
        : threadCount(desiredThreadCount == 0 ? getHardwareConcurrency() : desiredThreadCount)
    {
        if (threadCount == 0)
        {
            threadCount = 4;
        }

        auto workerEntry = Ark::Function<int32(void*)>([](void* userData) -> int32
                                                       {
                                                           ThreadPool* pool = static_cast<ThreadPool*>(userData);
                                                           pool->workerLoop();
                                                           return 0;
                                                       });

        workers.reserve(threadCount);
        for (uint32 i = 0; i < threadCount; ++i)
        {
            auto platformThread = Platform::Thread::create(workerEntry, this);
            if (!platformThread.isValid())
            {
                continue;
            }

            Thread thread;
            static_cast<Platform::Thread&>(thread) = Ark::move(platformThread);
            workers.append(Ark::move(thread));
        }

        ARK_ASSERT(!workers.isEmpty());
        threadCount = static_cast<uint32>(workers.getCount());

        if (threadCount == 0)
        {
            isShutdown = true;
            workQueue.finalize();
            ARK_FATAL_ERROR_MSG("ThreadPool failed to create worker threads");
        }
    }

    ThreadPool::~ThreadPool()
    {
        finalize();
        for (auto& t : workers)
        {
            t.join(nullptr);
        }
    }

    void ThreadPool::finalize()
    {
        isShutdown = true;
        workQueue.finalize();
    }

    void ThreadPool::workerLoop()
    {
        WorkItem item;
        while (workQueue.waitDequeue(item))
        {
            if (item.isValid())
            {
                item.execute();
            }
        }
    }
}
