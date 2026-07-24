#include "Ark/Testing/Test.hpp"
#include <catch2/catch_approx.hpp>

#include "Ark/Math/Angle.hpp"
#include "Ark/Math/Constants.hpp"

ARK_TEST_CASE("Angle", "[angle]")
{
    using Ark::Math::Angle;

    SECTION("unit conversions")
    {
        Angle a = Angle::fromDegrees(180.0f);
        REQUIRE(a.asRadians() == Catch::Approx(Ark::Math::Pi));
        REQUIRE(a.asDegrees() == Catch::Approx(180.0f));

        Angle b = Angle::fromRadians(Ark::Math::PiOver2);
        REQUIRE(b.asDegrees() == Catch::Approx(90.0f));
    }

    SECTION("wrap and normalize")
    {
        Angle a = Angle::fromDegrees(450.0f);
        Angle w = a.wrapped();
        REQUIRE(w.asDegrees() == Catch::Approx(90.0f));

        Angle n = a.getNormalized();
        REQUIRE(n.asDegrees() == Catch::Approx(90.0f));

        Angle b = Angle::fromDegrees(-450.0f);
        Angle bn = b.getNormalized();
        REQUIRE(bn.asDegrees() == Catch::Approx(-90.0f));
    }

    SECTION("atan2 semantics")
    {
        Angle a = Angle::arcTangent2(1.0f, 0.0f);
        REQUIRE(a.asDegrees() == Catch::Approx(90.0f));

        Angle b = Angle::arcTangent2(0.0f, -1.0f);
        REQUIRE(b.asDegrees() == Catch::Approx(180.0f));
    }
}
