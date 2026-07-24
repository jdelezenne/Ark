#include "Ark/Testing/Test.hpp"

#include "Ark/Core/Comparison.hpp"
#include "Ark/Core/Types.hpp"
#include "Ark/Memory/Functions.hpp"

ARK_TEST_CASE("Memory", "[memory]")
{
    SECTION("copy and move for trivially copyable types")
    {
        Ark::uint32 src[8] = {0, 1, 2, 3, 4, 5, 6, 7};
        Ark::uint32 dst[8] = {};

        Ark::Memory::copyCount(src, dst, 8);
        for (Ark::usize i = 0; i < 8; ++i)
        {
            REQUIRE(dst[i] == src[i]);
        }

        Ark::Memory::moveCount(dst, dst + 2, 6);
        REQUIRE(dst[2] == 0);
        REQUIRE(dst[7] == 5);

        Ark::uint32 arr[6] = {10, 11, 12, 13, 14, 15};
        Ark::Memory::moveCount(arr + 1, arr, 5);
        Ark::uint32 expect[6] = {11, 12, 13, 14, 15, 15};
        for (Ark::usize i = 0; i < 6; ++i)
        {
            REQUIRE(arr[i] == expect[i]);
        }
    }

    SECTION("set and zero")
    {
        Ark::byte buf[16];
        Ark::Memory::set<Ark::byte>(buf, static_cast<Ark::byte>(0xAB), 16);
        for (Ark::usize i = 0; i < 16; ++i)
        {
            REQUIRE(buf[i] == static_cast<Ark::byte>(0xAB));
        }

        Ark::Memory::zero(buf, 16);
        for (Ark::usize i = 0; i < 16; ++i)
        {
            REQUIRE(buf[i] == 0);
        }
    }

    SECTION("compare and find")
    {
        Ark::uint8 a[4] = {1, 2, 3, 4};
        Ark::uint8 b[4] = {1, 2, 3, 5};

        REQUIRE(Ark::Memory::compare(a, a, 4) == Ark::Ordering::Equal);
        REQUIRE(Ark::Memory::compare(a, b, 3) == Ark::Ordering::Equal);
        REQUIRE(Ark::Memory::compare(a, b, 4) == Ark::Ordering::Less);

        REQUIRE(Ark::Memory::find(a, static_cast<Ark::uint8>(3), 4));
        REQUIRE_FALSE(Ark::Memory::find(a, static_cast<Ark::uint8>(9), 4));
    }

    SECTION("power of two helpers")
    {
        REQUIRE(Ark::Memory::isPowerOfTwo(1));
        REQUIRE(Ark::Memory::isPowerOfTwo(2));
        REQUIRE(Ark::Memory::isPowerOfTwo(64));
        REQUIRE_FALSE(Ark::Memory::isPowerOfTwo(0));
        REQUIRE_FALSE(Ark::Memory::isPowerOfTwo(3));

        REQUIRE(Ark::Memory::nextPowerOfTwo(static_cast<Ark::uint32>(0)) == 1u);
        REQUIRE(Ark::Memory::nextPowerOfTwo(static_cast<Ark::uint32>(5)) == 8u);
        REQUIRE(Ark::Memory::nextPowerOfTwo(static_cast<Ark::uint32>(8)) == 8u);
    }

    SECTION("alignment helpers")
    {
        Ark::uintptr base = 0x1003;
        REQUIRE_FALSE(Ark::Memory::isAligned(base, 8));
        REQUIRE(Ark::Memory::isAligned(0x1008, 8));

        REQUIRE(Ark::Memory::alignSize(13, 8) == 16);
        REQUIRE(Ark::Memory::alignUp(0x1013, 16) == 0x1020);
        REQUIRE(Ark::Memory::alignDown(0x1013, 16) == 0x1010);
        REQUIRE(Ark::Memory::alignmentPadding(0x1013, 16) == 13);
    }
}
