#include "Ark/Testing/Test.hpp"
#include <catch2/catch_approx.hpp>

#include "Ark/Math/Vector3.hpp"

ARK_TEST_CASE("Vector3", "[vector3]")
{
    using Ark::Math::Vector3;

    SECTION("dot product")
    {
        Vector3 x{1.0f, 0.0f, 0.0f};
        Vector3 y{0.0f, 1.0f, 0.0f};
        REQUIRE(Vector3::dot(x, y) == 0.0f);
    }

    SECTION("cross product")
    {
        Vector3 x{1.0f, 0.0f, 0.0f};
        Vector3 y{0.0f, 1.0f, 0.0f};
        Vector3 z = Vector3::cross(x, y);
        REQUIRE(z == Vector3{0.0f, 0.0f, 1.0f});
    }

    SECTION("length and normalization")
    {
        Vector3 v{3.0f, 0.0f, 4.0f};
        REQUIRE(v.getLength() == Catch::Approx(5.0f));
        REQUIRE(v.getNormalized().getLength() == Catch::Approx(1.0f));
    }
}
