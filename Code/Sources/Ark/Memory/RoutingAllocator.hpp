#pragma once

#include "Ark/Core/NumericLimits.hpp"
#include "Ark/Core/Types.hpp"
#include "Ark/Memory/Allocator.hpp"
#include "Ark/Memory/Functions.hpp"

namespace Ark::Memory
{
    /// Allocator that routes requests to small or large backing allocators.
    /// Routing is based on a configurable allocation size threshold.
    class RoutingAllocator final : public Allocator
    {
    private:
        struct AllocationHeader
        {
            void* raw{nullptr};
            uint8 route{0};
            uint8 aligned{0};
        };

        static constexpr uint8 RouteSmall = 0;
        static constexpr uint8 RouteLarge = 1;

        Allocator& small;
        Allocator& large;
        usize threshold;

    public:
        /// Creates a routing allocator.
        /// @param smallObject Allocator used for small allocations.
        /// @param largeObject Allocator used for large allocations.
        /// @param smallThreshold Maximum size routed to smallObject.
        RoutingAllocator(Allocator& smallObject, Allocator& largeObject, usize smallThreshold)
            : small{smallObject}
            , large{largeObject}
            , threshold{smallThreshold}
        {
        }

        void* allocate(uint size) override
        {
            if (size == 0)
            {
                return nullptr;
            }

            bool useSmall = size <= threshold;
            Allocator& target = useSmall ? small : large;

            usize total = static_cast<usize>(size) + sizeof(AllocationHeader);
            void* raw = target.allocate(static_cast<uint>(total));
            if (raw == nullptr)
            {
                return nullptr;
            }

            auto* header = static_cast<AllocationHeader*>(raw);
            header->raw = raw;
            header->route = useSmall ? RouteSmall : RouteLarge;
            header->aligned = 0;
            return static_cast<byte*>(raw) + sizeof(AllocationHeader);
        }

        void deallocate(void* ptr) override
        {
            if (ptr == nullptr)
            {
                return;
            }

            auto* header = reinterpret_cast<AllocationHeader*>(static_cast<byte*>(ptr) - sizeof(AllocationHeader));
            if (header->route == RouteSmall)
            {
                header->aligned != 0 ? small.deallocateAligned(header->raw) : small.deallocate(header->raw);
            }
            else
            {
                header->aligned != 0 ? large.deallocateAligned(header->raw) : large.deallocate(header->raw);
            }
        }

        void* allocateAligned(uint size, uint alignment) override
        {
            if (size == 0 || alignment == 0 || !isPowerOfTwo(static_cast<usize>(alignment)))
            {
                return nullptr;
            }

            // Keep the header immediately before the user pointer and suitably aligned.
            uint const headerAlignment = static_cast<uint>(alignof(AllocationHeader));
            if (alignment < headerAlignment)
            {
                alignment = headerAlignment;
            }

            bool useSmall = size <= threshold;
            Allocator& target = useSmall ? small : large;

            usize total = static_cast<usize>(size) + sizeof(AllocationHeader) + static_cast<usize>(alignment) - 1;
            if (total < static_cast<usize>(size) || total > static_cast<usize>(NumericLimits<uint32>::max()))
            {
                return nullptr;
            }

            void* raw = target.allocateAligned(static_cast<uint>(total), alignment);
            if (raw == nullptr)
            {
                return nullptr;
            }

            uintptr rawAddr = reinterpret_cast<uintptr>(raw);
            uintptr userAddr = alignUp(rawAddr + sizeof(AllocationHeader), alignment);
            auto* header = reinterpret_cast<AllocationHeader*>(userAddr - sizeof(AllocationHeader));
            header->raw = raw;
            header->route = useSmall ? RouteSmall : RouteLarge;
            header->aligned = 1;
            return reinterpret_cast<void*>(userAddr);
        }

        void deallocateAligned(void* ptr) override
        {
            deallocate(ptr);
        }

        void reset() override
        {
            small.reset();
            large.reset();
        }

        /// Updates the routing threshold.
        /// @param value Maximum size routed to the small allocator.
        void setThreshold(usize value)
        {
            threshold = value;
        }

        /// Returns the current routing threshold.
        /// @return Maximum size routed to the small allocator.
        usize getThreshold() const
        {
            return threshold;
        }
    };
}
