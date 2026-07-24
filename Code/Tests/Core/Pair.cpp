#include <catch2/catch_test_macros.hpp>

#include "Ark/Core/Pair.hpp"

TEST_CASE("Pair", "[core][pair]")
{
    SECTION("construction and comparison")
    {
        Ark::Pair<int, int> p1{1, 2};
        Ark::Pair<int, int> p2{1, 2};
        Ark::Pair<int, int> p3{2, 1};

        REQUIRE(p1 == p2);
        REQUIRE(p1 != p3);
        REQUIRE(p1 < Ark::Pair<int, int>{2, 0});
        REQUIRE(p3 > p1);
    }

    SECTION("get, tie, and swap")
    {
        Ark::Pair<int, int> p{3, 4};
        REQUIRE(Ark::get<0>(p) == 3);
        REQUIRE(Ark::get<1>(p) == 4);

        auto refPair = Ark::tie(p.first, p.second);
        int& a = refPair.first;
        int& b = refPair.second;
        a = 10;
        b = 20;
        REQUIRE(p.first == 10);
        REQUIRE(p.second == 20);

        Ark::Pair<int, int> q{30, 40};
        p.swap(q);
        REQUIRE(p.first == 30);
        REQUIRE(p.second == 40);
    }

    SECTION("makePair forwards values")
    {
        auto pair = Ark::makePair(1, 2);
        REQUIRE(pair.first == 1);
        REQUIRE(pair.second == 2);

        int x = 5;
        auto fromLvalue = Ark::makePair(x, 6);
        REQUIRE(fromLvalue.first == 5);
        REQUIRE(fromLvalue.second == 6);
    }
}
