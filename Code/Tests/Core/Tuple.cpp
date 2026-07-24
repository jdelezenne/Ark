#include "Ark/Testing/Test.hpp"

#include "Ark/Core/Tuple.hpp"

ARK_TEST_CASE("Tuple", "[core][tuple]")
{
    SECTION("get accesses tuple elements")
    {
        Ark::Tuple<Ark::int32, Ark::uint32, Ark::int32> tuple(1, 2u, 3);

        REQUIRE(Ark::get<0>(tuple) == 1);
        REQUIRE(Ark::get<1>(tuple) == 2u);
        REQUIRE(Ark::get<2>(tuple) == 3);
    }

    SECTION("tie stores references")
    {
        Ark::int32 first = 4;
        Ark::int32 second = 9;
        auto tuple = Ark::tie(first, second);

        Ark::get<0>(tuple) = 7;
        Ark::get<1>(tuple) = 11;

        REQUIRE(first == 7);
        REQUIRE(second == 11);
    }

    SECTION("makeTuple decays stored values")
    {
        Ark::int32 value = 5;
        auto tuple = Ark::makeTuple(value, 3u);

        value = 12;

        REQUIRE(Ark::get<0>(tuple) == 5);
        REQUIRE(Ark::get<1>(tuple) == 3u);
    }

    SECTION("apply expands tuple elements into a callable")
    {
        auto tuple = Ark::makeTuple(2, 5, 7);

        Ark::int32 result = Ark::apply([](Ark::int32 left, Ark::int32 middle, Ark::int32 right)
                                       {
                                           return left + middle + right;
                                       },
                                       tuple);

        REQUIRE(result == 14);
    }

    SECTION("index sequence utilities expose the expected size")
    {
        using Sequence = Ark::Traits::MakeIndexSequence<4>;
        REQUIRE(Sequence::size() == 4);

        using ForTypes = Ark::Traits::IndexSequenceFor<Ark::int32, Ark::uint32, Ark::int16>;
        REQUIRE(ForTypes::size() == 3);
    }
}