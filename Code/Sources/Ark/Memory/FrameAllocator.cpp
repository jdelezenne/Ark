#include "Ark/Memory/FrameAllocator.hpp"

namespace Ark::Memory
{
    FrameAllocator::FrameAllocator(void* buffer, usize cap, usize alignment)
        : start{static_cast<byte*>(buffer)}
        , capacity{cap}
        , headCursor{0}
        , tailCursor{0}
        , defaultAlignment{alignment}
    {
    }

    void* FrameAllocator::allocate(uint size)
    {
        return allocateAligned(size, static_cast<uint>(defaultAlignment));
    }

    void FrameAllocator::deallocate(void*)
    {
        // no-op; reclamation via advanceTo
    }

    void* FrameAllocator::allocateAligned(uint size, uint alignment)
    {
        if (size == 0 || alignment == 0 || !isPowerOfTwo(static_cast<usize>(alignment)))
        {
            return nullptr;
        }

        if (capacity == 0)
        {
            return nullptr;
        }

        usize used = headCursor - tailCursor;
        if (used > capacity)
        {
            return nullptr;
        }

        usize index = headCursor % capacity;
        uintptr base = reinterpret_cast<uintptr>(start);
        usize alignedIndex = alignUp(base + index, alignment) - base;

        // Allocation without wrapping.
        if (alignedIndex + size <= capacity)
        {
            usize advance = (alignedIndex - index) + static_cast<usize>(size);
            if (used + advance <= capacity)
            {
                void* ptr = start + alignedIndex;
                headCursor += advance;
                return ptr;
            }
        }

        // Wrap to the beginning and allocate there.
        usize alignedStart = alignUp(base, alignment) - base;
        usize wrapAdvance = (capacity - index) + alignedStart + static_cast<usize>(size);
        if (alignedStart + size > capacity || used + wrapAdvance > capacity)
        {
            return nullptr;
        }

        void* ptr = start + alignedStart;
        headCursor += wrapAdvance;
        return ptr;
    }

    void FrameAllocator::deallocateAligned(void*)
    {
        // no-op
    }

    void FrameAllocator::reset()
    {
        headCursor = 0;
        tailCursor = 0;
    }

    FrameAllocator::Fence FrameAllocator::markFence() const
    {
        return headCursor;
    }

    void FrameAllocator::advanceTo(Fence fence)
    {
        if (fence < tailCursor)
        {
            return;
        }

        if (fence > headCursor)
        {
            tailCursor = headCursor;
            return;
        }

        // Caller guarantees all allocations up to 'fence' are no longer in use.
        tailCursor = fence;
    }
}
