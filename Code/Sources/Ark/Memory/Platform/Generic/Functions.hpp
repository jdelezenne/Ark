#pragma once

#include "Ark/Core/Comparison.hpp"
#include "Ark/Core/Types.hpp"

namespace Ark::Memory
{
    inline void* copy(void const* source, void* destination, uint size)
    {
        byte const* srcPtr = static_cast<byte const*>(source);
        byte* destPtr = static_cast<byte*>(destination);

        for (uint i = 0; i < size; ++i)
        {
            destPtr[i] = srcPtr[i];
        }

        return destination;
    }

    inline void* move(void const* source, void* destination, uint size)
    {
        byte const* srcPtr = static_cast<byte const*>(source);
        byte* destPtr = static_cast<byte*>(destination);

        if (destPtr < srcPtr)
        {
            for (uint i = 0; i < size; ++i)
            {
                destPtr[i] = srcPtr[i];
            }
        }
        else if (destPtr > srcPtr)
        {
            for (int i = static_cast<int>(size) - 1; i >= 0; --i)
            {
                destPtr[i] = srcPtr[i];
            }
        }

        return destination;
    }

    inline void* set(void* destination, uint size, int value)
    {
        byte* destPtr = static_cast<byte*>(destination);

        for (uint i = 0; i < size; ++i)
        {
            destPtr[i] = static_cast<byte>(value);
        }

        return destination;
    }

    inline void zero(void* destination, uint size)
    {
        set(destination, size, 0);
    }

    inline Ordering compare(void const* lhs, void const* rhs, uint size)
    {
        byte const* lhsPtr = static_cast<byte const*>(lhs);
        byte const* rhsPtr = static_cast<byte const*>(rhs);

        for (uint i = 0; i < size; ++i)
        {
            if (lhsPtr[i] < rhsPtr[i])
            {
                return Ordering::Less;
            }

            if (lhsPtr[i] > rhsPtr[i])
            {
                return Ordering::Greater;
            }
        }

        return Ordering::Equal;
    }

    inline bool find(void const* source, byte value, uint size)
    {
        byte const* sourcePtr = static_cast<byte const*>(source);

        for (uint i = 0; i < size; ++i)
        {
            if (sourcePtr[i] == value)
            {
                return true;
            }
        }

        return false;
    }
}
