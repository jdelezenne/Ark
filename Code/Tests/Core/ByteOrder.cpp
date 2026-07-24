#include "Ark/Core/ByteOrder.hpp"
#include "Ark/Testing/Test.hpp"

using Ark::float64;
using Ark::uint16;
using Ark::uint32;
using Ark::uint64;

ARK_TEST_CASE("ByteOrder", "[core][byteorder]")
{
    SECTION("integer swaps produce expected bit patterns")
    {
        REQUIRE(Ark::ByteSwap::swapUInt16(static_cast<uint16>(0x1234u)) == static_cast<uint16>(0x3412u));
        REQUIRE(Ark::ByteSwap::swapUInt32(static_cast<uint32>(0x12345678u)) == static_cast<uint32>(0x78563412u));
        REQUIRE(Ark::ByteSwap::swapUInt64(static_cast<uint64>(0x0123456789ABCDEFull)) == static_cast<uint64>(0xEFCDAB8967452301ull));
    }

    SECTION("integer swaps are involutive")
    {
        auto const v16 = static_cast<uint16>(0xA1B2u);
        auto const v32 = static_cast<uint32>(0x89ABCDEFu);
        auto const v64 = static_cast<uint64>(0x1122334455667788ull);

        REQUIRE(Ark::ByteSwap::swapUInt16(Ark::ByteSwap::swapUInt16(v16)) == v16);
        REQUIRE(Ark::ByteSwap::swapUInt32(Ark::ByteSwap::swapUInt32(v32)) == v32);
        REQUIRE(Ark::ByteSwap::swapUInt64(Ark::ByteSwap::swapUInt64(v64)) == v64);
    }

    SECTION("floating swaps round-trip")
    {
        float const f = 1.2345f;
        float64 const d = 3.141592653589793;

        float const f2 = Ark::ByteSwap::swapFloat(Ark::ByteSwap::swapFloat(f));
        float64 const d2 = Ark::ByteSwap::swapDouble(Ark::ByteSwap::swapDouble(d));

        REQUIRE(f2 == f);
        REQUIRE(d2 == d);
    }

    SECTION("constexpr swap functions produce expected values")
    {
        [[maybe_unused]] constexpr uint16 a16 = 0x1234u;
        [[maybe_unused]] constexpr uint32 a32 = 0x12345678u;
        [[maybe_unused]] constexpr uint64 a64 = 0x0123456789ABCDEFull;

        static_assert(Ark::Internal::swap(a16) == 0x3412u, "swap16 constexpr failed");
        static_assert(Ark::Internal::swap(a32) == 0x78563412u, "swap32 constexpr failed");
        static_assert(Ark::Internal::swap(a64) == 0xEFCDAB8967452301ull, "swap64 constexpr failed");
    }

    SECTION("native endian conversion identity on host")
    {
        auto const value = static_cast<uint32>(0x11223344u);
#if defined(ARK_LITTLE_ENDIAN)
        REQUIRE(Ark::getNativeByteOrder() == Ark::ByteOrder::LittleEndian);
        REQUIRE(Ark::ByteSwap::toLittleEndianUInt32(value) == value);
        REQUIRE(Ark::ByteSwap::toBigEndianUInt32(value) == Ark::ByteSwap::swapUInt32(value));
#elif defined(ARK_BIG_ENDIAN)
        REQUIRE(Ark::getNativeByteOrder() == Ark::ByteOrder::BigEndian);
        REQUIRE(Ark::ByteSwap::toBigEndianUInt32(value) == value);
        REQUIRE(Ark::ByteSwap::toLittleEndianUInt32(value) == Ark::ByteSwap::swapUInt32(value));
#endif
    }
}
