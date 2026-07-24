#include "Ark/Testing/Test.hpp"
#include <catch2/catch_approx.hpp>

#include "Ark/Math/Polygon.hpp"

ARK_TEST_CASE("Polygon", "[polygon]")
{
    SECTION("centre matches average point for symmetric polygon")
    {
        Ark::Math::Polygon polygon;
        polygon.append({0.0f, 0.0f});
        polygon.append({2.0f, 0.0f});
        polygon.append({2.0f, 2.0f});
        polygon.append({0.0f, 2.0f});

        Ark::Math::Vector2 const center = polygon.getCentre();
        REQUIRE(center.x == Catch::Approx(1.0f));
        REQUIRE(center.y == Catch::Approx(1.0f));
    }
}
