#include "Ark/Testing/Test.hpp"
#include <catch2/catch_approx.hpp>

#include "Ark/Math/Sphere.hpp"
#include "Ark/Math/Vector3.hpp"

ARK_TEST_CASE("Sphere", "[sphere]")
{
    using Ark::Math::Sphere;
    using Ark::Math::Vector3;

    SECTION("signed distance to point")
    {
        Sphere const sphere{{0.0f, 0.0f, 0.0f}, 2.0f};
        REQUIRE(sphere.getDistanceTo(Vector3{0.0f, 0.0f, 0.0f}) == Catch::Approx(-2.0f));
        REQUIRE(sphere.getDistanceTo(Vector3{4.0f, 0.0f, 0.0f}) == Catch::Approx(2.0f));
    }

    SECTION("signed distance to sphere")
    {
        Sphere const a{{0.0f, 0.0f, 0.0f}, 2.0f};
        Sphere const b{{3.0f, 0.0f, 0.0f}, 2.0f};
        Sphere const c{{6.0f, 0.0f, 0.0f}, 2.0f};

        REQUIRE(a.getDistanceTo(b) == Catch::Approx(-1.0f));
        REQUIRE(a.getDistanceTo(c) == Catch::Approx(2.0f));
    }
}
