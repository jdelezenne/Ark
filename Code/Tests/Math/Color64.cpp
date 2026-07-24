#include "Ark/Testing/Test.hpp"

#include "Ark/Math/Color64.hpp"

ARK_TEST_CASE("Color64", "[color64]")
{
    SECTION("compound operators use saturating arithmetic")
    {
        Ark::Color64 sum{65530, 100, 0, 65535};
        sum += Ark::Color64{10, 65535, 1, 10};
        REQUIRE(sum == Ark::Color64{65535, 65535, 1, 65535});

        Ark::Color64 diff{5, 10, 3, 1};
        diff -= Ark::Color64{10, 20, 5, 2};
        REQUIRE(diff == Ark::Color64{0, 0, 0, 0});
    }
}
