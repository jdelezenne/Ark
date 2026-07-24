#include "Ark/Testing/Test.hpp"
#include <catch2/catch_approx.hpp>

#include "Ark/Core/NumericLimits.hpp"
#include "Ark/Strings/AnsiConversions.hpp"
#include "Ark/Strings/StringSlice.hpp"

ARK_TEST_CASE("AnsiConversions integer and float edge cases", "[strings][conversions]")
{
    using namespace Ark::Strings;

    SECTION("toInt rejects whitespace-only")
    {
        REQUIRE_FALSE(toInt<int>("   ").hasValue());
        REQUIRE_FALSE(toInt<int>("").hasValue());
    }

    SECTION("fromInt formats INT_MIN correctly")
    {
        auto text = fromInt(Ark::NumericLimits<Ark::int32>::min());
        REQUIRE(text.hasValue());
        REQUIRE(text.getValue() == "-2147483648");
    }

    SECTION("toFloat handles negative exponents")
    {
        auto value = toFloat<double>("1.5e-2");
        REQUIRE(value.hasValue());
        REQUIRE(value.getValue() == Catch::Approx(0.015));
    }

    SECTION("toFloat rejects whitespace-only")
    {
        REQUIRE_FALSE(toFloat<double>(" \t ").hasValue());
    }

    SECTION("fromFloat keeps integer trailing zeros")
    {
        // precision > |exponent| so the value stays in fixed form ("100.00000" -> "100").
        auto text = fromFloat(100.0, 5, false);
        REQUIRE(text.hasValue());
        REQUIRE(text.getValue() == "100");
    }

    SECTION("fromFloat zero stays zero")
    {
        auto text = fromFloat(0.0, 0, false);
        REQUIRE(text.hasValue());
        REQUIRE(text.getValue() == "0");
    }
}
