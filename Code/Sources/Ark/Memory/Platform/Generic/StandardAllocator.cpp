#include "Ark/Memory/StandardAllocator.hpp"
#include "Ark/Core/NumericLimits.hpp"
#include "Ark/Memory/Functions.hpp"
#include "Ark/Memory/VirtualMemory.hpp"

namespace Ark::Memory
{
    namespace
    {
        struct AllocationHeader
        {
            void* reservationBase{nullptr};
            usize mappedSize{0};
        };

        constexpr usize kDefaultAlignment = 16;

        void* allocateWithAlignment(usize size, usize alignment)
        {
            if (size == 0 || alignment == 0 || !isPowerOfTwo(alignment))
            {
                return nullptr;
            }

            usize const headerBytes = sizeof(AllocationHeader);
            // Extra alignment bytes so an aligned user pointer can sit after a header.
            usize const total = headerBytes + alignment + size;
            if (total < size)
            {
                return nullptr;
            }

            void* const reservation = reserve(total, 4096);
            if (reservation == nullptr)
            {
                return nullptr;
            }

            if (!commit(reservation, total, Protect::ReadWrite))
            {
                release(reservation, total);
                return nullptr;
            }

            uintptr const minUser = reinterpret_cast<uintptr>(reservation) + headerBytes;
            uintptr const alignedUser = (minUser + alignment - 1) & ~(static_cast<uintptr>(alignment) - 1);
            auto* header = reinterpret_cast<AllocationHeader*>(alignedUser - headerBytes);
            header->reservationBase = reservation;
            header->mappedSize = total;
            return reinterpret_cast<void*>(alignedUser);
        }

        void freeAllocation(void* ptr)
        {
            if (ptr == nullptr)
            {
                return;
            }

            auto* header = reinterpret_cast<AllocationHeader*>(
                reinterpret_cast<uintptr>(ptr) - sizeof(AllocationHeader));
            void* const base = header->reservationBase;
            usize const mappedSize = header->mappedSize;
            release(base, mappedSize);
        }
    }

    void* StandardAllocator::allocate(uint size)
    {
        return allocateWithAlignment(static_cast<usize>(size), kDefaultAlignment);
    }

    void StandardAllocator::deallocate(void* ptr)
    {
        freeAllocation(ptr);
    }

    void* StandardAllocator::allocateAligned(uint size, uint alignment)
    {
        if (alignment < sizeof(void*))
        {
            alignment = static_cast<uint>(sizeof(void*));
        }

        if (!isPowerOfTwo(static_cast<usize>(alignment)))
        {
            return nullptr;
        }

        usize const alignedSize = alignSize(static_cast<usize>(size), static_cast<usize>(alignment));
        if (alignedSize < static_cast<usize>(size) || alignedSize > static_cast<usize>(NumericLimits<uint32>::max()))
        {
            return nullptr;
        }

        return allocateWithAlignment(alignedSize, static_cast<usize>(alignment));
    }

    void StandardAllocator::deallocateAligned(void* ptr)
    {
        freeAllocation(ptr);
    }
}
