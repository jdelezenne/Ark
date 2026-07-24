#include "Ark/Memory/GrowingLinearAllocator.hpp"
#include "Ark/Memory/Functions.hpp"

namespace Ark::Memory
{
    GrowingLinearAllocator::GrowingLinearAllocator(Allocator& p, usize initialSize, usize alignment, usize maxSize)
        : parent{p}
        , tail{nullptr}
        , defaultAlignment{alignment}
        , initialBlockSize{alignSize(initialSize, alignment)}
        , maxBlockSize{alignSize(maxSize, alignment)}
    {
    }

    GrowingLinearAllocator::~GrowingLinearAllocator()
    {
        reset();
    }

    void* GrowingLinearAllocator::allocate(uint size)
    {
        return allocateAligned(size, static_cast<uint>(defaultAlignment));
    }

    void GrowingLinearAllocator::deallocate(void*)
    {
        // no-op; use reset or rollback
    }

    bool GrowingLinearAllocator::ensureCapacity(usize size, usize alignment)
    {
        if (tail != nullptr)
        {
            uintptr base = reinterpret_cast<uintptr>(tail->base);
            uintptr alignedHead = alignUp(base + tail->head, static_cast<uint>(alignment));
            usize padding = alignedHead - (base + tail->head);
            if (tail->head <= tail->capacity)
            {
                usize remaining = tail->capacity - tail->head;
                if (padding + size <= remaining)
                {
                    return true;
                }
            }
        }

        usize request = size + alignment + DefaultAlignment;
        if (request < size)
        {
            return false;
        }
        if (request > maxBlockSize)
        {
            return false;
        }

        usize previousSize = (tail != nullptr) ? tail->capacity : 0;
        usize blockSize = growSize(request, previousSize, initialBlockSize, maxBlockSize);
        if (blockSize < request)
        {
            return false;
        }

        void* mem = parent.allocateAligned(static_cast<uint>(blockSize), static_cast<uint>(defaultAlignment));
        if (mem == nullptr)
        {
            return false;
        }

        void* blkMem = parent.allocate(sizeof(Block));
        if (blkMem == nullptr)
        {
            parent.deallocateAligned(mem);
            return false;
        }

        auto* b = static_cast<Block*>(blkMem);
        b->base = static_cast<byte*>(mem);
        b->capacity = blockSize;
        b->head = 0;
        b->prev = tail;
        tail = b;
        return true;
    }

    void* GrowingLinearAllocator::allocateAligned(uint size, uint alignment)
    {
        if (size == 0)
        {
            return nullptr;
        }
        if (!isPowerOfTwo(alignment))
        {
            return nullptr;
        }

        if (!ensureCapacity(static_cast<usize>(size), static_cast<usize>(alignment)))
        {
            return nullptr;
        }

        uintptr base = reinterpret_cast<uintptr>(tail->base);
        uintptr alignedHead = alignUp(base + tail->head, alignment);
        usize padding = alignedHead - (base + tail->head);
        if (padding > tail->capacity - tail->head)
        {
            return nullptr;
        }
        if (static_cast<usize>(size) > (tail->capacity - tail->head - padding))
        {
            return nullptr;
        }

        tail->head += padding;
        void* ptr = tail->base + tail->head;
        tail->head += static_cast<usize>(size);
        return ptr;
    }

    void GrowingLinearAllocator::deallocateAligned(void*)
    {
        // no-op; use reset or rollback
    }

    void GrowingLinearAllocator::reset()
    {
        releaseBlocksAfter(nullptr);
        tail = nullptr;
    }

    GrowingLinearAllocator::Marker GrowingLinearAllocator::mark() const
    {
        return Marker{tail, tail != nullptr ? tail->head : 0};
    }

    bool GrowingLinearAllocator::containsBlock(Block const* block) const
    {
        for (Block const* current = tail; current != nullptr; current = current->prev)
        {
            if (current == block)
            {
                return true;
            }
        }
        return false;
    }

    void GrowingLinearAllocator::rollback(Marker m)
    {
        Block* target = static_cast<Block*>(m.block);
        if (target == nullptr)
        {
            reset();
            return;
        }

        // Reject stale markers that refer to blocks already released by an earlier rollback.
        if (!containsBlock(target))
        {
            return;
        }

        releaseBlocksAfter(target);
        tail = target;
        if (m.head <= tail->capacity)
        {
            tail->head = m.head;
        }
    }

    void GrowingLinearAllocator::releaseBlocksAfter(Block* keepTail)
    {
        while (tail != nullptr && tail != keepTail)
        {
            Block* prev = tail->prev;
            parent.deallocateAligned(tail->base);
            parent.deallocate(tail);
            tail = prev;
        }
    }

    usize GrowingLinearAllocator::growSize(usize request, usize previousSize, usize initialSize, usize maxSize)
    {
        usize base = (previousSize == 0) ? initialSize : previousSize * 2;
        if (base < request)
        {
            base = request;
        }
        if (base > maxSize)
        {
            base = maxSize;
        }
        return alignSize(base, DefaultAlignment);
    }
}
