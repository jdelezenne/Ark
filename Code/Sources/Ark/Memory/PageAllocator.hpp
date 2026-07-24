#pragma once

#include "Ark/Core/Types.hpp"
#include "Ark/Memory/Allocator.hpp"
#include "Ark/Memory/Functions.hpp"
#include "Ark/Memory/VirtualMemory.hpp"

namespace Ark::Memory
{
    /// Allocator that reserves and commits memory in page-aligned regions.
    /// Intended for large allocations where virtual-memory semantics are required.
    class PageAllocator final : public Allocator
    {
    private:
        struct AllocationHeader
        {
            void* reservationBase{nullptr};
            usize reservationSize{0};
        };

        usize pageAlignment;

    public:
        /// Creates a page allocator.
        /// @param pageAlignment Page alignment used for reservations and commits.
        explicit PageAllocator(usize pageAlignment = 4096)
            : pageAlignment{pageAlignment}
        {
        }

        void* allocate(uint size) override
        {
            return allocateAligned(size, static_cast<uint>(pageAlignment));
        }

        void deallocate(void* ptr) override
        {
            deallocateAligned(ptr);
        }

        void* allocateAligned(uint size, uint alignment) override
        {
            if (size == 0 || alignment == 0 || !isPowerOfTwo(static_cast<usize>(alignment)))
            {
                return nullptr;
            }

            usize align = static_cast<usize>(alignment) < pageAlignment ? pageAlignment : static_cast<usize>(alignment);
            usize payloadSize = alignSize(static_cast<usize>(size), align);
            usize totalSize = alignSize(payloadSize + sizeof(AllocationHeader) + align - 1, pageAlignment);

            void* reservation = reserve(totalSize, pageAlignment);
            if (reservation == nullptr)
            {
                return nullptr;
            }

            if (!commit(reservation, totalSize, Protect::ReadWrite))
            {
                release(reservation, totalSize);
                return nullptr;
            }

            uintptr base = reinterpret_cast<uintptr>(reservation);
            uintptr userAddress = alignUp(base + sizeof(AllocationHeader), static_cast<uint>(align));
            auto* header = reinterpret_cast<AllocationHeader*>(userAddress - sizeof(AllocationHeader));
            header->reservationBase = reservation;
            header->reservationSize = totalSize;
            return reinterpret_cast<void*>(userAddress);
        }

        void deallocateAligned(void* ptr) override
        {
            if (ptr == nullptr)
            {
                return;
            }

            auto* header = reinterpret_cast<AllocationHeader*>(static_cast<byte*>(ptr) - sizeof(AllocationHeader));
            release(header->reservationBase, header->reservationSize);
        }

        void reset() override
        {
        }

        /// Returns the configured page alignment.
        /// @return Page alignment in bytes.
        usize getPageAlignment() const
        {
            return pageAlignment;
        }
    };
}
