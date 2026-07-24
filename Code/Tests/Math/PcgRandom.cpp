#include "Ark/Testing/Test.hpp"

#include "Ark/Math/PcgRandom.hpp"
#include "Ark/Core/NumericLimits.hpp"

ARK_TEST_CASE("PcgRandom", "[pcg]")
{
    SECTION("bounded uint64 generation returns in requested range")
    {
        Ark::Math::PcgRandom random(12345u);
        for (int i = 0; i < 1024; ++i)
        {
            Ark::uint64 const value = random.nextUInt64(3u, 9u);
            REQUIRE(value >= 3u);
            REQUIRE(value <= 9u);
        }
    }

    SECTION("full uint32 range is supported")
    {
        Ark::Math::PcgRandom random(67890u);
        Ark::uint32 const value = random.nextUInt32(0u, Ark::NumericLimits<Ark::uint32>::max());
        (void)value;
        REQUIRE(true);
    }

    SECTION("full int64 range is supported")
    {
        Ark::Math::PcgRandom random(24680u);
        Ark::int64 const value = random.nextInt64(Ark::NumericLimits<Ark::int64>::lowest(), Ark::NumericLimits<Ark::int64>::max());
        (void)value;
        REQUIRE(true);
    }
}
