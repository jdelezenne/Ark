#pragma once

#include "Ark/Core/Types.hpp"
#include "Ark/Memory/Allocator.hpp"
#include "Ark/Memory/Functions.hpp"

namespace Ark::Memory
{
    /// A ring-buffer style allocator for frame-based temporary allocations.
    /// Supports fence checkpoints to reclaim memory once consumers are done.
    class FrameAllocator final : public Allocator
    {
    public:
        // Fences: advance returns an opaque value identifying a checkpoint; callers reclaim once safe
        using Fence = usize;

    private:
        byte* start{nullptr};
        usize capacity{0};
        usize headCursor{0};
        usize tailCursor{0};
        usize defaultAlignment{DefaultAlignment};

    public:
        /// Creates a frame allocator over a caller-provided buffer.
        /// @param buffer Backing memory region.
        /// @param capacity Buffer size in bytes.
        /// @param alignment Default alignment used by allocate().
        FrameAllocator(void* buffer, usize capacity, usize alignment = DefaultAlignment);

        void* allocate(uint size) override;
        void deallocate(void* ptr) override;
        void* allocateAligned(uint size, uint alignment) override;
        void deallocateAligned(void* ptr) override;
        void reset() override;

        /// Captures the current allocation frontier as a fence value.
        /// @return Opaque fence token usable with advanceTo().
        Fence markFence() const;

        /// Reclaims allocations up to the specified fence.
        /// @param fence Fence token previously returned by markFence().
        void advanceTo(Fence fence);
    };
}
