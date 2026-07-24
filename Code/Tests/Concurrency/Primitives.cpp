#include "Ark/Testing/Test.hpp"

#include "Ark/Concurrency/Atomic.hpp"
#include "Ark/Concurrency/Concurrency.hpp"
#include "Ark/Concurrency/Semaphore.hpp"
#include "Ark/Concurrency/SpinLock.hpp"
#include "Ark/Concurrency/ThreadLocal.hpp"

ARK_TEST_CASE("Concurrency primitives", "[concurrency][primitives]")
{
    using Ark::Concurrency::Atomic;
    using Ark::Concurrency::AtomicFlag;
    using Ark::Concurrency::BinarySemaphore;
    using Ark::Concurrency::Mutex;
    using Ark::Concurrency::parallelFor;
    using Ark::Concurrency::parallelReduce;
    using Ark::Concurrency::RecursiveMutex;
    using Ark::Concurrency::Semaphore;
    using Ark::Concurrency::SpinLock;
    using Ark::Concurrency::Thread;
    using Ark::Concurrency::ThreadLocal;

    SECTION("Mutex is non-recursive; RecursiveMutex allows re-entry")
    {
        Mutex mutex;
        REQUIRE(mutex.tryLock());
        REQUIRE(!mutex.tryLock());
        mutex.unlock();

        RecursiveMutex recursive;
        REQUIRE(recursive.tryLock());
        REQUIRE(recursive.tryLock());
        recursive.unlock();
        recursive.unlock();
    }

    SECTION("SpinLock exclusive ownership")
    {
        SpinLock spin;
        REQUIRE(spin.tryLock());
        REQUIRE(!spin.tryLock());
        spin.unlock();
        spin.lock();
        spin.unlock();
    }

    SECTION("Semaphore counting and BinarySemaphore stays binary")
    {
        Semaphore semaphore(0);
        REQUIRE(!semaphore.tryAcquire());
        semaphore.release(2);
        REQUIRE(semaphore.tryAcquire());
        REQUIRE(semaphore.tryAcquire());
        REQUIRE(!semaphore.tryAcquire());

        BinarySemaphore binary(false);
        REQUIRE(binary.tryAcquire());
        REQUIRE(!binary.tryAcquire());
        binary.release();
        binary.release();
        REQUIRE(binary.tryAcquire());
        REQUIRE(!binary.tryAcquire());
    }

    SECTION("AtomicFlag and basic Atomic RMW")
    {
        AtomicFlag flag;
        REQUIRE(!flag.test());
        REQUIRE(!flag.testAndSet());
        REQUIRE(flag.test());
        REQUIRE(flag.testAndSet());
        flag.clear();
        REQUIRE(!flag.test());

        Atomic<Ark::uint32> value(10);
        REQUIRE(value.fetchAdd(5) == 10);
        REQUIRE(value.load() == 15);
        REQUIRE(value.fetchSubtract(3) == 15);
        REQUIRE(value.load() == 12);

        Ark::uint32 expected = 12;
        REQUIRE(value.compareExchange(expected, 20));
        REQUIRE(value.load() == 20);
        expected = 0;
        REQUIRE(!value.compareExchange(expected, 1));
        REQUIRE(expected == 20);
    }

    SECTION("ThreadLocal is per-thread")
    {
        ThreadLocal<Ark::int32> local([]()
                                      {
                                          return 1;
                                      });

        REQUIRE(local.get() == 1);
        local.set(7);
        REQUIRE(*local == 7);

        struct Payload
        {
            ThreadLocal<Ark::int32>* storage;
            Atomic<Ark::int32>* observed;
        };

        Atomic<Ark::int32> observed(0);
        Payload payload{&local, &observed};

        auto workerEntry = +[](void* userData) -> Ark::int32
        {
            auto* data = static_cast<Payload*>(userData);
            data->observed->store(data->storage->get());
            data->storage->set(99);
            return 0;
        };

        Thread worker = Thread::create(workerEntry, &payload);
        worker.join(nullptr);
        REQUIRE(observed.load() == 1);
        REQUIRE(local.get() == 7);
    }

    SECTION("parallelFor and parallelReduce handle empty ranges")
    {
        Atomic<Ark::uint32> visits(0);
        parallelFor(5, 5, [&visits](Ark::usize)
                    {
                        visits.fetchAdd(1);
                    });
        parallelFor(8, 3, [&visits](Ark::usize)
                    {
                        visits.fetchAdd(1);
                    });
        REQUIRE(visits.load() == 0);

        Ark::usize const reduced = parallelReduce<Ark::usize>(4, 1, 42, [](Ark::usize)
                                                              {
                                                                  return 1u;
                                                              },
                                                              [](Ark::usize left, Ark::usize right)
                                                              {
                                                                  return left + right;
                                                              });
        REQUIRE(reduced == 42);
    }

    SECTION("default thread pool is reused across parallelFor calls")
    {
        using Ark::Concurrency::ThreadPool;

        ThreadPool& first = ThreadPool::getDefault();
        ThreadPool& second = ThreadPool::getDefault();
        REQUIRE(&first == &second);

        Atomic<Ark::uint32> visits(0);
        parallelFor(0, 2000, [&visits](Ark::usize)
                    {
                        visits.fetchAdd(1);
                    });
        parallelFor(0, 2000, [&visits](Ark::usize)
                    {
                        visits.fetchAdd(1);
                    });
        REQUIRE(visits.load() == 4000);
    }

    SECTION("thread name and priority APIs are usable")
    {
        using Ark::Concurrency::ThreadPriority;

        REQUIRE(Thread::setCurrentPriority(ThreadPriority::Normal));

        Atomic<Ark::int32> ran(0);
        auto entry = +[](void* userData) -> Ark::int32
        {
            auto* flag = static_cast<Atomic<Ark::int32>*>(userData);
            Thread::setCurrentPriority(ThreadPriority::High);
            flag->store(1);
            return 0;
        };

        Thread worker = Thread::create(entry, Ark::String("ark-worker"), &ran);
        worker.join(nullptr);
        REQUIRE(ran.load() == 1);
    }
}
