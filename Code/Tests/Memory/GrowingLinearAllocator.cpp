#include "Ark/Testing/Test.hpp"

#include "Ark/Memory/Functions.hpp"
#include "Ark/Memory/GrowingLinearAllocator.hpp"
#include "Ark/Memory/StandardAllocator.hpp"

ARK_TEST_CASE("GrowingLinearAllocator basic allocate and reset", "[arena]")
{
    Ark::Memory::StandardAllocator stdAlloc;
    Ark::Memory::GrowingLinearAllocator arena{stdAlloc, 256, Ark::Memory::DefaultAlignment};

    void* a = arena.allocateAligned(64, 64);
    REQUIRE(a != nullptr);

    void* b = arena.allocateAligned(64, 64);
    REQUIRE(b != nullptr);
    REQUIRE(b != a);

    arena.reset();
    void* c = arena.allocateAligned(128, 64);
    REQUIRE(c != nullptr);
}

ARK_TEST_CASE("GrowingLinearAllocator grows across blocks", "[arena]")
{
    Ark::Memory::StandardAllocator stdAlloc;
    Ark::Memory::GrowingLinearAllocator smallArena{stdAlloc, 128, 64};

    void* a = smallArena.allocateAligned(120, 64);
    REQUIRE(a != nullptr);

    void* b = smallArena.allocateAligned(96, 64);
    REQUIRE(b != nullptr);
}

ARK_TEST_CASE("GrowingLinearAllocator mark and rollback across blocks", "[arena]")
{
    Ark::Memory::StandardAllocator stdAlloc;
    Ark::Memory::GrowingLinearAllocator smallArena{stdAlloc, 128, 64};

    void* a = smallArena.allocateAligned(64, 64);
    REQUIRE(a != nullptr);
    auto m = smallArena.mark();

    void* b = smallArena.allocateAligned(256, 64);
    REQUIRE(b != nullptr);

    smallArena.rollback(m);
    void* c = smallArena.allocateAligned(64, 64);
    REQUIRE(c != nullptr);
}

ARK_TEST_CASE("GrowingLinearAllocator rejects requests larger than max block", "[arena]")
{
    Ark::Memory::StandardAllocator stdAlloc;
    Ark::Memory::GrowingLinearAllocator arena{stdAlloc, 128, 64, 256};

    void* p = arena.allocateAligned(300, 64);
    REQUIRE(p == nullptr);
}

ARK_TEST_CASE("GrowingLinearAllocator stale marker rollback is ignored", "[arena]")
{
    Ark::Memory::StandardAllocator stdAlloc;
    Ark::Memory::GrowingLinearAllocator arena{stdAlloc, 128, 64};

    void* a = arena.allocateAligned(64, 64);
    REQUIRE(a != nullptr);
    auto early = arena.mark();

    void* b = arena.allocateAligned(256, 64);
    REQUIRE(b != nullptr);
    auto late = arena.mark();

    arena.rollback(early);
    arena.rollback(late); // stale: block was released by earlier rollback

    void* c = arena.allocateAligned(64, 64);
    REQUIRE(c != nullptr);
}

ARK_TEST_CASE("GrowingLinearAllocator destructor releases blocks", "[arena]")
{
    Ark::Memory::StandardAllocator stdAlloc;
    {
        Ark::Memory::GrowingLinearAllocator arena{stdAlloc, 128, 64};
        REQUIRE(arena.allocateAligned(64, 64) != nullptr);
        REQUIRE(arena.allocateAligned(256, 64) != nullptr);
    }
}
