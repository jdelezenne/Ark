#include "Ark/Testing/Test.hpp"

#include "Ark/Collections/Algorithms.hpp"
#include "Ark/Strings/String.hpp"
#include "Ark/Strings/StringSlice.hpp"

#include <format>

ARK_TEST_CASE("String", "[strings][string][regression]")
{
    using Ark::contains;
    using Ark::endsWith;
    using Ark::findFirst;
    using Ark::findLast;
    using Ark::startsWith;
    using Ark::String;
    using Ark::StringSlice;

    SECTION("construction and mutation")
    {
        String value("hello");

        REQUIRE(value.getLength() == 5);
        REQUIRE(value == "hello");

        StringSlice slice = value.toSlice();
        REQUIRE(slice.getCount() == 5);
        REQUIRE(slice.isEqual("hello"));
    }

    SECTION("append characters slices and strings")
    {
        String value("ab");

        value.append('c');
        value.append("de", 2);
        value.append(StringSlice("fg"));

        REQUIRE(value == "abcdefg");
        REQUIRE(value.getLength() == 7);
    }

    SECTION("copy and move preserve content")
    {
        String original("copy me");
        String copy = original;
        String moved = Ark::move(original);

        REQUIRE(copy == "copy me");
        REQUIRE(moved == "copy me");
    }

    SECTION("search trim and split")
    {
        StringSlice value("  alpha,beta,gamma  ");
        StringSlice trimmed = value.trimmed();

        REQUIRE(startsWith(trimmed, "alpha"));
        REQUIRE(contains(trimmed, "beta"));
        REQUIRE(endsWith(trimmed, "gamma"));
        REQUIRE(!contains(trimmed, "delta"));
    }

    SECTION("substring and trim helpers")
    {
        StringSlice value("  trim me  ");
        StringSlice trimmed = value.trimmed();

        REQUIRE(trimmed.isEqual("trim me"));
        REQUIRE(trimmed.subslice(0, 4).isEqual("trim"));
        REQUIRE(trimmed.subslice(5).isEqual("me"));
        REQUIRE(trimmed.substring(2, 3).isEqual("im "));
        REQUIRE(trimmed.trimmedStart().isEqual("trim me"));
        REQUIRE(trimmed.trimmedEnd().isEqual("trim me"));
    }

    SECTION("substring search and split")
    {
        String value("alpha,beta,gamma");

        auto first = findFirst(value.toSlice(), "beta");
        auto last = findLast(value.toSlice(), "alpha");

        REQUIRE(first.hasValue());
        REQUIRE(first.getValue() == 6);
        REQUIRE(last.hasValue());
        REQUIRE(last.getValue() == 0);

        auto parts = value.split(',');
        REQUIRE(parts.getCount() == 3);
        REQUIRE(parts[0].isEqual("alpha"));
        REQUIRE(parts[1].isEqual("beta"));
        REQUIRE(parts[2].isEqual("gamma"));
    }

    SECTION("character search")
    {
        StringSlice value("mississippi");

        REQUIRE(value.findFirst('s') == 2);
        REQUIRE(value.findLast('s') == 6);
        REQUIRE(value.findFirstIgnoreCase('M') == 0);
        REQUIRE(value.findLastIgnoreCase('I') == 10);
    }

    SECTION("edge-case regression coverage")
    {
        String value("abc");
        REQUIRE(value.findFirst(StringSlice(""), value.getLength()) == value.getLength());
    }

    SECTION("removeAll with longer needle is a no-op")
    {
        String value("abc");
        REQUIRE(value.removeAll(StringSlice("abcd")) == 0);
        REQUIRE(value == "abc");
    }

    SECTION("replace supports expansion safely")
    {
        String value("aaaa");
        REQUIRE(value.replace(StringSlice("a"), StringSlice("bb")) == 4);
        REQUIRE(value == "bbbbbbbb");
    }

    SECTION("formatter for non-null-terminated StringSlice respects length")
    {
        StringSlice slice("abcdef", 3);
        REQUIRE(String::format("{}", slice) == "abc");
    }

    SECTION("left c-string concatenation handles long lhs")
    {
        String lhs('x', 200);
        String rhs("tail");
        String combined = lhs.asPointer() + rhs;

        REQUIRE(combined.getLength() == lhs.getLength() + rhs.getLength());
        REQUIRE(combined.endsWith("tail"));
    }
}