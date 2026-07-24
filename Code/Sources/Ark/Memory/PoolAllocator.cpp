#include "Ark/Memory/PoolAllocator.hpp"
#include "Ark/Core/Utilities.hpp"
#include "Ark/Memory/Functions.hpp"

namespace Ark::Memory
{
    struct FreeListNode
    {
        FreeListNode* next;
    };

    PoolAllocator::PoolAllocator(void* buffer, usize capacityBytes, usize blkSize, usize alignment)
        : start{nullptr}
        , capacity{0}
        , blockSize{0}
        , freeList{nullptr}
        , defaultAlignment{alignment}
    {
        if (buffer == nullptr || capacityBytes == 0 || blkSize == 0 || alignment == 0 || !isPowerOfTwo(alignment))
        {
            return;
        }

        usize requiredBlockSize = Ark::roundUpToMultiple(blkSize, alignment);
        usize const minBlockSize = Ark::roundUpToMultiple(sizeof(FreeNode), alignment);
        if (requiredBlockSize < minBlockSize)
        {
            requiredBlockSize = minBlockSize;
        }
        if (requiredBlockSize == 0)
        {
            return;
        }

        uintptr const bufferAddress = reinterpret_cast<uintptr>(buffer);
        uintptr const alignedAddress = alignUp(bufferAddress, static_cast<uint>(alignment));
        if (alignedAddress < bufferAddress)
        {
            return;
        }

        usize const leading = static_cast<usize>(alignedAddress - bufferAddress);
        if (leading >= capacityBytes)
        {
            return;
        }

        start = reinterpret_cast<byte*>(alignedAddress);
        capacity = capacityBytes - leading;
        blockSize = requiredBlockSize;
        defaultAlignment = alignment;

        usize const blockCount = capacity / blockSize;
        byte* ptr = start;
        for (usize i = 0; i < blockCount; ++i)
        {
            auto* node = reinterpret_cast<FreeNode*>(ptr);
            node->next = freeList;
            freeList = node;
            ptr += blockSize;
        }
    }

    static inline bool isPointerInPool(byte* start, usize capacity, usize blockSize, void* ptr)
    {
        if (blockSize == 0 || start == nullptr)
        {
            return false;
        }

        byte* value = static_cast<byte*>(ptr);
        if (value < start || value >= (start + capacity))
        {
            return false;
        }

        usize offset = static_cast<usize>(value - start);
        return (offset % blockSize) == 0;
    }

    static inline bool isNodeInFreeList(FreeListNode* freeList, FreeListNode* node)
    {
        for (FreeListNode* current = freeList; current != nullptr; current = current->next)
        {
            if (current == node)
            {
                return true;
            }
        }
        return false;
    }

    void* PoolAllocator::allocate(uint size)
    {
        if (blockSize == 0 || size > blockSize || freeList == nullptr)
        {
            return nullptr;
        }
        FreeNode* node = freeList;
        freeList = freeList->next;
        return node;
    }

    void PoolAllocator::deallocate(void* ptr)
    {
        if (ptr == nullptr || blockSize == 0)
        {
            return;
        }

        bool validPointer = isPointerInPool(start, capacity, blockSize, ptr);
        if (!validPointer)
        {
            return;
        }

        auto* node = reinterpret_cast<FreeNode*>(ptr);

        bool alreadyFreed = isNodeInFreeList(reinterpret_cast<FreeListNode*>(freeList), reinterpret_cast<FreeListNode*>(node));
        if (alreadyFreed)
        {
            return;
        }

        node->next = freeList;
        freeList = node;
    }

    void* PoolAllocator::allocateAligned(uint size, uint alignment)
    {
        if (alignment > defaultAlignment)
        {
            return nullptr;
        }
        return allocate(size);
    }

    void PoolAllocator::deallocateAligned(void* ptr)
    {
        deallocate(ptr);
    }

    void PoolAllocator::reset()
    {
        freeList = nullptr;
        if (blockSize == 0 || start == nullptr)
        {
            return;
        }

        usize const blockCount = capacity / blockSize;
        byte* ptr = start;
        for (usize i = 0; i < blockCount; ++i)
        {
            auto* node = reinterpret_cast<FreeNode*>(ptr);
            node->next = freeList;
            freeList = node;
            ptr += blockSize;
        }
    }
}
