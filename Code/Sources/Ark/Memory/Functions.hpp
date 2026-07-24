#pragma once

#include "Ark/Core/Comparison.hpp"
#include "Ark/Core/Concepts.hpp"
#include "Ark/Core/Configuration.hpp"
#include "Ark/Core/Debug/Assert.hpp"
#include "Ark/Core/NumericLimits.hpp"
#include "Ark/Core/Types.hpp"
#include "Ark/Memory/Allocator.hpp"

#if (ARK_USE_STANDARD_LIBRARY == ARK_STANDARD_LIBRARY_CPP)
#include "Ark/Memory/Platform/CppStd/Functions.hpp"
#elif (ARK_USE_STANDARD_LIBRARY == ARK_STANDARD_LIBRARY_C)
#include "Ark/Memory/Platform/CStd/Functions.hpp"
#else
#include "Ark/Memory/Platform/Generic/Functions.hpp"
#endif

namespace Ark::Memory
{
    /// Default alignment value for memory operations.
    inline constexpr usize DefaultAlignment = alignof(float64);

    /// Default alignment requirement of the specified type.
    /// @tparam T The type for which the alignment is determined.
    template <typename T>
    inline constexpr usize TypeAlignment = alignof(T);

    inline void* copy(void const* source, void* destination, uint size);

    inline void* move(void const* source, void* destination, uint size);

    inline void* set(void* destination, uint size, int value);

    inline void zero(void* destination, uint size);

    inline Ordering compare(void const* lhs, void const* rhs, uint size);

    inline bool find(void const* source, byte value, uint size);

    /// Copies a block of memory from source to destination.
    /// @param source The source memory block.
    /// @param destination The destination memory block.
    /// @param size The size of the memory block to copy.
    /// @returns The destination memory block.
    template <typename T>
    inline T* copyCount(T const* source, T* destination, uint size)
    {
        ARK_DEBUG_ASSERT(size == 0 || (source != nullptr && destination != nullptr));

        T const* srcPtr = static_cast<T const*>(source);
        T* destPtr = static_cast<T*>(destination);

        for (uint i = 0; i < size; ++i)
        {
            destPtr[i] = srcPtr[i];
        }

        return destination;
    }

    /// Moves a block of memory from source to destination.
    /// @param source The source memory block.
    /// @param destination The destination memory block.
    /// @param size The size of the memory block to move.
    /// @returns The destination memory block.
    template <typename T>
    inline T* moveCount(T const* source, T* destination, uint size)
    {
        ARK_DEBUG_ASSERT(size == 0 || (source != nullptr && destination != nullptr));

        if (destination < source)
        {
            for (uint i = 0; i < size; ++i)
            {
                destination[i] = source[i];
            }
        }
        else if (destination > source)
        {
            for (int i = static_cast<int>(size) - 1; i >= 0; --i)
            {
                destination[i] = source[i];
            }
        }

        return destination;
    }

    /// Sets a block of memory to a specified value.
    /// @param destination The memory block to set.
    /// @param value The value to set.
    /// @param size The size of the memory block.
    /// @returns The destination memory block.
    template <typename T>
    inline T* set(T* destination, T const& value, uint size)
    {
        ARK_DEBUG_ASSERT(size == 0 || destination != nullptr);

        for (uint i = 0; i < size; ++i)
        {
            destination[i] = value;
        }

        return destination;
    }

    /// Compares two blocks of memory.
    /// @param lhs The first memory block.
    /// @param rhs The second memory block.
    /// @param size The size of the memory blocks.
    /// @returns `Ordering::Less` if the first differing value in lhs is less than the corresponding value in rhs.
    /// `Ordering::Equal` if all count values of lhs and rhs are equal.
    /// `Ordering::Greater` if the first differing value in lhs is greater than the corresponding value in rhs.
    template <Concepts::TotallyOrdered T>
    inline Ordering compare(T const* lhs, T const* rhs, uint size)
    {
        ARK_DEBUG_ASSERT(size == 0 || (lhs != nullptr && rhs != nullptr));

        for (uint i = 0; i < size; ++i)
        {
            if (lhs[i] < rhs[i])
            {
                return Ordering::Less;
            }

            if (lhs[i] > rhs[i])
            {
                return Ordering::Greater;
            }
        }

        return Ordering::Equal;
    }

    /// Find bytes in a block of memory.
    /// @param source The memory block to search.
    /// @param value The value to search.
    /// @param size The size of the memory blocks.
    /// @returns `true` if the value is found, otherwise `false`.
    template <Concepts::EqualityComparable T>
    inline bool find(T const* source, T value, uint size)
    {
        ARK_DEBUG_ASSERT(size == 0 || source != nullptr);

        for (uint i = 0; i < size; ++i)
        {
            if (source[i] == value)
            {
                return true;
            }
        }

        return false;
    }

    /// Checks if the given number is a power of 2
    /// @param x The number to check
    /// @return Returns `true` if the number is a power of 2, otherwise `false`.
    inline bool isPowerOfTwo(usize x)
    {
        return (x > 0) && (x & (x - 1)) == 0;
    }

    /// Find the next power of two
    /// @param x The value
    /// @return The next power of two
    inline uint32 nextPowerOfTwo(uint32 x)
    {
        if (x == 0)
        {
            return 1;
        }

        if (isPowerOfTwo(x))
        {
            return x;
        }

        x--;
        x |= x >> 1;
        x |= x >> 2;
        x |= x >> 4;
        x |= x >> 8;
        x |= x >> 16;
        x++;
        return x;
    }

    /// Find the next power of two
    /// @param x The value
    /// @return The next power of two
    inline uint64 nextPowerOfTwo(uint64 x)
    {
        if (x == 0)
        {
            return 1;
        }

        if (isPowerOfTwo(x))
        {
            return x;
        }

        x--;
        x |= x >> 1;
        x |= x >> 2;
        x |= x >> 4;
        x |= x >> 8;
        x |= x >> 16;
        x |= x >> 32;
        x++;
        return x;
    }

    /// Checks if the given address is aligned to the specified alignment.
    /// @param address The address to check.
    /// @param alignment The alignment boundary (must be a power of 2).
    /// @return Returns `true` if the address is aligned, otherwise `false`.
    inline bool isAligned(uintptr address, usize alignment)
    {
        return (address & (alignment - 1)) == 0;
    }

    /// Aligns the given size up to the specified alignment.
    /// @param size The size to align.
    /// @param alignment The alignment boundary (must be a power of 2).
    /// @return The aligned size.
    inline usize alignSize(usize size, usize alignment = DefaultAlignment)
    {
        return (size + (alignment - 1)) & ~(alignment - 1);
    }

    /// Aligns up to the nearest multiple of the specified alignment.
    /// @param address The address to be aligned.
    /// @param alignment The alignment boundary. Must be a power of two.
    /// @return The aligned address, which is the smallest multiple of alignment that is greater than or equal to address.
    inline uintptr alignUp(uintptr address, uint alignment = DefaultAlignment)
    {
        return (address + alignment - 1) & ~(alignment - 1);
    }

    /// Aligns down to the nearest multiple of the specified alignment.
    /// @param address The address to be aligned down.
    /// @param alignment The alignment boundary. Must be a power of two.
    /// @return The aligned address, which is the largest multiple of alignment that is less than or equal to address.
    inline uintptr alignDown(uintptr address, uint alignment = DefaultAlignment)
    {
        return address & ~(alignment - 1);
    }

    /// Calculates the padding needed to align the given address.
    /// @param address The address to align.
    /// @param alignment The alignment boundary. Must be a power of two.
    /// @return The padding needed to align the size.
    inline uintptr alignmentPadding(uintptr address, uint alignment = DefaultAlignment)
    {
        uintptr padding = (alignment - (address % alignment)) % alignment;
        return padding;
    }

    /// Attempts to allocate a block of memory. Returns nullptr on failure or overflow.
    inline void* tryAllocate(Allocator& allocator, usize size)
    {
        if (size > static_cast<usize>(NumericLimits<uint32>::max()))
        {
            return nullptr;
        }

        return allocator.allocate(static_cast<uint>(size));
    }

    template <typename T>
    inline T* tryAllocate(Allocator& allocator, usize count = 1)
    {
        if (count == 0)
        {
            return nullptr;
        }
        if (count > static_cast<usize>(NumericLimits<uint32>::max()) / sizeof(T))
        {
            return nullptr;
        }
        void* p = tryAllocate(allocator, count * sizeof(T));
        return static_cast<T*>(p);
    }

    /// Attempts to allocate an aligned block of memory. Returns nullptr on failure or invalid alignment.
    inline void* tryAllocateAligned(Allocator& allocator, usize size, usize alignment)
    {
        if (size > static_cast<usize>(NumericLimits<uint32>::max()))
        {
            return nullptr;
        }

        if (alignment == 0 || !isPowerOfTwo(alignment) || alignment > static_cast<usize>(NumericLimits<uint32>::max()))
        {
            return nullptr;
        }

        return allocator.allocateAligned(static_cast<uint>(size), static_cast<uint>(alignment));
    }

    template <typename T>
    inline T* tryAllocateAligned(Allocator& allocator, usize count, usize alignment = TypeAlignment<T>)
    {
        if (count == 0)
        {
            return nullptr;
        }
        if (count > static_cast<usize>(NumericLimits<uint32>::max()) / sizeof(T))
        {
            return nullptr;
        }
        void* p = tryAllocateAligned(allocator, count * sizeof(T), alignment);
        return static_cast<T*>(p);
    }

    /// Allocates a zero-initialized block of memory; returns nullptr on failure.
    /// @param allocator The allocator to use.
    /// @param size The size in bytes.
    inline void* allocateZeroed(Allocator& allocator, usize size)
    {
        void* ptr = tryAllocate(allocator, size);
        if (ptr != nullptr)
        {
            zero(ptr, static_cast<uint>(size));
        }

        return ptr;
    }

    template <typename T>
    inline T* allocateZeroed(Allocator& allocator, usize count)
    requires(Concepts::TriviallyCopyable<T>)
    {
        T* p = tryAllocate<T>(allocator, count);
        if (p != nullptr)
        {
            zero(p, static_cast<uint>(count * sizeof(T)));
        }
        return p;
    }

    /// Allocates a zero-initialized aligned block of memory; returns nullptr on failure.
    /// @param allocator The allocator to use.
    /// @param size The size in bytes.
    /// @param alignment The alignment in bytes.
    inline void* allocateZeroedAligned(Allocator& allocator, usize size, usize alignment)
    {
        void* ptr = tryAllocateAligned(allocator, size, alignment);
        if (ptr != nullptr)
        {
            zero(ptr, static_cast<uint>(size));
        }

        return ptr;
    }

    template <typename T>
    inline T* allocateZeroedAligned(Allocator& allocator, usize count, usize alignment = TypeAlignment<T>)
    requires(Concepts::TriviallyCopyable<T>)
    {
        T* p = tryAllocateAligned<T>(allocator, count, alignment);
        if (p != nullptr)
        {
            zero(p, static_cast<uint>(count * sizeof(T)));
        }
        return p;
    }

    /// Reallocates a memory block by allocating a new block, copying, and freeing the old block.
    /// @param allocator The allocator to use.
    /// @param oldPointer The previous pointer (may be nullptr).
    /// @param oldSize The previous size in bytes.
    /// @param newSize The new size in bytes.
    /// @return The new pointer or nullptr if allocation failed (old memory is freed when newSize == 0).
    inline void* reallocate(Allocator& allocator, void* oldPointer, usize oldSize, usize newSize)
    {
        if (oldPointer == nullptr)
        {
            return tryAllocate(allocator, newSize);
        }

        if (newSize == 0)
        {
            allocator.deallocate(oldPointer);
            return nullptr;
        }

        void* newPointer = tryAllocate(allocator, newSize);
        if (newPointer == nullptr)
        {
            return nullptr;
        }

        usize bytesToCopy = oldSize < newSize ? oldSize : newSize;
        copy(static_cast<byte const*>(oldPointer), static_cast<byte*>(newPointer), static_cast<uint>(bytesToCopy));
        allocator.deallocate(oldPointer);
        return newPointer;
    }

    template <typename T>
    inline T* reallocate(Allocator& allocator, T* oldPointer, usize oldCount, usize newCount)
    requires(Concepts::TriviallyCopyable<T>)
    {
        void* np = reallocate(allocator, static_cast<void*>(oldPointer), oldCount * sizeof(T), newCount * sizeof(T));
        return static_cast<T*>(np);
    }

    /// Reallocates an aligned memory block by allocating a new block, copying, and freeing the old block.
    /// @param allocator The allocator to use.
    /// @param oldPointer The previous pointer (may be nullptr).
    /// @param oldSize The previous size in bytes.
    /// @param newSize The new size in bytes.
    /// @param alignment The alignment in bytes.
    inline void* reallocateAligned(Allocator& allocator, void* oldPointer, usize oldSize, usize newSize, usize alignment)
    {
        if (oldPointer == nullptr)
        {
            return tryAllocateAligned(allocator, newSize, alignment);
        }

        if (newSize == 0)
        {
            allocator.deallocateAligned(oldPointer);
            return nullptr;
        }

        void* newPointer = tryAllocateAligned(allocator, newSize, alignment);
        if (newPointer == nullptr)
        {
            return nullptr;
        }

        usize bytesToCopy = oldSize < newSize ? oldSize : newSize;
        copy(static_cast<byte const*>(oldPointer), static_cast<byte*>(newPointer), static_cast<uint>(bytesToCopy));
        allocator.deallocateAligned(oldPointer);
        return newPointer;
    }

    template <typename T>
    inline T* reallocateAligned(Allocator& allocator, T* oldPointer, usize oldCount, usize newCount, usize alignment = TypeAlignment<T>)
    requires(Concepts::TriviallyCopyable<T>)
    {
        void* np = reallocateAligned(allocator, static_cast<void*>(oldPointer), oldCount * sizeof(T), newCount * sizeof(T), alignment);
        return static_cast<T*>(np);
    }
}
