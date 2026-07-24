#include "Ark/Testing/Test.hpp"

#include "Ark/Strings/String.hpp"
#include "Ark/Strings/StringSlice.hpp"

ARK_TEST_CASE("StringSlice access helpers", "[strings][slice]")
{
    Ark::String value("slice");
    Ark::StringSlice slice = value;

    REQUIRE(slice.getCount() == 5);
    REQUIRE(slice.getFirst() == 's');
    REQUIRE(slice.getLast() == 'e');
    REQUIRE(slice.get(2) == 'i');
    REQUIRE(slice.tryGet(10).hasValue() == false);
}

ARK_TEST_CASE("StringSlice comparison semantics", "[strings][slice]")
{
    Ark::StringSlice mixed("MiXeD");

    REQUIRE(mixed.isEqualIgnoreCase("mixed"));
    REQUIRE(mixed.compareIgnoreCase("mixed") == Ark::Ordering::Equal);
    REQUIRE(mixed.compare("mixed") != Ark::Ordering::Equal);
}

ARK_TEST_CASE("StringSlice range iteration", "[strings][slice]")
{
    Ark::StringSlice slice("abc");

    Ark::String collected;
    for (char ch : slice)
    {
        collected.append(ch);
    }

    REQUIRE(collected == "abc");
}
