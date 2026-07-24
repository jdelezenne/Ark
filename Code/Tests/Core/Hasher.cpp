#include "Ark/Testing/Test.hpp"

#include "Ark/Core/Hasher.hpp"

using Ark::operator""_hash;

ARK_TEST_CASE("Hasher", "[hasher]")
{
    SECTION("Hasher computes stable values for integers and floats")
    {
        Ark::Hasher<int> intHasher;
        REQUIRE(intHasher(0) == Ark::computeHashValue(0));
        REQUIRE(intHasher(123456) == Ark::computeHashValue(123456));

        Ark::Hasher<float> floatHasher;
        REQUIRE(floatHasher(0.0f) == Ark::computeHashValue(0.0f));
        REQUIRE(floatHasher(-0.0f) == Ark::computeHashValue(0.0f));
    }

    SECTION("User-defined literal _hash produces FNV-1a")
    {
        constexpr auto h1 = "hello"_hash;
        constexpr auto h2 = "hello"_hash;
        constexpr auto h3 = "world"_hash;
        STATIC_REQUIRE(h1 == h2);
        STATIC_REQUIRE(h1 != h3);
    }
}
