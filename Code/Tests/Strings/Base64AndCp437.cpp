#include "Ark/Testing/Test.hpp"

#include "Ark/Collections/Array.hpp"
#include "Ark/Strings/Base64.hpp"
#include "Ark/Strings/Cp437.hpp"
#include "Ark/Strings/String.hpp"
#include "Ark/Strings/StringSlice.hpp"

ARK_TEST_CASE("Base64 decode validates input", "[strings][base64]")
{
    using Ark::Collections::Array;
    using Ark::uint8;

    SECTION("round trip")
    {
        Ark::String encoded = Ark::Base64::encode(Ark::StringSlice("hello"));
        Array<uint8> bytes;
        REQUIRE(Ark::Base64::decode(encoded.toSlice(), bytes));
        REQUIRE(bytes.getCount() == 5);
        REQUIRE(bytes[0] == 'h');
        REQUIRE(bytes[4] == 'o');
    }

    SECTION("invalid characters fail")
    {
        Array<uint8> bytes;
        REQUIRE(!Ark::Base64::decode(Ark::StringSlice("@@@@"), bytes));
        REQUIRE(bytes.isEmpty());
    }

    SECTION("truncated input fails")
    {
        Array<uint8> bytes;
        REQUIRE(!Ark::Base64::decode(Ark::StringSlice("QQ"), bytes));
        REQUIRE(bytes.isEmpty());
    }
}

ARK_TEST_CASE("CP437 preserves NUL byte", "[strings][cp437]")
{
    Ark::Collections::Array<Ark::uint8> input;
    input.append(0);
    input.append(static_cast<Ark::uint8>('A'));

    Ark::String utf8 = Ark::Strings::convertCp437ToUtf8(input);
    REQUIRE(utf8.getLength() == 2);
    REQUIRE(utf8.asPointer()[0] == '\0');
    REQUIRE(utf8.asPointer()[1] == 'A');
}
