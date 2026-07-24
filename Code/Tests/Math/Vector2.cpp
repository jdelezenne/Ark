#include "Ark/Testing/Test.hpp"
#include <catch2/catch_approx.hpp>

#include "Ark/Math/Vector2.hpp"

ARK_TEST_CASE("Vector2", "[vector2]")
{
    using Ark::Math::Vector2;

    SECTION("length")
    {
        Vector2 v{3.0f, 4.0f};
        REQUIRE(v.getLength() == Catch::Approx(5.0f));
    }

    SECTION("addition")
    {
        Vector2 v{3.0f, 4.0f};
        Vector2 u{1.0f, 2.0f};
        Vector2 w = v + u;
        REQUIRE(w == Vector2{4.0f, 6.0f});
    }

    SECTION("normalization")
    {
        Vector2 v{3.0f, 4.0f};
        Vector2 n = v.getNormalized();
        REQUIRE(n.getLength() == Catch::Approx(1.0f));
    }
}
