#include "Ark/Testing/Test.hpp"

#include "Ark/Concurrency/Concurrency.hpp"
#include "Ark/System/Time.hpp"

namespace
{
    Ark::uint64 getCurrentTimeMilliseconds()
    {
        auto currentTime = Ark::System::Time::getCurrentTimeMilliseconds();
        REQUIRE(currentTime.isOk());
        return currentTime.getValue();
    }

    struct TrackingLock final
    {
        bool locked{false};
        bool failTryLock{false};
        Ark::int32 id{0};
        Ark::int32* lockOrder{nullptr};
        Ark::int32* lockCount{nullptr};
        Ark::int32* unlockOrder{nullptr};
        Ark::int32* unlockCount{nullptr};
        Ark::int32 lockCalls{0};
        Ark::int32 unlockCalls{0};
        Ark::int32 tryLockCount{0};

        void lock()
        {
            ARK_ASSERT(!locked);
            locked = true;
            ++lockCalls;

            if ((lockOrder != nullptr) && (lockCount != nullptr))
            {
                lockOrder[*lockCount] = id;
                ++(*lockCount);
            }
        }

        bool tryLock()
        {
            ++tryLockCount;
            if (failTryLock || locked)
            {
                return false;
            }

            lock();
            return true;
        }

        void unlock()
        {
            ARK_ASSERT(locked);
            locked = false;
            ++unlockCalls;

            if ((unlockOrder != nullptr) && (unlockCount != nullptr))
            {
                unlockOrder[*unlockCount] = id;
                ++(*unlockCount);
            }
        }
    };

    struct TrackingSharedLock final
    {
        bool locked{false};
        bool failTryLock{false};
        Ark::int32 lockCount{0};
        Ark::int32 unlockCount{0};
        Ark::int32 tryLockCount{0};

        void lockShared()
        {
            ARK_ASSERT(!locked);
            locked = true;
            ++lockCount;
        }

        bool tryLockShared()
        {
            ++tryLockCount;
            if (failTryLock || locked)
            {
                return false;
            }

            locked = true;
            ++lockCount;
            return true;
        }

        void unlockShared()
        {
            ARK_ASSERT(locked);
            locked = false;
            ++unlockCount;
        }
    };

    struct LatchPayload final
    {
        Ark::Concurrency::Latch* latch;
        Ark::Concurrency::Atomic<Ark::uint32>* counter;
    };

    Ark::int32 runLatchWorker(void* userData)
    {
        auto* payload = static_cast<LatchPayload*>(userData);
        payload->counter->fetchAdd(1);
        payload->latch->countDown();
        return 0;
    }

    struct BarrierPayload final
    {
        Ark::Concurrency::Barrier* barrier;
        Ark::Concurrency::Atomic<Ark::uint32>* arrivalCount;
        Ark::Concurrency::Atomic<Ark::uint32>* leaderCount;
        Ark::uint32 phaseCount;
    };

    Ark::int32 runBarrierWorker(void* userData)
    {
        auto* payload = static_cast<BarrierPayload*>(userData);

        for (Ark::uint32 phase = 0; phase < payload->phaseCount; ++phase)
        {
            payload->arrivalCount->fetchAdd(1);
            if (payload->barrier->wait())
            {
                payload->leaderCount->fetchAdd(1);
            }
        }

        return 0;
    }

    struct ConditionVariablePayload final
    {
        Ark::Concurrency::Mutex* mutex;
        Ark::Concurrency::ConditionVariable* condition;
        bool* ready;
        Ark::uint32 delayMs;
        bool updateReady;
    };

    Ark::int32 runConditionVariableNotifier(void* userData)
    {
        auto* payload = static_cast<ConditionVariablePayload*>(userData);
        Ark::System::Time::delay(payload->delayMs);

        Ark::Concurrency::UniqueLock<Ark::Concurrency::Mutex> lock(*payload->mutex);
        if (payload->updateReady)
        {
            *payload->ready = true;
        }
        payload->condition->notifyOne();
        return 0;
    }

    struct ConditionVariableAnyPayload final
    {
        Ark::Concurrency::RecursiveMutex* mutex;
        Ark::Concurrency::ConditionVariableAny* condition;
        bool* ready;
        Ark::uint32 delayMs;
        bool updateReady;
    };

    Ark::int32 runConditionVariableAnyNotifier(void* userData)
    {
        auto* payload = static_cast<ConditionVariableAnyPayload*>(userData);
        Ark::System::Time::delay(payload->delayMs);

        Ark::Concurrency::UniqueLock<Ark::Concurrency::RecursiveMutex> lock(*payload->mutex);
        if (payload->updateReady)
        {
            *payload->ready = true;
        }
        payload->condition->notifyOne();
        return 0;
    }
}

ARK_TEST_CASE("Concurrency", "[concurrency]")
{
    using Ark::Concurrency::adoptLock;
    using Ark::Concurrency::Atomic;
    using Ark::Concurrency::Barrier;
    using Ark::Concurrency::ConditionVariable;
    using Ark::Concurrency::ConditionVariableAny;
    using Ark::Concurrency::deferLock;
    using Ark::Concurrency::Latch;
    using Ark::Concurrency::lock;
    using Ark::Concurrency::LockGuard;
    using Ark::Concurrency::makeScopedLock;
    using Ark::Concurrency::Mutex;
    using Ark::Concurrency::parallelFor;
    using Ark::Concurrency::parallelReduce;
    using Ark::Concurrency::RecursiveMutex;
    using Ark::Concurrency::ScopedLock;
    using Ark::Concurrency::SharedLock;
    using Ark::Concurrency::SharedMutex;
    using Ark::Concurrency::Thread;
    using Ark::Concurrency::ThreadPool;
    using Ark::Concurrency::tryLock;
    using Ark::Concurrency::tryToLock;
    using Ark::Concurrency::UniqueLock;
    using Ark::Concurrency::withLock;
    using Ark::Concurrency::withSharedLock;

    SECTION("lock helpers support empty argument lists")
    {
        lock();
        REQUIRE(tryLock() == -1);
    }

    SECTION("LockGuard unlocks on scope exit and supports adoptLock")
    {
        Mutex mutex;

        {
            LockGuard<Mutex> guard(mutex);
            REQUIRE(!mutex.tryLock());
        }

        REQUIRE(mutex.tryLock());
        mutex.unlock();

        mutex.lock();
        {
            LockGuard<Mutex> guard(mutex, adoptLock);
            REQUIRE(!mutex.tryLock());
        }

        REQUIRE(mutex.tryLock());
        mutex.unlock();
    }

    SECTION("UniqueLock matches std-style ownership operations")
    {
        Mutex firstMutex;
        Mutex secondMutex;

        UniqueLock<Mutex> empty;
        REQUIRE(empty.mutex() == nullptr);
        REQUIRE(!empty.ownsLock());

        UniqueLock<Mutex> deferred(firstMutex, deferLock);
        REQUIRE(deferred.mutex() == &firstMutex);
        REQUIRE(!deferred.ownsLock());
        deferred.lock();
        REQUIRE(deferred.ownsLock());
        REQUIRE(!firstMutex.tryLock());
        deferred.unlock();
        REQUIRE(firstMutex.tryLock());
        firstMutex.unlock();

        firstMutex.lock();
        UniqueLock<Mutex> attempted(firstMutex, tryToLock);
        REQUIRE(!attempted.ownsLock());
        firstMutex.unlock();

        firstMutex.lock();
        {
            UniqueLock<Mutex> adopted(firstMutex, adoptLock);
            REQUIRE(adopted.ownsLock());
            Mutex* released = adopted.release();
            REQUIRE(released == &firstMutex);
            REQUIRE(!adopted.ownsLock());
            released->unlock();
        }

        UniqueLock<Mutex> movedSource(firstMutex);
        UniqueLock<Mutex> movedTarget(Ark::move(movedSource));
        REQUIRE(!movedSource.ownsLock());
        REQUIRE(movedTarget.ownsLock());
        REQUIRE(movedTarget.mutex() == &firstMutex);
        movedTarget.unlock();

        UniqueLock<Mutex> deferredSwap(firstMutex, deferLock);
        UniqueLock<Mutex> lockedSwap(secondMutex);
        deferredSwap.swap(lockedSwap);
        REQUIRE(deferredSwap.mutex() == &secondMutex);
        REQUIRE(deferredSwap.ownsLock());
        REQUIRE(lockedSwap.mutex() == &firstMutex);
        REQUIRE(!lockedSwap.ownsLock());
        deferredSwap.unlock();
    }

    SECTION("SharedLock matches std-style shared ownership operations")
    {
        SharedMutex mutex;

        SharedLock<SharedMutex> empty;
        REQUIRE(empty.mutex() == nullptr);
        REQUIRE(!empty.ownsLock());

        SharedLock<SharedMutex> deferred(mutex, deferLock);
        REQUIRE(deferred.mutex() == &mutex);
        REQUIRE(!deferred.ownsLock());
        deferred.lock();
        REQUIRE(deferred.ownsLock());
        REQUIRE(!mutex.tryLock());
        deferred.unlock();
        REQUIRE(mutex.tryLockShared());
        mutex.unlockShared();

        mutex.lock();
        SharedLock<SharedMutex> attempted(mutex, tryToLock);
        REQUIRE(!attempted.ownsLock());
        mutex.unlock();

        mutex.lockShared();
        {
            SharedLock<SharedMutex> adopted(mutex, adoptLock);
            REQUIRE(adopted.ownsLock());
            SharedMutex* released = adopted.release();
            REQUIRE(released == &mutex);
            REQUIRE(!adopted.ownsLock());
            released->unlockShared();
        }

        SharedLock<SharedMutex> movedSource(mutex);
        SharedLock<SharedMutex> movedTarget(Ark::move(movedSource));
        REQUIRE(!movedSource.ownsLock());
        REQUIRE(movedTarget.ownsLock());
        REQUIRE(movedTarget.mutex() == &mutex);
        movedTarget.unlock();

        SharedMutex otherMutex;
        SharedLock<SharedMutex> deferredSwap(mutex, deferLock);
        SharedLock<SharedMutex> lockedSwap(otherMutex);
        deferredSwap.swap(lockedSwap);
        REQUIRE(deferredSwap.mutex() == &otherMutex);
        REQUIRE(deferredSwap.ownsLock());
        REQUIRE(lockedSwap.mutex() == &mutex);
        REQUIRE(!lockedSwap.ownsLock());
        deferredSwap.unlock();
    }

    SECTION("lock and tryLock follow std-style multi-lock behavior")
    {
        Ark::int32 lockCount = 0;
        Ark::int32 unlockCount = 0;
        Ark::int32 lockOrder[2]{};
        Ark::int32 unlockOrder[2]{};
        TrackingLock first{false, false, 1, lockOrder, &lockCount, unlockOrder, &unlockCount};
        TrackingLock second{false, false, 2, lockOrder, &lockCount, unlockOrder, &unlockCount};
        Ark::int32 expectedFirstId = reinterpret_cast<Ark::uintptr>(&first) < reinterpret_cast<Ark::uintptr>(&second) ? 1 : 2;
        Ark::int32 expectedSecondId = expectedFirstId == 1 ? 2 : 1;

        lock(second, first);
        REQUIRE(first.locked);
        REQUIRE(second.locked);
        REQUIRE(lockOrder[0] == expectedFirstId);
        REQUIRE(lockOrder[1] == expectedSecondId);
        second.unlock();
        first.unlock();

        lockCount = 0;
        if (expectedFirstId == 1)
        {
            second.failTryLock = true;
            REQUIRE(tryLock(first, second) == 1);
            REQUIRE(first.unlockCalls == 2);
            REQUIRE(second.unlockCalls == 1);
        }
        else
        {
            first.failTryLock = true;
            REQUIRE(tryLock(first, second) == 0);
            REQUIRE(first.unlockCalls == 1);
            REQUIRE(second.unlockCalls == 2);
        }

        REQUIRE(!first.locked);
        REQUIRE(!second.locked);
        REQUIRE(first.tryLockCount >= 1);
        REQUIRE(second.tryLockCount >= 1);
    }

    SECTION("ScopedLock and makeScopedLock release all locks on scope exit")
    {
        Ark::int32 lockCount = 0;
        Ark::int32 unlockCount = 0;
        Ark::int32 lockOrder[2]{};
        Ark::int32 unlockOrder[2]{};
        TrackingLock first{false, false, 1, lockOrder, &lockCount, unlockOrder, &unlockCount};
        TrackingLock second{false, false, 2, lockOrder, &lockCount, unlockOrder, &unlockCount};
        Ark::int32 expectedFirstId = reinterpret_cast<Ark::uintptr>(&first) < reinterpret_cast<Ark::uintptr>(&second) ? 1 : 2;
        Ark::int32 expectedSecondId = expectedFirstId == 1 ? 2 : 1;

        {
            ScopedLock<TrackingLock, TrackingLock> guard(second, first);
            REQUIRE(first.locked);
            REQUIRE(second.locked);
            REQUIRE(lockOrder[0] == expectedFirstId);
            REQUIRE(lockOrder[1] == expectedSecondId);
        }

        REQUIRE(!first.locked);
        REQUIRE(!second.locked);
        REQUIRE(unlockOrder[0] == expectedSecondId);
        REQUIRE(unlockOrder[1] == expectedFirstId);

        lockCount = 0;
        unlockCount = 0;
        lockOrder[0] = 0;
        lockOrder[1] = 0;
        unlockOrder[0] = 0;
        unlockOrder[1] = 0;

        {
            auto guard = makeScopedLock(first, second);
            REQUIRE(first.locked);
            REQUIRE(second.locked);
        }

        REQUIRE(!first.locked);
        REQUIRE(!second.locked);
        REQUIRE(lockCount == 2);
        REQUIRE(unlockCount == 2);
    }

    SECTION("withLock and withSharedLock execute callbacks while holding the lock")
    {
        Mutex mutex;
        SharedMutex sharedMutex;
        Ark::int32 value = 0;

        Ark::int32 result = withLock(mutex, [&]()
                                     {
                                         value = 7;
                                         return value + 3;
                                     });
        REQUIRE(result == 10);
        REQUIRE(value == 7);
        REQUIRE(mutex.tryLock());
        mutex.unlock();

        Ark::int32 sharedResult = withSharedLock(sharedMutex, [&]()
                                                 {
                                                     return 42;
                                                 });
        REQUIRE(sharedResult == 42);
        REQUIRE(sharedMutex.tryLockShared());
        sharedMutex.unlockShared();

        TrackingSharedLock trackingShared;
        withSharedLock(trackingShared, [&]()
                       {
                           REQUIRE(trackingShared.locked);
                       });
        REQUIRE(!trackingShared.locked);
        REQUIRE(trackingShared.lockCount == 1);
        REQUIRE(trackingShared.unlockCount == 1);
    }

    SECTION("ConditionVariable waits through UniqueLock and duration overloads")
    {
        Mutex mutex;
        ConditionVariable condition;
        bool ready = false;

        ConditionVariablePayload payload{&mutex, &condition, &ready, 20, true};
        Thread notifier = Thread::create(runConditionVariableNotifier, &payload);

        UniqueLock<Mutex> lock(mutex);
        bool const isReady = condition.waitFor(lock, Ark::Duration::fromMilliseconds(250), [&ready]()
                                               {
                                                   return ready;
                                               });

        REQUIRE(isReady);
        REQUIRE(ready);
        REQUIRE(lock.ownsLock());
        lock.unlock();

        notifier.join(nullptr);
    }

    SECTION("ConditionVariable predicate waitFor honors the original deadline across wakeups")
    {
        Mutex mutex;
        ConditionVariable condition;
        bool ready = false;

        ConditionVariablePayload payload{&mutex, &condition, &ready, 20, false};
        Thread notifier = Thread::create(runConditionVariableNotifier, &payload);

        UniqueLock<Mutex> lock(mutex);
        Ark::uint64 const startMs = getCurrentTimeMilliseconds();
        bool const isReady = condition.waitFor(lock, Ark::Duration::fromMilliseconds(80), [&ready]()
                                               {
                                                   return ready;
                                               });
        Ark::uint64 const elapsedMs = getCurrentTimeMilliseconds() - startMs;

        REQUIRE(!isReady);
        REQUIRE(elapsedMs >= 60);
        REQUIRE(elapsedMs < 95);
        REQUIRE(lock.ownsLock());
        lock.unlock();

        notifier.join(nullptr);
    }

    SECTION("ConditionVariableAny waits through generic lock types")
    {
        RecursiveMutex mutex;
        ConditionVariableAny condition;
        bool ready = false;

        ConditionVariableAnyPayload payload{&mutex, &condition, &ready, 20, true};
        Thread notifier = Thread::create(runConditionVariableAnyNotifier, &payload);

        UniqueLock<RecursiveMutex> lock(mutex);
        bool const isReady = condition.waitFor(lock, Ark::Duration::fromMilliseconds(250), [&ready]()
                                               {
                                                   return ready;
                                               });

        REQUIRE(isReady);
        REQUIRE(ready);
        REQUIRE(lock.ownsLock());
        lock.unlock();

        notifier.join(nullptr);
    }

    SECTION("ConditionVariableAny predicate waitFor keeps the original timeout")
    {
        RecursiveMutex mutex;
        ConditionVariableAny condition;
        bool ready = false;

        ConditionVariableAnyPayload payload{&mutex, &condition, &ready, 20, false};
        Thread notifier = Thread::create(runConditionVariableAnyNotifier, &payload);

        UniqueLock<RecursiveMutex> lock(mutex);
        Ark::uint64 const startMs = getCurrentTimeMilliseconds();
        bool const isReady = condition.waitFor(lock, Ark::Duration::fromMilliseconds(80), [&ready]()
                                               {
                                                   return ready;
                                               });
        Ark::uint64 const elapsedMs = getCurrentTimeMilliseconds() - startMs;

        REQUIRE(!isReady);
        REQUIRE(elapsedMs >= 60);
        REQUIRE(elapsedMs < 95);
        REQUIRE(lock.ownsLock());
        lock.unlock();

        notifier.join(nullptr);
    }

    SECTION("latch waits until all workers count down")
    {
        Latch latch(2);
        Atomic<Ark::uint32> counter(0);
        LatchPayload payload{&latch, &counter};

        Thread first = Thread::create(runLatchWorker, &payload);
        Thread second = Thread::create(runLatchWorker, &payload);

        latch.wait();

        first.join(nullptr);
        second.join(nullptr);

        REQUIRE(counter.load() == 2);
        REQUIRE(latch.isReady());
    }

    SECTION("barrier is reusable and elects one leader per phase")
    {
        constexpr Ark::uint32 phaseCount = 2;

        Barrier barrier(3);
        Atomic<Ark::uint32> arrivalCount(0);
        Atomic<Ark::uint32> leaderCount(0);
        BarrierPayload payload{&barrier, &arrivalCount, &leaderCount, phaseCount};

        Thread first = Thread::create(runBarrierWorker, &payload);
        Thread second = Thread::create(runBarrierWorker, &payload);

        for (Ark::uint32 phase = 0; phase < phaseCount; ++phase)
        {
            arrivalCount.fetchAdd(1);
            if (barrier.wait())
            {
                leaderCount.fetchAdd(1);
            }
        }

        first.join(nullptr);
        second.join(nullptr);

        REQUIRE(arrivalCount.load() == phaseCount * 3);
        REQUIRE(leaderCount.load() == phaseCount);
    }

    SECTION("thread pool drains submitted work before destruction")
    {
        Atomic<Ark::uint32> executed(0);

        {
            ThreadPool pool(2);

            for (Ark::uint32 index = 0; index < 64; ++index)
            {
                REQUIRE(pool.submit([&executed]()
                                    {
                                        executed.fetchAdd(1);
                                    }));
            }
        }

        REQUIRE(executed.load() == 64);
    }

    SECTION("parallel algorithms complete before returning")
    {
        Atomic<Ark::uint32> visitCount(0);

        parallelFor(0, 4000, [&visitCount](Ark::usize)
                    {
                        visitCount.fetchAdd(1);
                    });

        REQUIRE(visitCount.load() == 4000);

        Ark::usize sum = parallelReduce<Ark::usize>(0, 4000, 0, [](Ark::usize index)
                                                    {
                                                        return index;
                                                    },
                                                    [](Ark::usize left, Ark::usize right)
                                                    {
                                                        return left + right;
                                                    });

        REQUIRE(sum == (3999u * 4000u) / 2u);
    }
}