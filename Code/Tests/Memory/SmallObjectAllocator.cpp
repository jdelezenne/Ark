#include "Ark/Testing/Test.hpp"

#include "Ark/Memory/Functions.hpp"
#include "Ark/Memory/SmallObjectAllocator.hpp"
#include "Ark/Memory/StandardAllocator.hpp"

ARK_TEST_CASE("SmallObjectAllocator", "[smallobj]")
{
    Ark::Memory::StandardAllocator parent;
    Ark::Memory::SmallObjectAllocator soa{parent, 256, static_cast<Ark::usize>(16) * 1024};

    SECTION("size class allocation and reuse")
    {
        void* a = soa.allocate(24);
        void* b = soa.allocate(24);
        REQUIRE(a != nullptr);
        REQUIRE(b != nullptr);
        REQUIRE(a != b);

        soa.deallocate(a);
        void* c = soa.allocate(24);
        REQUIRE(c == a);
        soa.deallocate(b);
        soa.deallocate(c);
    }

    SECTION("fallback allocation for large blocks")
    {
        void* big = soa.allocate(1024);
        REQUIRE(big != nullptr);
        soa.deallocate(big);
    }

    SECTION("aligned fallback round-trips through deallocateAligned")
    {
        void* p = soa.allocateAligned(2048, 64);
        REQUIRE(p != nullptr);
        REQUIRE(Ark::Memory::isAligned(reinterpret_cast<Ark::uintptr>(p), 64));
        REQUIRE_FALSE(soa.owns(p));
        soa.deallocateAligned(p);
    }

    SECTION("owns reports slab pointers")
    {
        void* p = soa.allocate(16);
        REQUIRE(p != nullptr);
        REQUIRE(soa.owns(p));
        soa.deallocate(p);
    }
}

ARK_TEST_CASE("SmallObjectAllocator destructor releases slabs", "[smallobj]")
{
    Ark::Memory::StandardAllocator parent;
    {
        Ark::Memory::SmallObjectAllocator soa{parent, 256, static_cast<Ark::usize>(4) * 1024};
        REQUIRE(soa.allocate(32) != nullptr);
        REQUIRE(soa.allocate(64) != nullptr);
    }
}
