#pragma once

#include "Ark/Collections/HashMap.hpp"
#include "Ark/Core/Types.hpp"
#include "Ark/Memory/Allocator.hpp"

namespace Ark::Memory
{
    /// Decorator allocator that tracks allocation statistics.
    /// Forwards allocations to a parent allocator while recording counters.
    /// Bookkeeping storage uses the process default allocator, not `parent`.
    class TrackingAllocator final : public Allocator
    {
    private:
        struct AllocationRecord
        {
            usize size{0};
            bool aligned{false};
        };

        Allocator& parent;
        usize bytesAllocated{0};
        usize peakBytesAllocated{0};
        usize allocationCount{0};
        Collections::HashMap<void*, AllocationRecord> liveAllocations;

    public:
        /// Creates a tracking allocator.
        /// @param parent Allocator used for actual allocation operations.
        explicit TrackingAllocator(Allocator& parent);

        void* allocate(uint size) override;
        void deallocate(void* ptr) override;
        void* allocateAligned(uint size, uint alignment) override;
        void deallocateAligned(void* ptr) override;
        void reset() override;

        /// Returns current bytes believed to be allocated.
        /// @return Number of bytes currently tracked as allocated.
        usize getBytesAllocated() const;

        /// Returns the high-water mark of tracked allocated bytes.
        /// @return Peak tracked allocation size in bytes.
        usize getPeakBytesAllocated() const;

        /// Returns the number of allocation calls observed.
        /// @return Allocation call count.
        usize getAllocationCount() const;
    };
}
