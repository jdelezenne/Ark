#include "Ark/Testing/Test.hpp"

#include "Ark/Core/Option.hpp"

ARK_TEST_CASE("Option", "[option]")
{
    SECTION("empty and value state")
    {
        Ark::Option<int> empty{};
        REQUIRE(!empty);
        REQUIRE(empty == Ark::none);
        REQUIRE(empty.getValueOr(42) == 42);

        Ark::Option<int> withValue{123};
        REQUIRE(withValue);
        REQUIRE(withValue != Ark::none);
        REQUIRE(*withValue == 123);
        REQUIRE(withValue.getValue() == 123);
        REQUIRE(withValue.getValueOr(0) == 123);
    }

    SECTION("reset and assignment")
    {
        Ark::Option<int> opt{};
        REQUIRE_FALSE(static_cast<bool>(opt));

        opt.reset();
        REQUIRE(!opt);
        opt = 3;
        REQUIRE(opt);
        REQUIRE(*opt == 3);

        opt = 100;
        REQUIRE(*opt == 100);

        opt = Ark::none;
        REQUIRE(!opt);
    }

    SECTION("comparisons and swap")
    {
        Ark::Option<int> a{5};
        Ark::Option<int> b{Ark::none};
        REQUIRE(a != b);
        REQUIRE(b < a);
        REQUIRE(a > b);

        Ark::swap(a, b);
        REQUIRE(!a);
        REQUIRE(b);
        REQUIRE(*b == 5);
    }

    SECTION("arrow and dereference operators")
    {
        struct Point
        {
            int x, y;
            int sum() const
            {
                return x + y;
            }
        };

        Ark::Option<Point> point{Point{3, 4}};
        REQUIRE(point->x == 3);
        REQUIRE(point->y == 4);
        REQUIRE(point->sum() == 7);
        point->x = 10;
        REQUIRE(point->x == 10);

        Ark::Option<int> opt{25};
        REQUIRE(*opt == 25);
        int& ref = *opt;
        ref = 50;
        REQUIRE(*opt == 50);

        Ark::Option<int> const constOpt{75};
        REQUIRE(*constOpt == 75);
    }

    SECTION("in-place construction")
    {
        struct Data
        {
            int value;
            Data(int v)
                : value(v)
            {
            }
        };

        Ark::Option<Data> opt{Ark::inPlace, 42};
        REQUIRE(opt);
        REQUIRE(opt->value == 42);

        Data& emplaced = opt.emplace(99);
        REQUIRE(emplaced.value == 99);
        REQUIRE(opt->value == 99);
    }

    SECTION("copy and move semantics")
    {
        Ark::Option<int> opt1{100};
        Ark::Option<int> opt2{Ark::move(opt1)};
        REQUIRE(opt2);
        REQUIRE(*opt2 == 100);

        Ark::Option<int> opt3{200};
        Ark::Option<int> opt4;
        opt4 = Ark::move(opt3);
        REQUIRE(opt4);
        REQUIRE(*opt4 == 200);

        Ark::Option<int> copySource{10};
        Ark::Option<int> copyTarget = copySource;
        REQUIRE(copyTarget);
        REQUIRE(*copyTarget == 10);
    }

    SECTION("hasValue and bool conversion")
    {
        Ark::Option<int> empty;
        REQUIRE(!empty.hasValue());
        REQUIRE(!static_cast<bool>(empty));

        Ark::Option<int> filled{42};
        REQUIRE(filled.hasValue());
        REQUIRE(static_cast<bool>(filled));
    }

    SECTION("valueOr on rvalue option")
    {
        Ark::Option<int> empty;
        int result = Ark::move(empty).getValueOr(999);
        REQUIRE(result == 999);

        Ark::Option<int> filled{42};
        result = Ark::move(filled).getValueOr(999);
        REQUIRE(result == 42);
    }

    SECTION("swap scenarios")
    {
        Ark::Option<int> empty;
        Ark::Option<int> filled{100};
        Ark::swap(empty, filled);
        REQUIRE(empty);
        REQUIRE(!filled);
        REQUIRE(*empty == 100);

        Ark::Option<int> opt1{10};
        Ark::Option<int> opt2{20};
        Ark::swap(opt1, opt2);
        REQUIRE(*opt1 == 20);
        REQUIRE(*opt2 == 10);

        Ark::Option<int> e1;
        Ark::Option<int> e2;
        Ark::swap(e1, e2);
        REQUIRE(!e1);
        REQUIRE(!e2);
    }
}

ARK_TEST_CASE("Option reference specialization", "[option][reference]")
{
    SECTION("bind empty and value")
    {
        int value = 10;
        Ark::Option<int&> empty{};
        REQUIRE(!empty.hasValue());
        REQUIRE(empty == Ark::none);

        Ark::Option<int&> bound{value};
        REQUIRE(bound.hasValue());
        REQUIRE(*bound == 10);
        REQUIRE(&bound.getValue() == &value);

        *bound = 20;
        REQUIRE(value == 20);
    }

    SECTION("rebind on assignment")
    {
        int a = 1;
        int b = 2;
        Ark::Option<int&> opt{a};
        REQUIRE(&*opt == &a);

        opt = b;
        REQUIRE(&*opt == &b);
        REQUIRE(*opt == 2);

        opt = Ark::none;
        REQUIRE(!opt);

        opt.emplace(a);
        REQUIRE(&*opt == &a);
    }

    SECTION("const option still allows mutable referred object")
    {
        int value = 5;
        Ark::Option<int&> const opt{value};
        REQUIRE(opt.hasValue());
        *opt = 7;
        REQUIRE(value == 7);
    }

    SECTION("Option of const reference")
    {
        int value = 9;
        Ark::Option<int const&> opt{value};
        REQUIRE(opt.hasValue());
        REQUIRE(*opt == 9);
        REQUIRE(&*opt == &value);
    }

    SECTION("getValueOr and makeOptionRef")
    {
        int value = 11;
        int fallback = 99;

        Ark::Option<int&> empty;
        REQUIRE(&empty.getValueOr(fallback) == &fallback);

        Ark::Option<int&> bound = Ark::makeOptionRef(value);
        REQUIRE(&bound.getValueOr(fallback) == &value);
        REQUIRE(bound.getValueOr(fallback) == 11);
    }

    SECTION("swap rebinds")
    {
        int a = 1;
        int b = 2;
        Ark::Option<int&> left{a};
        Ark::Option<int&> right{b};

        Ark::swap(left, right);
        REQUIRE(&*left == &b);
        REQUIRE(&*right == &a);
    }

    SECTION("comparisons use referred values")
    {
        int a = 1;
        int b = 2;
        Ark::Option<int&> left{a};
        Ark::Option<int&> right{b};
        REQUIRE(left < right);
        REQUIRE(left != right);
        REQUIRE(left == 1);
    }
}

ARK_TEST_CASE("Option monadic operations", "[option][monadic]")
{
    SECTION("andThen transform orElse chain")
    {
        auto parsePositive = [](int value) -> Ark::Option<int> {
            if (value > 0)
            {
                return value;
            }
            return Ark::none;
        };

        Ark::Option<int> start{21};
        auto result = start
                          .andThen(parsePositive)
                          .transform([](int value) { return value * 2; })
                          .orElse([] { return Ark::Option<int>{0}; });

        REQUIRE(result.hasValue());
        REQUIRE(result.getValue() == 42);

        Ark::Option<int> empty;
        auto fallback = empty
                            .andThen(parsePositive)
                            .transform([](int value) { return value * 2; })
                            .orElse([] { return Ark::Option<int>{7}; });
        REQUIRE(fallback.getValue() == 7);

        Ark::Option<int> negative{-3};
        auto rejected = negative.andThen(parsePositive);
        REQUIRE(!rejected.hasValue());
    }

    SECTION("reference option monadic ops")
    {
        int value = 5;
        Ark::Option<int&> bound{value};

        auto doubled = bound.transform([](int& ref) { return ref * 2; });
        REQUIRE(doubled.getValue() == 10);

        auto chained = bound.andThen([](int& ref) -> Ark::Option<int> { return ref + 1; });
        REQUIRE(chained.getValue() == 6);

        Ark::Option<int&> empty;
        int fallback = 99;
        auto recovered = empty.orElse([&] { return Ark::Option<int&>{fallback}; });
        REQUIRE(&recovered.getValue() == &fallback);
    }
}
