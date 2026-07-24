#include "Ark/Testing/Test.hpp"

#include "Ark/Strings/AnsiStrings.hpp"

#include <string_view>

ARK_TEST_CASE("AnsiStrings copy and append null-terminate", "[strings][ansi]")
{
    using namespace Ark::AnsiStrings;

    SECTION("bounded copy advances destination and writes NUL")
    {
        char dest[8] = {'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x'};
        REQUIRE(copy("hi", dest, 8));
        REQUIRE(dest[0] == 'h');
        REQUIRE(dest[1] == 'i');
        REQUIRE(dest[2] == '\0');
    }

    SECTION("bounded copy truncates with NUL")
    {
        char dest[3] = {};
        REQUIRE(!copy("hello", dest, 3));
        REQUIRE(dest[2] == '\0');
    }

    SECTION("append null-terminates")
    {
        char dest[16] = "ab";
        REQUIRE(append("cd", dest, 16));
        REQUIRE(std::string_view(dest) == "abcd");
    }

    SECTION("appendUnsafe null-terminates")
    {
        char dest[16] = "ab";
        appendUnsafe("cd", dest);
        REQUIRE(std::string_view(dest) == "abcd");
    }

    SECTION("compare and findFirst bounded helpers")
    {
        REQUIRE(compare("abc", "abd", 3) == Ark::Ordering::Less);
        auto found = findFirst("abcdef", "cd", 6);
        REQUIRE(found.hasValue());
        REQUIRE(*found.getValue() == 'c');
    }
}
