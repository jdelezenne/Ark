#pragma once

#include "Ark/Core/Types.hpp"
#include "Ark/Memory/Allocator.hpp"
#include "Ark/Memory/Functions.hpp"

namespace Ark::Memory
{
    /// A bump-pointer allocator over a fixed buffer.
    /// Individual deallocation is unsupported; use mark()/rollback() or reset().
    class LinearAllocator final : public Allocator
    {
    public:
        /// Checkpoint used to restore allocator state.
        struct Marker
        {
            usize offset{0};
        };

    private:
        byte* start{nullptr};
        usize capacity{0};
        usize head{0};
        usize defaultAlignment{DefaultAlignment};

    public:
        /// Creates a linear allocator over a caller-provided buffer.
        /// @param buffer Backing memory region.
        /// @param capacity Buffer size in bytes.
        /// @param alignment Default alignment used by allocate().
        LinearAllocator(void* buffer, usize capacity, usize alignment = DefaultAlignment);

        void* allocate(uint size) override;
        void deallocate(void* ptr) override;
        void* allocateAligned(uint size, uint alignment) override;
        void deallocateAligned(void* ptr) override;
        void reset() override;

        /// Captures the current allocation offset.
        /// @return Marker representing the current state.
        Marker mark() const;

        /// Restores the allocator to a previous marker.
        /// @param m Marker returned by mark().
        void rollback(Marker m);
    };

    /// RAII helper that rolls back a LinearAllocator on scope exit.
    struct LinearScope
    {
        LinearAllocator& arena;
        LinearAllocator::Marker marker;

        /// Captures allocator state on construction.
        /// @param a Allocator to scope.
        explicit LinearScope(LinearAllocator& a)
            : arena{a}
            , marker{a.mark()}
        {
        }

        /// Rolls back the allocator to the captured marker.
        ~LinearScope()
        {
            arena.rollback(marker);
        }
    };
}
