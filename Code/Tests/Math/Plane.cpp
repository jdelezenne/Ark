#include "Ark/Testing/Test.hpp"

#include "Ark/Math/Plane.hpp"
#include "Ark/Math/Vector3.hpp"

ARK_TEST_CASE("Plane", "[plane]")
{
    using Ark::Math::Plane;
    using Ark::Math::Vector3;

    SECTION("inequality when one component differs")
    {
        Plane const a{Vector3::UnitY, 1.0f};
        Plane const b{Vector3::UnitY, 2.0f};
        REQUIRE(a != b);

        Plane const c{Vector3::UnitX, 1.0f};
        REQUIRE(a != c);
    }
}
