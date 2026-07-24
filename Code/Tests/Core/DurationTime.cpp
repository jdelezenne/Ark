#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "Ark/Core/Duration.hpp"
#include "Ark/Core/Time.hpp"
using Ark::TimeHelper::microsecondsToNanoseconds;
using Ark::TimeHelper::millisecondsPerSecond;
using Ark::TimeHelper::millisecondsToNanoseconds;
using Ark::TimeHelper::nanosecondsToMicroseconds;
using Ark::TimeHelper::nanosecondsToMilliseconds;
using Ark::TimeHelper::secondsToMilliseconds;

TEST_CASE("Duration construction and arithmetic", "[core][duration]")
{
    SECTION("construction from seconds and milliseconds")
    {
        auto d1 = Ark::Duration::fromSeconds(2.0);
        auto d2 = Ark::Duration::fromMilliseconds(500.0);
        auto d3 = d1 + d2;
        REQUIRE(d1 < d3);
        REQUIRE(d3.getTotalSeconds() > 2.0f);
    }

    SECTION("arithmetic inverse and absolute value")
    {
        auto d1 = Ark::Duration::fromSeconds(2.0);
        auto d2 = Ark::Duration::fromMilliseconds(500.0);
        auto d3 = d1 + d2;
        auto d4 = d3 - d2;
        REQUIRE(d4 == d1);

        auto d5 = -d1;
        REQUIRE(d5.getAbsolute() == d1);
    }
}

TEST_CASE("Time helper conversions", "[core][time]")
{
    SECTION("seconds and milliseconds")
    {
        REQUIRE(secondsToMilliseconds(2.0) == 2000.0);
        REQUIRE_THAT(Ark::TimeHelper::millisecondsToSeconds(500.0), Catch::Matchers::WithinAbs(0.5, 1e-9));
    }

    SECTION("milliseconds and nanoseconds")
    {
        REQUIRE(millisecondsToNanoseconds(1.0) == 1'000'000.0);
        REQUIRE(nanosecondsToMilliseconds(1'000'000.0) == 1.0);
    }

    SECTION("microseconds and nanoseconds")
    {
        REQUIRE(microsecondsToNanoseconds(2.0) == 2000.0);
        REQUIRE(nanosecondsToMicroseconds(2000.0) == 2.0);
    }
}
