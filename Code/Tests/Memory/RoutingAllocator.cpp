#include "Ark/Testing/Test.hpp"

#include "Ark/Memory/RoutingAllocator.hpp"
#include "Ark/Memory/StandardAllocator.hpp"

namespace
{
    struct CountingAllocator final : Ark::Memory::Allocator
    {
        Ark::Memory::Allocator& parent;
        Ark::usize allocCount{0};
        Ark::usize deallocCount{0};
        Ark::usize allocAlignedCount{0};
        Ark::usize deallocAlignedCount{0};

        explicit CountingAllocator(Ark::Memory::Allocator& parentAllocator)
            : parent{parentAllocator}
        {
        }

        void* allocate(Ark::uint size) override
        {
            ++allocCount;
            return parent.allocate(size);
        }

        void deallocate(void* ptr) override
        {
            ++deallocCount;
            parent.deallocate(ptr);
        }

        void* allocateAligned(Ark::uint size, Ark::uint alignment) override
        {
            ++allocAlignedCount;
            return parent.allocateAligned(size, alignment);
        }

        void deallocateAligned(void* ptr) override
        {
            ++deallocAlignedCount;
            parent.deallocateAligned(ptr);
        }
    };
}

ARK_TEST_CASE("RoutingAllocator routes allocation and deallocation consistently", "[memory][routing]")
{
    Ark::Memory::StandardAllocator parent;
    CountingAllocator small{parent};
    CountingAllocator large{parent};
    Ark::Memory::RoutingAllocator routing{small, large, 64};

    SECTION("small allocation deallocates through small allocator")
    {
        void* p = routing.allocate(32);
        REQUIRE(p != nullptr);
        REQUIRE(small.allocCount == 1);
        REQUIRE(large.allocCount == 0);

        routing.deallocate(p);
        REQUIRE(small.deallocCount == 1);
        REQUIRE(large.deallocCount == 0);
    }

    SECTION("large allocation deallocates through large allocator")
    {
        void* p = routing.allocate(128);
        REQUIRE(p != nullptr);
        REQUIRE(small.allocCount == 0);
        REQUIRE(large.allocCount == 1);

        routing.deallocate(p);
        REQUIRE(small.deallocCount == 0);
        REQUIRE(large.deallocCount == 1);
    }

    SECTION("aligned allocation deallocates through aligned path of selected allocator")
    {
        void* p = routing.allocateAligned(24, 32);
        REQUIRE(p != nullptr);
        REQUIRE(Ark::Memory::isAligned(reinterpret_cast<Ark::uintptr>(p), 32));
        REQUIRE(small.allocAlignedCount == 1);
        REQUIRE(large.allocAlignedCount == 0);

        routing.deallocateAligned(p);
        REQUIRE(small.deallocAlignedCount == 1);
        REQUIRE(large.deallocAlignedCount == 0);
    }

    SECTION("tiny requested alignment still yields aligned header storage")
    {
        void* p = routing.allocateAligned(8, 1);
        REQUIRE(p != nullptr);
        REQUIRE(Ark::Memory::isAligned(reinterpret_cast<Ark::uintptr>(p), alignof(void*)));
        routing.deallocateAligned(p);
        REQUIRE(small.deallocAlignedCount == 1);
    }
}
