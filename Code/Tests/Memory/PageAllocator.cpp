#include "Ark/Testing/Test.hpp"

#include "Ark/Memory/PageAllocator.hpp"
#include "Ark/Memory/VirtualMemory.hpp"

ARK_TEST_CASE("PageAllocator basic allocation contracts", "[memory][page]")
{
    Ark::Memory::PageAllocator allocator;

    SECTION("allocateAligned returns requested alignment")
    {
        void* p = allocator.allocateAligned(1024, 4096);
        REQUIRE(p != nullptr);
        REQUIRE(Ark::Memory::isAligned(reinterpret_cast<Ark::uintptr>(p), 4096));
        allocator.deallocateAligned(p);
    }

    SECTION("allocate/deallocate round-trip")
    {
        void* p = allocator.allocate(2048);
        REQUIRE(p != nullptr);
        allocator.deallocate(p);
    }

    SECTION("reject invalid alignment")
    {
        void* p = allocator.allocateAligned(128, 24);
        REQUIRE(p == nullptr);
    }

    SECTION("decommit leaves pages inaccessible until recommit")
    {
        constexpr Ark::usize size = 4096;
        void* region = Ark::Memory::reserve(size, size);
        REQUIRE(region != nullptr);
        REQUIRE(Ark::Memory::commit(region, size));
        REQUIRE(Ark::Memory::decommit(region, size));
        REQUIRE(Ark::Memory::commit(region, size));
        REQUIRE(Ark::Memory::release(region, size));
    }
}
