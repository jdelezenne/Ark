#pragma once

#include "Ark/Core/Debug/Assert.hpp"
#include "Ark/Core/Functions.hpp"
#include "Ark/Core/NumericLimits.hpp"
#include "Ark/Core/Types.hpp"

#include <new>

namespace Ark::Memory
{
    /// Interface for allocator implementations.
    /// @details
    /// Support for both standard and aligned memory operations.
    class Allocator
    {
    public:
        virtual ~Allocator() = default;

        /// Allocates a block of memory of the specified size.
        /// @param size The size of the memory block to allocate.
        /// @return A pointer to the allocated memory block, or `nullptr` if allocation fails.
        virtual void* allocate(uint size) = 0;

        /// Allocates a block of memory.
        /// @tparam T The type of the memory block to allocate.
        /// @return A pointer to the allocated memory block, or `nullptr` if allocation fails.
        template <typename T>
        T* allocate()
        {
            return static_cast<T*>(allocate(sizeof(T)));
        }

        /// Allocates an array of elements.
        /// @tparam T The element type.
        /// @param count The number of elements to allocate.
        /// @return A pointer to the allocated array, or `nullptr` if allocation fails.
        template <typename T>
        T* allocateArray(usize count)
        {
            if (count == 0)
            {
                return nullptr;
            }

            if (count > (~static_cast<usize>(0)) / sizeof(T))
            {
                return nullptr;
            }

            usize const bytes = sizeof(T) * count;
            if (bytes > static_cast<usize>(NumericLimits<uint32>::max()))
            {
                return nullptr;
            }

            return static_cast<T*>(allocate(static_cast<uint>(bytes)));
        }

        /// Deallocates a previously allocated block of memory.
        /// @param ptr A pointer to the memory block to deallocate.
        virtual void deallocate(void* ptr) = 0;

        /// Allocates a block of memory of the specified size with the specified alignment.
        /// @param size The size of the memory block to allocate.
        /// @param alignment The alignment requirement for the memory block.
        /// @return A pointer to the allocated memory block, or `nullptr` if allocation fails.
        virtual void* allocateAligned(uint size, uint alignment) = 0;

        /// Deallocates a previously allocated aligned block of memory.
        /// @param ptr A pointer to the aligned memory block to deallocate.
        virtual void deallocateAligned(void* ptr) = 0;

        /// Resets the allocator, making all allocated memory available again.
        virtual void reset()
        {
        }

        template <typename T, typename... Args>
        T* make(Args&&... args)
        {
            T* ptr = allocate<T>();
            if (ptr == nullptr)
            {
                ARK_ASSERT(false);
                return nullptr;
            }

            try
            {
                ::new (static_cast<void*>(ptr)) T(Ark::forward<Args>(args)...);
            }
            catch (...)
            {
                deallocate(ptr);
                throw;
            }

            return ptr;
        }

        template <typename T>
        void destroy(T* ptr)
        {
            ARK_ASSERT(ptr != nullptr);

            if (ptr != nullptr)
            {
                ptr->~T();
                deallocate(ptr);
            }
        }
    };
}
