#include "Ark/Testing/Test.hpp"
#include <catch2/catch_approx.hpp>

#include "Ark/Math/Color.hpp"

ARK_TEST_CASE("Color", "[color]")
{
    SECTION("clamped returns clamped copy")
    {
        Ark::Color const source{1.5f, -0.25f, 0.5f, 2.0f};
        Ark::Color const result = source.clamped();

        REQUIRE(result.r == Catch::Approx(1.0f));
        REQUIRE(result.g == Catch::Approx(0.0f));
        REQUIRE(result.b == Catch::Approx(0.5f));
        REQUIRE(result.a == Catch::Approx(1.0f));

        REQUIRE(source.r == Catch::Approx(1.5f));
        REQUIRE(source.g == Catch::Approx(-0.25f));
        REQUIRE(source.b == Catch::Approx(0.5f));
        REQUIRE(source.a == Catch::Approx(2.0f));
    }
}
