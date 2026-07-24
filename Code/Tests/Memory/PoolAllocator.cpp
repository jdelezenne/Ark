#include <catch2/catch_test_macros.hpp>

#include "Ark/Memory/PoolAllocator.hpp"

TEST_CASE("PoolAllocator alloc/free reuse", "[memory][pool]")
{
    alignas(64) Ark::byte buffer[1024];
    Ark::Memory::PoolAllocator pool{buffer, sizeof(buffer), 64, 64};

    SECTION("allocations return distinct blocks")
    {
        void* a = pool.allocate(32);
        void* b = pool.allocate(32);
        REQUIRE(a != nullptr);
        REQUIRE(b != nullptr);
        REQUIRE(a != b);
        pool.deallocate(a);
        pool.deallocate(b);
    }

    SECTION("freed blocks are reused")
    {
        void* a = pool.allocate(32);
        REQUIRE(a != nullptr);
        pool.deallocate(a);
        void* c = pool.allocate(16);
        REQUIRE(c == a);
    }

    SECTION("foreign pointer deallocation is ignored")
    {
        alignas(64) Ark::byte oneBlock[64];
        Ark::Memory::PoolAllocator oneBlockPool{oneBlock, sizeof(oneBlock), 64, 64};

        void* a = oneBlockPool.allocate(64);
        REQUIRE(a != nullptr);

        Ark::byte foreign = 0;
        oneBlockPool.deallocate(&foreign);

        void* b = oneBlockPool.allocate(64);
        REQUIRE(b == nullptr);
    }

    SECTION("misaligned in-range pointer deallocation is ignored")
    {
        alignas(64) Ark::byte oneBlock[64];
        Ark::Memory::PoolAllocator oneBlockPool{oneBlock, sizeof(oneBlock), 64, 64};

        void* a = oneBlockPool.allocate(64);
        REQUIRE(a != nullptr);

        oneBlockPool.deallocate(static_cast<void*>(oneBlock + 1));

        void* b = oneBlockPool.allocate(64);
        REQUIRE(b == nullptr);
    }

    SECTION("double free does not duplicate freelist entries")
    {
        alignas(64) Ark::byte oneBlock[64];
        Ark::Memory::PoolAllocator oneBlockPool{oneBlock, sizeof(oneBlock), 64, 64};

        void* a = oneBlockPool.allocate(64);
        REQUIRE(a != nullptr);

        oneBlockPool.deallocate(a);
        oneBlockPool.deallocate(a);

        void* b = oneBlockPool.allocate(64);
        REQUIRE(b == a);
        void* c = oneBlockPool.allocate(64);
        REQUIRE(c == nullptr);
    }

    SECTION("zero block size yields empty pool")
    {
        alignas(64) Ark::byte buffer[256];
        Ark::Memory::PoolAllocator pool{buffer, sizeof(buffer), 0, 64};
        REQUIRE(pool.allocate(8) == nullptr);
    }
}
