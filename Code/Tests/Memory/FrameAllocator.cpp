#include <catch2/catch_test_macros.hpp>

#include "Ark/Memory/FrameAllocator.hpp"

TEST_CASE("FrameAllocator ring allocation and fence advance", "[memory][frame]")
{
    alignas(64) Ark::byte buffer[512];
    Ark::Memory::FrameAllocator alloc{buffer, sizeof(buffer), 64};

    SECTION("ring allocation")
    {
        void* a = alloc.allocateAligned(128, 64);
        REQUIRE(a != nullptr);

        void* b = alloc.allocateAligned(256, 64);
        REQUIRE(b != nullptr);
    }

    SECTION("fence advance reclaims earlier region")
    {
        void* a = alloc.allocateAligned(128, 64);
        REQUIRE(a != nullptr);
        auto fence = alloc.markFence();

        void* b = alloc.allocateAligned(256, 64);
        REQUIRE(b != nullptr);

        alloc.advanceTo(fence);
        void* c = alloc.allocateAligned(128, 64);
        REQUIRE(c != nullptr);
    }

    SECTION("stale fence does not move tail backward")
    {
        void* a = alloc.allocateAligned(128, 64);
        REQUIRE(a != nullptr);
        auto oldFence = alloc.markFence();

        void* b = alloc.allocateAligned(128, 64);
        REQUIRE(b != nullptr);
        auto newFence = alloc.markFence();

        alloc.advanceTo(newFence);
        alloc.advanceTo(oldFence);

        void* c = alloc.allocateAligned(256, 64);
        REQUIRE(c != nullptr);
    }
}
