#pragma once

#include "Ark/Core/Types.hpp"
#include "Ark/Memory/Allocator.hpp"
#include "Ark/Memory/Functions.hpp"

namespace Ark::Memory
{
    /// Fixed-size block allocator over a caller-provided buffer.
    /// Best suited for many allocations of similar size.
    class PoolAllocator final : public Allocator
    {
    private:
        struct FreeNode
        {
            FreeNode* next;
        };

        byte* start{nullptr};
        usize capacity{0};
        usize blockSize{0};
        FreeNode* freeList{nullptr};
        usize defaultAlignment{DefaultAlignment};

    public:
        /// Creates a pool allocator.
        /// @param buffer Backing memory region.
        /// @param capacityBytes Buffer size in bytes.
        /// @param blockSize Size of each pool block.
        /// @param alignment Default alignment used by allocate().
        PoolAllocator(void* buffer, usize capacityBytes, usize blockSize, usize alignment = DefaultAlignment);

        void* allocate(uint size) override;
        void deallocate(void* ptr) override;
        void* allocateAligned(uint size, uint alignment) override;
        void deallocateAligned(void* ptr) override;
        void reset() override;
    };
}
