#include "Ark/Testing/Test.hpp"

#include "Ark/Core/Function.hpp"

ARK_TEST_CASE("Function", "[function][functionref]")
{
    SECTION("default and nullptr state")
    {
        Ark::Function<int(int)> f;
        REQUIRE(!f);

        f = nullptr;
        REQUIRE(!f);
    }

    SECTION("stores and invokes callable")
    {
        Ark::Function<int(int)> f = [](int x)
        {
            return x + 1;
        };

        REQUIRE(f);
        REQUIRE(f(41) == 42);
    }

    SECTION("copy and move semantics")
    {
        Ark::Function<int(int)> f = [](int x)
        {
            return x * 2;
        };

        Ark::Function<int(int)> copy = f;
        REQUIRE(copy(21) == 42);

        Ark::Function<int(int)> moved = Ark::move(f);
        REQUIRE(moved(21) == 42);
    }

    SECTION("swap supports small and heap callables")
    {
        struct LargeCapture
        {
            int payload[128]{};

            int operator()(int x) const
            {
                return x + payload[0];
            }
        };

        LargeCapture large;
        large.payload[0] = 5;

        Ark::Function<int(int)> small = [](int x)
        {
            return x + 1;
        };

        Ark::Function<int(int)> heap = large;

        REQUIRE(small(10) == 11);
        REQUIRE(heap(10) == 15);

        small.swap(heap);

        REQUIRE(small(10) == 15);
        REQUIRE(heap(10) == 11);
    }

    SECTION("getTarget without RTTI")
    {
        auto lambda = [](int x)
        {
            return x + 3;
        };

        using LambdaType = decltype(lambda);
        Ark::Function<int(int)> f = lambda;

        REQUIRE(f.template getTarget<LambdaType>() != nullptr);
        REQUIRE(f.template getTarget<int>() == nullptr);
    }

    SECTION("member function pointer construction")
    {
        struct Accumulator
        {
            int base = 0;

            int add(int x)
            {
                return base + x;
            }

            int addConst(int x) const
            {
                return base + x;
            }
        };

        Accumulator a{40};
        Ark::Function<int(int)> f1(&Accumulator::add, &a);
        Ark::Function<int(int)> f2(&Accumulator::addConst, &a);

        REQUIRE(f1(2) == 42);
        REQUIRE(f2(2) == 42);
    }

    SECTION("FunctionRef references callable object")
    {
        auto lambda = [](int x)
        {
            return x + 10;
        };

        Ark::FunctionRef<int(int)> ref(lambda);
        REQUIRE(ref(32) == 42);
    }

    SECTION("FunctionRef references member function")
    {
        struct Adder
        {
            int value = 0;
            int apply(int x)
            {
                return value + x;
            }
        };

        Adder adder{40};
        Ark::FunctionRef<int(int)> ref(&Adder::apply, &adder);
        REQUIRE(ref(2) == 42);

        Ark::FunctionRef<int(int)> copied = ref;
        REQUIRE(copied(2) == 42);
        REQUIRE(ref(3) == 43);
    }

    SECTION("FunctionRef references Function")
    {
        Ark::Function<int(int)> f = [](int x)
        {
            return x + 1;
        };

        Ark::FunctionRef<int(int)> ref(f);
        REQUIRE(ref(41) == 42);
    }
}
