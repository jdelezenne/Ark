#include "Ark/Testing/Test.hpp"
#include <catch2/catch_approx.hpp>

#include "Ark/Math/Functions.hpp"

ARK_TEST_CASE("Math Functions", "[functions]")
{
    SECTION("saturate and saturateWrap")
    {
        using Ark::Math::saturate;
        using Ark::Math::saturateWrap;

        REQUIRE(saturate(-0.5f) == Catch::Approx(0.0f));
        REQUIRE(saturate(1.5f) == Catch::Approx(1.0f));
        REQUIRE(saturateWrap(1.25f) == Catch::Approx(0.25f));
    }

    SECTION("clampWrap and modulo")
    {
        using Ark::Math::clampWrap;
        using Ark::Math::modulo;

        REQUIRE(clampWrap<int, int, int>(-1, 0, 5) == 5);
        REQUIRE(clampWrap<int, int, int>(6, 0, 5) == 0);
        REQUIRE(modulo<float>(7.5f, 2.0f) == Catch::Approx(1.5f));
    }

    SECTION("pow10 and smoothStep")
    {
        using Ark::Math::pow10;
        using Ark::Math::smoothStep;

        REQUIRE(pow10(0) == Catch::Approx(1.0));
        REQUIRE(pow10(3) == Catch::Approx(1000.0));
        REQUIRE(pow10(-2) == Catch::Approx(0.01));

        REQUIRE(smoothStep(-1.0f) == Catch::Approx(0.0f));
        REQUIRE(smoothStep(0.0f) == Catch::Approx(0.0f));
        REQUIRE(smoothStep(1.0f) == Catch::Approx(1.0f));
    }

    SECTION("normalize remap and wrap")
    {
        using Ark::Math::normalize;
        using Ark::Math::remap;
        using Ark::Math::wrap;

        REQUIRE(normalize(5.0f, 0.0f, 10.0f) == Catch::Approx(0.5f));
        REQUIRE(remap(5.0f, 0.0f, 10.0f, -1.0f, 1.0f) == Catch::Approx(0.0f));
        REQUIRE(wrap(370.0f, 0.0f, 360.0f) == Catch::Approx(10.0f));
    }

    SECTION("isZero uses tolerance")
    {
        REQUIRE(Ark::Math::isZero(0.0f));
        REQUIRE(Ark::Math::isZero(0.0));

        REQUIRE(Ark::Math::isZero(Ark::Math::ZeroTolerance<Ark::float32> * 0.5f));
        REQUIRE(Ark::Math::isZero(Ark::Math::ZeroTolerance<Ark::float64> * 0.5));

        REQUIRE_FALSE(Ark::Math::isZero(Ark::Math::ZeroTolerance<Ark::float32> * 2.0f));
        REQUIRE_FALSE(Ark::Math::isZero(Ark::Math::ZeroTolerance<Ark::float64> * 2.0));
    }
}
