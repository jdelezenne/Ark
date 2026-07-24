#include "Ark/Testing/Test.hpp"

#include "Ark/Collections/Array.hpp"
#include "Ark/Strings/String.hpp"
#include "Ark/Strings/StringSlice.hpp"
#include "Ark/Strings/Utf8Strings.hpp"

#include <format>
#include <string>

ARK_TEST_CASE("String containsIgnoreCase and join", "[strings][utf8][join]")
{
    using Ark::String;
    using Ark::StringSlice;

    SECTION("containsIgnoreCase matches differing case")
    {
        String value("Hello World");
        REQUIRE(value.containsIgnoreCase(StringSlice("hello")));
        REQUIRE(value.containsIgnoreCase(StringSlice("WORLD")));
        REQUIRE(!value.containsIgnoreCase(StringSlice("planet")));
    }

    SECTION("static join with separator")
    {
        String a("one");
        String b("two");
        String c("three");
        String* parts[] = {&a, &b, &c};
        String joined = String::join(Ark::Collections::Slice<String*>(parts), String(","));
        REQUIRE(joined == "one,two,three");
    }

    SECTION("instance join allows empty separator")
    {
        String a("x");
        String b("y");
        String* parts[] = {&a, &b};
        String joined = String::join(Ark::Collections::Slice<String*>(parts), String{});
        REQUIRE(joined == "xy");
    }

    SECTION("invalid equal-length UTF-8 compares by remaining bytes")
    {
        char const lhs[] = {'\xFF', 'a'};
        char const rhs[] = {'\xFE', 'a'};
        REQUIRE(Ark::Utf8Strings::compare(lhs, 2, rhs, 2) == Ark::Ordering::Greater);
        REQUIRE(Ark::Utf8Strings::compare(lhs, 2, lhs, 2) == Ark::Ordering::Equal);
    }
}

ARK_TEST_CASE("String assign splitOff replace and codepoint", "[strings][string][mutate]")
{
    using Ark::String;
    using Ark::StringSlice;

    SECTION("assign from interior substring is safe")
    {
        String value("abcdef");
        value.assign(value.asPointer() + 2, 3);
        REQUIRE(value == "cde");
    }

    SECTION("splitOff preserves embedded content after NUL-looking lengths")
    {
        char raw[] = {'a', 'b', '\0', 'c', 'd'};
        String value(raw, 5);
        String tail = value.splitOff(2);
        REQUIRE(value.getLength() == 2);
        REQUIRE(tail.getLength() == 3);
        REQUIRE(tail.asPointer()[0] == '\0');
        REQUIRE(tail.asPointer()[1] == 'c');
    }

    SECTION("replace with self-slice is safe")
    {
        String value("xxYYxx");
        StringSlice self = value.substring(2, 2);
        value.replace(0, 2, self);
        REQUIRE(value.getLength() == 6);
    }

    SECTION("appendCodepoint rejects surrogates via replacement")
    {
        String value;
        value.appendCodepoint(static_cast<Ark::UnicodeChar>(0xD800));
        REQUIRE(value.getLength() == 3); // U+FFFD is 3 UTF-8 bytes
    }

    SECTION("formatter respects length with embedded NUL")
    {
        char raw[] = {'a', '\0', 'b'};
        String value(raw, 3);
        REQUIRE(String::format("{}", value) == String(raw, 3));
    }
}
