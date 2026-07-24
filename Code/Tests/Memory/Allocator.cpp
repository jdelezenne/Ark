#include "Ark/Testing/Test.hpp"

#include "Ark/Memory/Allocator.hpp"
#include "Ark/Memory/Functions.hpp"
#include "Ark/Memory/StandardAllocator.hpp"

using Ark::Memory::allocateZeroed;
using Ark::Memory::allocateZeroedAligned;
using Ark::Memory::Allocator;
using Ark::Memory::reallocate;
using Ark::Memory::reallocateAligned;
using Ark::Memory::tryAllocate;
using Ark::Memory::tryAllocateAligned;

ARK_TEST_CASE("Allocator", "[memory][allocator]")
{
    SECTION("tryAllocate returns nullptr on overflow")
    {
        Ark::Memory::StandardAllocator allocator;

        Ark::usize tooLarge = static_cast<Ark::usize>(Ark::NumericLimits<Ark::uint32>::max()) + 1u;
        void* p = tryAllocate(allocator, tooLarge);
        REQUIRE(p == nullptr);
    }

    SECTION("allocateZeroed returns zeroed memory")
    {
        Ark::Memory::StandardAllocator allocator;

        constexpr Ark::usize size = 128;
        auto* p = static_cast<Ark::byte*>(allocateZeroed(allocator, size));
        REQUIRE(p != nullptr);
        for (Ark::usize i = 0; i < size; ++i)
        {
            REQUIRE(p[i] == 0);
        }
        allocator.deallocate(p);
    }

    SECTION("tryAllocateAligned returns aligned pointer")
    {
        Ark::Memory::StandardAllocator allocator;

        constexpr Ark::usize alignment = 32;
        constexpr Ark::usize size = 128;
        void* p = tryAllocateAligned(allocator, size, alignment);
        REQUIRE(p != nullptr);
        REQUIRE(Ark::Memory::isAligned(reinterpret_cast<Ark::uintptr>(p), alignment));
        allocator.deallocateAligned(p);
    }

    SECTION("reallocate grows and preserves data")
    {
        Ark::Memory::StandardAllocator allocator;

        constexpr Ark::usize oldSize = 64;
        constexpr Ark::usize newSize = 256;

        auto* pOld = static_cast<Ark::byte*>(tryAllocate(allocator, oldSize));
        REQUIRE(pOld != nullptr);
        for (Ark::usize i = 0; i < oldSize; ++i)
        {
            pOld[i] = static_cast<Ark::byte>(i & 0xFF);
        }

        void* pNewVoid = reallocate(allocator, pOld, oldSize, newSize);
        auto* pNew = static_cast<Ark::byte*>(pNewVoid);
        REQUIRE(pNew != nullptr);
        for (Ark::usize i = 0; i < oldSize; ++i)
        {
            REQUIRE(pNew[i] == static_cast<Ark::byte>(i & 0xFF));
        }
        allocator.deallocate(pNew);
    }

    SECTION("allocateArray rejects element-count overflow")
    {
        Ark::Memory::StandardAllocator allocator;
        Ark::usize tooMany = (~static_cast<Ark::usize>(0) / sizeof(Ark::uint64)) + 1u;
        REQUIRE(allocator.allocateArray<Ark::uint64>(tooMany) == nullptr);
    }

    SECTION("make constructs and destroy cleans up")
    {
        Ark::Memory::StandardAllocator allocator;
        struct Value
        {
            int x;
            explicit Value(int v)
                : x(v)
            {
            }
        };

        Value* value = allocator.make<Value>(42);
        REQUIRE(value != nullptr);
        REQUIRE(value->x == 42);
        allocator.destroy(value);
    }

    SECTION("typed allocateZeroed and reallocate")
    {
        Ark::Memory::StandardAllocator allocator;

        constexpr Ark::usize oldCount = 16;
        constexpr Ark::usize newCount = 64;

        Ark::uint32* arr = Ark::Memory::allocateZeroed<Ark::uint32>(allocator, oldCount);
        REQUIRE(arr != nullptr);
        for (Ark::usize i = 0; i < oldCount; ++i)
        {
            REQUIRE(arr[i] == 0u);
            arr[i] = static_cast<Ark::uint32>(i);
        }

        Ark::uint32* arr2 = Ark::Memory::reallocate<Ark::uint32>(allocator, arr, oldCount, newCount);
        REQUIRE(arr2 != nullptr);
        for (Ark::usize i = 0; i < oldCount; ++i)
        {
            REQUIRE(arr2[i] == static_cast<Ark::uint32>(i));
        }
        allocator.deallocate(arr2);
    }
}
