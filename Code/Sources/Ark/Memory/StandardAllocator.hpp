#pragma once

#include "Ark/Memory/Allocator.hpp"

namespace Ark::Memory
{
    /// Process default heap allocator.
    /// Platform backends may use OS allocators (malloc, HeapAlloc, VirtualMemory, etc.).
    class StandardAllocator final : public Allocator
    {
    public:
        void* allocate(uint size) override;

        void deallocate(void* ptr) override;

        void* allocateAligned(uint size, uint alignment) override;

        void deallocateAligned(void* ptr) override;
    };
}
