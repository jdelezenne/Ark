#include "Ark/Testing/Test.hpp"

#include "Ark/Memory/LinearAllocator.hpp"

ARK_TEST_CASE("LinearAllocator", "[arena]")
{
    SECTION("basic allocation and reset")
    {
        alignas(64) Ark::byte buffer[1024];
        Ark::Memory::LinearAllocator arena{buffer, sizeof(buffer), 64};

        void* p1 = arena.allocateAligned(100, 64);
        REQUIRE(p1 != nullptr);

        void* p2 = arena.allocateAligned(200, 64);
        REQUIRE(p2 != nullptr);
        REQUIRE(p2 != p1);

        arena.reset();
        void* p3 = arena.allocateAligned(300, 64);
        REQUIRE(p3 != nullptr);
    }

    SECTION("mark and rollback")
    {
        alignas(64) Ark::byte buffer[256];
        Ark::Memory::LinearAllocator arena{buffer, sizeof(buffer), 64};

        void* a = arena.allocateAligned(64, 64);
        REQUIRE(a != nullptr);
        auto m = arena.mark();
        void* b = arena.allocateAligned(64, 64);
        REQUIRE(b != nullptr);

        arena.rollback(m);
        void* c = arena.allocateAligned(64, 64);
        REQUIRE(c == b);
    }

    SECTION("invalid alignment is rejected")
    {
        alignas(64) Ark::byte buffer[256];
        Ark::Memory::LinearAllocator arena{buffer, sizeof(buffer), 64};

        void* a = arena.allocateAligned(64, 0);
        REQUIRE(a == nullptr);

        void* b = arena.allocateAligned(64, 24);
        REQUIRE(b == nullptr);
    }

    SECTION("allocation that would overflow capacity is rejected")
    {
        alignas(64) Ark::byte buffer[128];
        Ark::Memory::LinearAllocator arena{buffer, sizeof(buffer), 8};

        void* a = arena.allocateAligned(96, 8);
        REQUIRE(a != nullptr);
        void* b = arena.allocateAligned(64, 8);
        REQUIRE(b == nullptr);
    }
}
