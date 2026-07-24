#include "Ark/Testing/Test.hpp"

#include "Ark/Math/BoundingBox.hpp"

ARK_TEST_CASE("BoundingBox", "[boundingbox]")
{
    using Ark::Math::BoundingBox;

    SECTION("overlap and disjoint")
    {
        BoundingBox const a{{0.0f, 0.0f, 0.0f}, {2.0f, 2.0f, 2.0f}};
        BoundingBox const b{{1.0f, 1.0f, 1.0f}, {3.0f, 3.0f, 3.0f}};
        BoundingBox const c{{3.1f, 0.0f, 0.0f}, {4.0f, 1.0f, 1.0f}};

        REQUIRE(a.overlaps(b));
        REQUIRE_FALSE(a.overlaps(c));
    }
}
