#pragma once

#include "Ark/Core/Comparison.hpp"
#include "Ark/Core/Types.hpp"

#include <cstring>

namespace Ark::Memory
{
    inline void* copy(void const* source, void* destination, uint size)
    {
        return std::memcpy(destination, source, size);
    }

    inline void* set(void* destination, uint size, int value)
    {
        return std::memset(destination, value, size);
    }

    inline void zero(void* destination, uint size)
    {
        set(destination, size, 0);
    }

    inline Ordering compare(void const* lhs, void const* rhs, uint size)
    {
        return orderingFromInt(std::memcmp(lhs, rhs, size));
    }

    inline void* move(void const* source, void* destination, uint size)
    {
        return std::memmove(destination, source, size);
    }

    inline bool find(void const* source, byte value, uint size)
    {
        return std::memchr(source, static_cast<int>(static_cast<unsigned char>(value)), size) != nullptr;
    }
}
