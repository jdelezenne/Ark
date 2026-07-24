#include "Ark/System/Timer.hpp"

#include "Ark/Collections/Array.hpp"
#include "Ark/Concurrency/Atomic.hpp"
#include "Ark/Concurrency/ConditionVariable.hpp"
#include "Ark/Concurrency/Locks.hpp"
#include "Ark/Concurrency/Mutex.hpp"
#include "Ark/Concurrency/Thread.hpp"
#include "Ark/System/Time.hpp"

namespace Ark::System::Timer
{
    namespace
    {
        struct TimerEntry
        {
            TimerId id = 0;
            TimerCallback callback;
            uint64 nextFireMs = 0;
            uint32 intervalMs = 0;
            bool canceled = false;
        };

        class Manager
        {
        private:
            Concurrency::Mutex mutex;
            Concurrency::ConditionVariable cv;
            Collections::Array<TimerEntry> timers;
            Concurrency::AtomicUInt64 nextId{1};
            Concurrency::Thread worker;
            Concurrency::AtomicBool running{false};

            static uint64 nowMs()
            {
                return System::Time::getCurrentTimeMilliseconds().getValueOr(0);
            }

            static int32 workerEntry(void* userData)
            {
                static_cast<Manager*>(userData)->run();
                return 0;
            }

            void run()
            {
                Concurrency::UniqueLock<Concurrency::Mutex> lock(mutex);
                while (running.load(Concurrency::MemoryOrder::Acquire))
                {
                    for (isize i = static_cast<isize>(timers.getCount()) - 1; i >= 0; --i)
                    {
                        if (timers[static_cast<usize>(i)].canceled)
                        {
                            timers.removeAt(static_cast<usize>(i));
                        }
                    }

                    if (timers.isEmpty())
                    {
                        cv.wait(lock);
                        continue;
                    }

                    uint64 const now = nowMs();
                    usize earliestIndex = 0;
                    for (usize i = 1; i < timers.getCount(); ++i)
                    {
                        if (timers[i].nextFireMs < timers[earliestIndex].nextFireMs)
                        {
                            earliestIndex = i;
                        }
                    }

                    TimerEntry& earliest = timers[earliestIndex];
                    if (earliest.nextFireMs > now)
                    {
                        uint64 const remaining = earliest.nextFireMs - now;
                        uint32 timeout = remaining > static_cast<uint64>(~uint32{0})
                            ? ~uint32{0}
                            : static_cast<uint32>(remaining);
                        cv.waitFor(lock, timeout);
                        continue;
                    }

                    TimerEntry fired = earliest;
                    lock.unlock();
                    uint32 nextIntervalMs = fired.callback ? fired.callback(fired.intervalMs) : 0;
                    lock.lock();

                    for (usize i = 0; i < timers.getCount(); ++i)
                    {
                        if (timers[i].id != fired.id)
                        {
                            continue;
                        }

                        if (nextIntervalMs == 0 || timers[i].canceled)
                        {
                            timers[i].canceled = true;
                        }
                        else
                        {
                            timers[i].intervalMs = nextIntervalMs;
                            timers[i].nextFireMs = nowMs() + nextIntervalMs;
                        }
                        break;
                    }
                }
            }

        public:
            ~Manager()
            {
                stop();
            }

            void start()
            {
                if (running.load(Concurrency::MemoryOrder::Acquire))
                {
                    return;
                }

                running.store(true, Concurrency::MemoryOrder::Release);
                worker = Concurrency::Thread::create(&Manager::workerEntry, this);
            }

            void stop()
            {
                if (!running.load(Concurrency::MemoryOrder::Acquire))
                {
                    return;
                }

                running.store(false, Concurrency::MemoryOrder::Release);
                cv.notifyAll();
                if (worker.isValid())
                {
                    worker.join(nullptr);
                }

                Concurrency::LockGuard<Concurrency::Mutex> lock(mutex);
                timers.clear();
            }

            TimerId add(uint32 intervalMs, TimerCallback callback)
            {
                if (intervalMs == 0 || !callback)
                {
                    return 0;
                }

                start();

                Concurrency::LockGuard<Concurrency::Mutex> lock(mutex);
                TimerId id = nextId.fetchAdd(1, Concurrency::MemoryOrder::AcquireRelease);
                TimerEntry entry{};
                entry.id = id;
                entry.callback = Ark::move(callback);
                entry.intervalMs = intervalMs;
                entry.nextFireMs = nowMs() + intervalMs;
                timers.append(Ark::move(entry));
                cv.notifyAll();
                return id;
            }

            bool remove(TimerId id)
            {
                Concurrency::LockGuard<Concurrency::Mutex> lock(mutex);
                for (usize i = 0; i < timers.getCount(); ++i)
                {
                    if (timers[i].id == id)
                    {
                        timers[i].canceled = true;
                        cv.notifyAll();
                        return true;
                    }
                }
                return false;
            }
        };

        Manager& getManager()
        {
            static Manager instance;
            return instance;
        }
    }

    TimerId addTimer(uint32 intervalMs, TimerCallback callback)
    {
        return getManager().add(intervalMs, Ark::move(callback));
    }

    bool removeTimer(TimerId id)
    {
        return getManager().remove(id);
    }
}
