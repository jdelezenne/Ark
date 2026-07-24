#pragma once

#include "Ark/Core/Concepts.hpp"
#include "Ark/Core/Types.hpp"

namespace Ark
{
    /// A type that represents a Byte (8 bits).
    /// @details Byte is used to represent raw binary data and is not intended for arithmetic operations.
    enum class Byte : uint8
    {
    };

    /// Explicit conversion from integral types to Byte
    template <Concepts::Integral Type>
    constexpr Byte toByte(Type value)
    {
        return static_cast<Byte>(value);
    }

    /// Explicit conversion from Byte to integral types
    template <Concepts::Integral Type>
    constexpr Type toInteger(Byte b)
    {
        return static_cast<Type>(b);
    }

    /// Bitwise OR operation
    constexpr Byte operator|(Byte left, Byte right)
    {
        return static_cast<Byte>(
            static_cast<unsigned char>(left) | static_cast<unsigned char>(right));
    }

    /// Bitwise AND operation
    constexpr Byte operator&(Byte left, Byte right)
    {
        return static_cast<Byte>(
            static_cast<unsigned char>(left) & static_cast<unsigned char>(right));
    }

    /// Bitwise XOR operation
    constexpr Byte operator^(Byte left, Byte right)
    {
        return static_cast<Byte>(
            static_cast<unsigned char>(left) ^ static_cast<unsigned char>(right));
    }

    /// Bitwise NOT operation
    constexpr Byte operator~(Byte b)
    {
        return static_cast<Byte>(~static_cast<unsigned char>(b));
    }

    /// Bitwise OR assignment operator
    constexpr Byte& operator|=(Byte& left, Byte right)
    {
        left = left | right;
        return left;
    }

    /// Bitwise AND assignment operator
    constexpr Byte& operator&=(Byte& left, Byte right)
    {
        left = left & right;
        return left;
    }

    /// Bitwise XOR assignment operator
    constexpr Byte& operator^=(Byte& left, Byte right)
    {
        left = left ^ right;
        return left;
    }

    /// Shift operator that shifts the bits of the Byte to the left by the specified number of positions.
    template <Concepts::Integral Type>
    constexpr Byte operator<<(Byte b, Type shift)
    {
        return static_cast<Byte>(static_cast<unsigned char>(b) << shift);
    }

    /// Shift operator that shifts the bits of the Byte to the right by the specified number of positions.
    template <Concepts::Integral Type>
    constexpr Byte operator>>(Byte b, Type shift)
    {
        return static_cast<Byte>(static_cast<unsigned char>(b) >> shift);
    }

    // Compound assignment operators that shifts the bits of the Byte to the left by the specified number of positions.
    template <Concepts::Integral Type>
    constexpr Byte& operator<<=(Byte& b, Type shift)
    {
        b = b << shift;
        return b;
    }

    // Compound assignment operators that shifts the bits of the Byte to the right by the specified number of positions.
    template <Concepts::Integral Type>
    constexpr Byte& operator>>=(Byte& b, Type shift)
    {
        b = b >> shift;
        return b;
    }
}
