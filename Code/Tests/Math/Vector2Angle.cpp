#include "Ark/Testing/Test.hpp"
#include <catch2/catch_approx.hpp>

#include "Ark/Math/Constants.hpp"
#include "Ark/Math/Vector2.hpp"

ARK_TEST_CASE("Vector2 Angle", "[vector2][angle]")
{
    SECTION("angle is signed by cross product direction")
    {
        using Ark::Math::Vector2;

        float const ccw = Vector2::getAngle(Vector2::UnitX, Vector2::UnitY);
        REQUIRE(ccw == Catch::Approx(Ark::Math::PiOver2));

        float const cw = Vector2::getAngle(Vector2::UnitY, Vector2::UnitX);
        REQUIRE(cw == Catch::Approx(-Ark::Math::PiOver2));
    }
}
