#include "Ark/Memory/LinearAllocator.hpp"
#include "Ark/Memory/Functions.hpp"

namespace Ark::Memory
{
    LinearAllocator::LinearAllocator(void* buffer, usize cap, usize alignment)
        : start{static_cast<byte*>(buffer)}
        , capacity{cap}
        , head{0}
        , defaultAlignment{alignment}
    {
    }

    void* LinearAllocator::allocate(uint size)
    {
        return allocateAligned(size, static_cast<uint>(defaultAlignment));
    }

    void LinearAllocator::deallocate(void*)
    {
        // no-op; use reset or rollback
    }

    void* LinearAllocator::allocateAligned(uint size, uint alignment)
    {
        if (size == 0 || alignment == 0 || !isPowerOfTwo(static_cast<usize>(alignment)))
        {
            return nullptr;
        }

        usize const current = head;
        usize const aligned = alignUp(reinterpret_cast<uintptr>(start) + current, alignment) - reinterpret_cast<uintptr>(start);
        if (aligned < current || aligned > capacity)
        {
            return nullptr;
        }

        if (static_cast<usize>(size) > capacity - aligned)
        {
            return nullptr;
        }

        head = aligned + static_cast<usize>(size);
        return start + aligned;
    }

    void LinearAllocator::deallocateAligned(void*)
    {
        // no-op; use reset or rollback
    }

    void LinearAllocator::reset()
    {
        head = 0;
    }

    LinearAllocator::Marker LinearAllocator::mark() const
    {
        return Marker{head};
    }

    void LinearAllocator::rollback(Marker m)
    {
        if (m.offset <= capacity)
        {
            head = m.offset;
        }
    }
}
