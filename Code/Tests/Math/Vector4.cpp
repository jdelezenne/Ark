#include "Ark/Testing/Test.hpp"
#include <catch2/catch_approx.hpp>

#include "Ark/Math/Vector4.hpp"

ARK_TEST_CASE("Vector4", "[vector4]")
{
    using Ark::Math::Vector4;

    SECTION("length and normalize")
    {
        Vector4 v{2.0f, 0.0f, 0.0f, 0.0f};
        REQUIRE(v.getLength() == Catch::Approx(2.0f));

        Vector4 n = v.getNormalized();
        REQUIRE(n.x == Catch::Approx(1.0f));
        REQUIRE(n.y == Catch::Approx(0.0f));
        REQUIRE(n.z == Catch::Approx(0.0f));
        REQUIRE(n.w == Catch::Approx(0.0f));
    }

    SECTION("lerp and dot")
    {
        Vector4 a{0.0f, 0.0f, 0.0f, 0.0f};
        Vector4 b{1.0f, 2.0f, 3.0f, 4.0f};

        Vector4 m = Vector4::lerp(a, b, 0.5f);
        REQUIRE(m.x == Catch::Approx(0.5f));
        REQUIRE(m.y == Catch::Approx(1.0f));
        REQUIRE(m.z == Catch::Approx(1.5f));
        REQUIRE(m.w == Catch::Approx(2.0f));

        REQUIRE(Vector4::dot(b, b) == Catch::Approx(1 + 4 + 9 + 16));
    }
}
