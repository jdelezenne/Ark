#include <catch2/catch_test_macros.hpp>

#include "Ark/Core/Comparison.hpp"

TEST_CASE("orderingFromInt converts correctly", "[core][comparison]")
{
    SECTION("negative values map to Less")
    {
        REQUIRE(Ark::orderingFromInt(-100) == Ark::Ordering::Less);
    }

    SECTION("zero maps to Equal")
    {
        REQUIRE(Ark::orderingFromInt(0) == Ark::Ordering::Equal);
    }

    SECTION("positive values map to Greater")
    {
        REQUIRE(Ark::orderingFromInt(42) == Ark::Ordering::Greater);
    }
}
