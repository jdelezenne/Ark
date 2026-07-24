#pragma once

#include "Ark/Core/Types.hpp"
#include "Ark/Memory/Allocator.hpp"
#include "Ark/Memory/Functions.hpp"

namespace Ark::Memory
{
    /// Growable bump-pointer allocator composed of multiple blocks.
    class GrowingLinearAllocator final : public Allocator
    {
    private:
        struct Block
        {
            byte* base{nullptr};
            usize capacity{0};
            usize head{0};
            Block* prev{nullptr};
        };

    public:
        /// Checkpoint used to restore allocator state.
        struct Marker
        {
            void* block;
            usize head{0};
        };

    private:
        Allocator& parent;
        Block* tail{nullptr};
        usize defaultAlignment{DefaultAlignment};
        usize initialBlockSize{0};
        usize maxBlockSize{0};

    public:
        /// Creates a growable linear allocator backed by a parent allocator.
        /// @param parent Parent allocator used to acquire blocks.
        /// @param initialBlockSize Size of the first allocated block.
        /// @param alignment Default alignment used by allocate().
        /// @param maxBlockSize Maximum block size used during growth.
        GrowingLinearAllocator(Allocator& parent, usize initialBlockSize = static_cast<usize>(64) * static_cast<usize>(1024), usize alignment = DefaultAlignment, usize maxBlockSize = static_cast<usize>(8) * static_cast<usize>(1024) * static_cast<usize>(1024));
        ~GrowingLinearAllocator() override;

        void* allocate(uint size) override;
        void deallocate(void* ptr) override;
        void* allocateAligned(uint size, uint alignment) override;
        void deallocateAligned(void* ptr) override;
        void reset() override;

        /// Captures the current allocation state.
        /// @return Marker representing the current tail block and head offset.
        Marker mark() const;

        /// Restores allocator state to a previously captured marker.
        /// Stale markers that refer to already-released blocks are ignored.
        /// @param m Marker returned by mark().
        void rollback(Marker m);

    private:
        bool ensureCapacity(usize size, usize alignment);
        bool containsBlock(Block const* block) const;
        void releaseBlocksAfter(Block* keepTail);
        static usize growSize(usize request, usize previousSize, usize initialSize, usize maxSize);
    };

    /// RAII helper that rolls back a GrowingLinearAllocator on scope exit.
    struct GrowingLinearScope
    {
        GrowingLinearAllocator& allocator;
        GrowingLinearAllocator::Marker marker;

        /// Captures allocator state on construction.
        /// @param a Allocator to scope.
        explicit GrowingLinearScope(GrowingLinearAllocator& a)
            : allocator{a}
            , marker{a.mark()}
        {
        }

        /// Rolls back the allocator to the captured marker.
        ~GrowingLinearScope()
        {
            allocator.rollback(marker);
        }
    };
}
