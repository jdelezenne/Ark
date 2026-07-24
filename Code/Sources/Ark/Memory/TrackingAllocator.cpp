#include "Ark/Memory/TrackingAllocator.hpp"
#include "Ark/Memory/Memory.hpp"

namespace Ark::Memory
{
    TrackingAllocator::TrackingAllocator(Allocator& parentAllocator)
        : parent{parentAllocator}
        , liveAllocations{getDefaultAllocator()}
    {
    }

    void* TrackingAllocator::allocate(uint size)
    {
        void* p = parent.allocate(size);
        if (p != nullptr)
        {
            liveAllocations.insertOrAssign(p, AllocationRecord{size, false});
            bytesAllocated += size;
            if (bytesAllocated > peakBytesAllocated)
            {
                peakBytesAllocated = bytesAllocated;
            }
            allocationCount += 1;
        }
        return p;
    }

    void TrackingAllocator::deallocate(void* ptr)
    {
        if (ptr == nullptr)
        {
            return;
        }

        auto allocation = liveAllocations.tryGet(ptr);
        if (allocation.hasValue())
        {
            bytesAllocated -= allocation.getValue().size;
            bool const aligned = allocation.getValue().aligned;
            liveAllocations.remove(ptr);
            if (aligned)
            {
                parent.deallocateAligned(ptr);
            }
            else
            {
                parent.deallocate(ptr);
            }
            return;
        }

        parent.deallocate(ptr);
    }

    void* TrackingAllocator::allocateAligned(uint size, uint alignment)
    {
        void* p = parent.allocateAligned(size, alignment);
        if (p != nullptr)
        {
            liveAllocations.insertOrAssign(p, AllocationRecord{size, true});
            bytesAllocated += size;
            if (bytesAllocated > peakBytesAllocated)
            {
                peakBytesAllocated = bytesAllocated;
            }
            allocationCount += 1;
        }
        return p;
    }

    void TrackingAllocator::deallocateAligned(void* ptr)
    {
        if (ptr == nullptr)
        {
            return;
        }

        auto allocation = liveAllocations.tryGet(ptr);
        if (allocation.hasValue())
        {
            bytesAllocated -= allocation.getValue().size;
            liveAllocations.remove(ptr);
        }

        parent.deallocateAligned(ptr);
    }

    void TrackingAllocator::reset()
    {
        // Free any still-tracked allocations through the correct parent path first.
        auto it = liveAllocations.begin();
        while (it != liveAllocations.end())
        {
            void* ptr = it->first;
            AllocationRecord const record = it->second;
            it = liveAllocations.remove(it);
            if (record.aligned)
            {
                parent.deallocateAligned(ptr);
            }
            else
            {
                parent.deallocate(ptr);
            }
        }

        bytesAllocated = 0;
        peakBytesAllocated = 0;
        allocationCount = 0;
        parent.reset();
    }

    usize TrackingAllocator::getBytesAllocated() const
    {
        return bytesAllocated;
    }

    usize TrackingAllocator::getPeakBytesAllocated() const
    {
        return peakBytesAllocated;
    }

    usize TrackingAllocator::getAllocationCount() const
    {
        return allocationCount;
    }
}
