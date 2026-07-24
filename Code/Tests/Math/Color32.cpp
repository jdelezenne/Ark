#include "Ark/Testing/Test.hpp"

#include "Ark/Math/Color32.hpp"

ARK_TEST_CASE("Color32", "[color32]")
{
    SECTION("compound operators use saturating arithmetic")
    {
        Ark::Color32 sum{250, 10, 0, 255};
        sum += Ark::Color32{10, 255, 1, 10};
        REQUIRE(sum == Ark::Color32{255, 255, 1, 255});

        Ark::Color32 diff{5, 10, 3, 1};
        diff -= Ark::Color32{10, 20, 5, 2};
        REQUIRE(diff == Ark::Color32{0, 0, 0, 0});
    }
}
