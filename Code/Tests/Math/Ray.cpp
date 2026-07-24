#include "Ark/Testing/Test.hpp"

#include "Ark/Math/Ray.hpp"
#include "Ark/Math/Vector3.hpp"

ARK_TEST_CASE("Ray", "[ray]")
{
    using Ark::Math::Ray;
    using Ark::Math::Vector3;

    SECTION("constructor stores position and direction")
    {
        Ray const ray{{1.0f, 2.0f, 3.0f}, {0.0f, 1.0f, 0.0f}};
        REQUIRE(ray.position == Vector3{1.0f, 2.0f, 3.0f});
        REQUIRE(ray.direction == Vector3{0.0f, 1.0f, 0.0f});
    }
}
