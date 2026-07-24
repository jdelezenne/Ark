#include "Ark/Memory/StandardAllocator.hpp"
#include "Ark/Memory/Functions.hpp"

#include <cstdlib>

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
        void* result = nullptr;

        if (alignment < sizeof(void*))
        {
            alignment = static_cast<uint>(sizeof(void*));
        }

        // POSIX requires alignment to be a power of two and a multiple of sizeof(void*)
        if ((alignment % sizeof(void*)) != 0)
        {
            return nullptr;
        }

        if (!Ark::Memory::isPowerOfTwo(static_cast<Ark::usize>(alignment)))
        {
            return nullptr;
        }

        int rc = ::posix_memalign(&result, alignment, size);
        if (rc != 0)
        {
            return nullptr;
        }

        return result;
    }

    void StandardAllocator::deallocateAligned(void* ptr)
    {
        std::free(ptr);
    }
}
