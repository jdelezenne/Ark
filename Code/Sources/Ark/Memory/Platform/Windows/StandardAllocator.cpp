#include "Ark/Memory/StandardAllocator.hpp"

#include <malloc.h>

namespace Ark::Memory
{
    void* StandardAllocator::allocate(uint size)
    {
        return std::malloc(size);
    }

    void StandardAllocator::deallocate(void* ptr)
    {
        std::free(ptr);
    }

    void* StandardAllocator::allocateAligned(uint size, uint alignment)
    {
        return _aligned_malloc(size, alignment);
    }

    void StandardAllocator::deallocateAligned(void* ptr)
    {
        _aligned_free(ptr);
    }
}
