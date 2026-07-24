#include "Ark/Testing/Test.hpp"

#include "Ark/Memory/OwnedReference.hpp"
#include "Ark/Memory/SharedPointer.hpp"
#include "Ark/Memory/UniquePointer.hpp"

namespace
{
    struct Counter final
    {
        static inline Ark::int32 constructions = 0;
        static inline Ark::int32 destructions = 0;

        Ark::int32 value = 0;

        explicit Counter(Ark::int32 v)
            : value(v)
        {
            ++constructions;
        }

        ~Counter()
        {
            ++destructions;
        }

        static void reset()
        {
            constructions = 0;
            destructions = 0;
        }
    };

    struct Pair final
    {
        static inline Ark::int32 destructions = 0;

        Ark::int32 first = 0;
        Ark::int32 second = 0;

        Pair(Ark::int32 a, Ark::int32 b)
            : first(a)
            , second(b)
        {
        }

        ~Pair()
        {
            ++destructions;
        }

        static void reset()
        {
            destructions = 0;
        }
    };

    struct UniqueCounterDeleter final
    {
        static inline Ark::int32 calls = 0;

        void operator()(Counter* pointer) const
        {
            ++calls;
            delete pointer;
        }

        static void reset()
        {
            calls = 0;
        }
    };

    struct SharedCounterDeleter final
    {
        static inline Ark::int32 calls = 0;

        void operator()(Counter* pointer) const
        {
            ++calls;
            delete pointer;
        }

        static void reset()
        {
            calls = 0;
        }
    };
}

ARK_TEST_CASE("SmartPointers UniquePointer basic ownership", "[memory][smartptr][unique]")
{
    Counter::reset();

    {
        Ark::UniquePointer<Counter> ptr = Ark::makeUnique<Counter>(7);
        REQUIRE(ptr);
        REQUIRE(ptr->value == 7);

        Ark::UniquePointer<Counter> moved = Ark::move(ptr);
        REQUIRE(!ptr);
        REQUIRE(moved);
        REQUIRE(moved->value == 7);

        moved.reset();
        REQUIRE(!moved);
    }

    REQUIRE(Counter::constructions == 1);
    REQUIRE(Counter::destructions == 1);
}

ARK_TEST_CASE("SmartPointers SharedPointer weak promotion", "[memory][smartptr][shared]")
{
    Ark::WeakPointer<Counter> weak;

    {
        Ark::SharedPointer<Counter> shared = Ark::makeShared<Counter>(42);
        weak = shared;
        REQUIRE(shared.useCount() == 1);
        REQUIRE(!weak.expired());
    }

    REQUIRE(weak.expired());

    // Promotion from expired weak must yield empty pointer and must not terminate the process.
    Ark::SharedPointer<Counter> promoted{weak};
    REQUIRE(!promoted);

    Ark::SharedPointer<Counter> locked = weak.lock();
    REQUIRE(!locked);
}

ARK_TEST_CASE("SmartPointers custom deleters are invoked once", "[memory][smartptr][deleter]")
{
    Counter::reset();
    UniqueCounterDeleter::reset();
    SharedCounterDeleter::reset();

    {
        Ark::UniquePointer<Counter, UniqueCounterDeleter> unique{new Counter(1)};
        REQUIRE(unique);
    }

    {
        Ark::SharedPointer<Counter> shared{new Counter(2), SharedCounterDeleter{}};
        REQUIRE(shared);
        REQUIRE(shared.useCount() == 1);
    }

    REQUIRE(UniqueCounterDeleter::calls == 1);
    REQUIRE(SharedCounterDeleter::calls == 1);
    REQUIRE(Counter::constructions == 2);
    REQUIRE(Counter::destructions == 2);
}

ARK_TEST_CASE("SmartPointers SharedPointer aliasing keeps owner alive", "[memory][smartptr][shared][alias]")
{
    Pair::reset();

    Ark::SharedPointer<Pair> owner = Ark::makeShared<Pair>(10, 20);
    REQUIRE(owner.useCount() == 1);

    Ark::SharedPointer<Ark::int32> alias{owner, &owner->second};
    REQUIRE(owner.useCount() == 2);
    REQUIRE(alias.useCount() == 2);
    REQUIRE(*alias == 20);

    owner.reset();
    REQUIRE(!owner);
    REQUIRE(alias.useCount() == 1);
    REQUIRE(*alias == 20);
    REQUIRE(Pair::destructions == 0);

    alias.reset();
    REQUIRE(Pair::destructions == 1);
}

ARK_TEST_CASE("SmartPointers OwnedReference release detaches ownership", "[memory][smartptr][owned]")
{
    Counter::reset();

    Ark::OwnedReference<Counter> owned = Ark::makeOwned<Counter>(100);
    Ark::WeakReference<Counter> weak = owned;
    REQUIRE(weak.get() != nullptr);

    Counter* raw = owned.release();
    REQUIRE(raw != nullptr);
    REQUIRE(!owned);

    // After release, weak observers must no longer report a valid object.
    REQUIRE(weak.get() == nullptr);
    REQUIRE(weak.expired());

    delete raw;

    REQUIRE(Counter::constructions == 1);
    REQUIRE(Counter::destructions == 1);
}

ARK_TEST_CASE("SmartPointers AtomicOwnedReference weak expiration", "[memory][smartptr][owned][atomic]")
{
    Counter::reset();

    Ark::AtomicWeakReference<Counter> weak;

    {
        Ark::AtomicOwnedReference<Counter> owned = Ark::makeAtomicOwned<Counter>(55);
        weak = owned;
        REQUIRE(!weak.expired());
        REQUIRE(weak.get() != nullptr);
        REQUIRE(weak.get()->value == 55);
    }

    REQUIRE(weak.expired());
    REQUIRE(weak.get() == nullptr);
    REQUIRE(Counter::constructions == 1);
    REQUIRE(Counter::destructions == 1);
}

ARK_TEST_CASE("SmartPointers SharedPointer relational operators", "[memory][smartptr][shared]")
{
    Ark::SharedPointer<Counter> a = Ark::makeShared<Counter>(1);
    Ark::SharedPointer<Counter> b = a;
    Ark::SharedPointer<Counter> c = Ark::makeShared<Counter>(2);

    REQUIRE(a == b);
    REQUIRE(a <= b);
    REQUIRE(a >= b);
    bool const ordered = (a < c) || (c < a);
    REQUIRE(ordered);
}
