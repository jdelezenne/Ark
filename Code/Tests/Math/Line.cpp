#include "Ark/Testing/Test.hpp"
#include <catch2/catch_approx.hpp>

#include "Ark/Math/Line.hpp"

ARK_TEST_CASE("Line", "[line]")
{
    using Ark::Math::Line2;
    using Ark::Math::Vector2;

    SECTION("distance uses closest point on segment")
    {
        Line2 line{{0.0f, 0.0f}, {10.0f, 0.0f}};
        REQUIRE(line.getDistance({5.0f, 5.0f}) == Catch::Approx(5.0f));
        REQUIRE(line.getDistance({15.0f, 0.0f}) == Catch::Approx(5.0f));
    }

    SECTION("contains checks geometric distance to segment")
    {
        Line2 line{{0.0f, 0.0f}, {10.0f, 0.0f}};
        REQUIRE(line.contains({5.0f, 0.0f}, 0.0001f));
        REQUIRE_FALSE(line.contains({5.0f, 0.1f}, 0.0001f));
    }

    SECTION("intersection works regardless of line orientation")
    {
        Line2 a{{1.0f, 1.0f}, {0.0f, 0.0f}};
        Line2 b{{1.0f, 0.0f}, {0.0f, 1.0f}};

        auto intersection = a.intersection(b);
        REQUIRE(intersection.hasValue());
        REQUIRE(intersection.getValue().x == Catch::Approx(0.5f));
        REQUIRE(intersection.getValue().y == Catch::Approx(0.5f));
    }
}
