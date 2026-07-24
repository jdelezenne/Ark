#pragma once

#include "Ark/Core/Traits.hpp"

namespace Ark::Internal
{
    /// Converts an enum value to its underlying integral type.
    /// @tparam E Enum type.
    /// @param e Enum value.
    /// @return Underlying integral representation.
    template <typename E>
    constexpr Traits::UnderlyingType<E> toUnderlying(E e)
    {
        return static_cast<Traits::UnderlyingType<E>>(e);
    }
}

/// Declares bitwise operators and a `hasFlag` helper for an enum type.
/// @param EnumType Enum type for which operators are generated.
#define ARK_FLAG_ENUM(EnumType)                                                                                    \
    constexpr EnumType operator|(EnumType lhs, EnumType rhs)                                                       \
    {                                                                                                              \
        return static_cast<EnumType>((Ark::Internal::toUnderlying((lhs))) | (Ark::Internal::toUnderlying((rhs)))); \
    }                                                                                                              \
                                                                                                                   \
    constexpr EnumType operator&(EnumType lhs, EnumType rhs)                                                       \
    {                                                                                                              \
        return static_cast<EnumType>((Ark::Internal::toUnderlying((lhs))) & (Ark::Internal::toUnderlying((rhs)))); \
    }                                                                                                              \
                                                                                                                   \
    constexpr EnumType operator^(EnumType lhs, EnumType rhs)                                                       \
    {                                                                                                              \
        return static_cast<EnumType>((Ark::Internal::toUnderlying((lhs))) ^ (Ark::Internal::toUnderlying((rhs)))); \
    }                                                                                                              \
                                                                                                                   \
    constexpr EnumType operator~(EnumType value)                                                                   \
    {                                                                                                              \
        return static_cast<EnumType>(~(Ark::Internal::toUnderlying((value))));                                     \
    }                                                                                                              \
                                                                                                                   \
    constexpr EnumType& operator|=(EnumType& lhs, EnumType rhs)                                                    \
    {                                                                                                              \
        (lhs) = (lhs) | (rhs);                                                                                     \
        return lhs;                                                                                                \
    }                                                                                                              \
                                                                                                                   \
    constexpr EnumType& operator&=(EnumType& lhs, EnumType rhs)                                                    \
    {                                                                                                              \
        (lhs) = (lhs) & (rhs);                                                                                     \
        return lhs;                                                                                                \
    }                                                                                                              \
                                                                                                                   \
    constexpr EnumType& operator^=(EnumType& lhs, EnumType rhs)                                                    \
    {                                                                                                              \
        (lhs) = (lhs) ^ (rhs);                                                                                     \
        return lhs;                                                                                                \
    }                                                                                                              \
                                                                                                                   \
    constexpr bool hasFlag(EnumType value, EnumType flag)                                                          \
    {                                                                                                              \
        return ((Ark::Internal::toUnderlying((value))) & (Ark::Internal::toUnderlying((flag)))) ==                 \
               (Ark::Internal::toUnderlying((flag)));                                                              \
    }

/// Alias macro for declaring bitwise operators on enum types.
/// @param EnumType Enum type for which operators are generated.
#define ARK_ENUM_FLAG(EnumType)                                                                                    \
    constexpr EnumType operator|(EnumType lhs, EnumType rhs)                                                       \
    {                                                                                                              \
        return static_cast<EnumType>((Ark::Internal::toUnderlying((lhs))) | (Ark::Internal::toUnderlying((rhs)))); \
    }                                                                                                              \
                                                                                                                   \
    constexpr EnumType operator&(EnumType lhs, EnumType rhs)                                                       \
    {                                                                                                              \
        return static_cast<EnumType>((Ark::Internal::toUnderlying((lhs))) & (Ark::Internal::toUnderlying((rhs)))); \
    }                                                                                                              \
                                                                                                                   \
    constexpr EnumType operator^(EnumType lhs, EnumType rhs)                                                       \
    {                                                                                                              \
        return static_cast<EnumType>((Ark::Internal::toUnderlying((lhs))) ^ (Ark::Internal::toUnderlying((rhs)))); \
    }                                                                                                              \
                                                                                                                   \
    constexpr EnumType operator~(EnumType value)                                                                   \
    {                                                                                                              \
        return static_cast<EnumType>(~(Ark::Internal::toUnderlying((value))));                                     \
    }                                                                                                              \
                                                                                                                   \
    constexpr EnumType& operator|=(EnumType& lhs, EnumType rhs)                                                    \
    {                                                                                                              \
        (lhs) = (lhs) | (rhs);                                                                                     \
        return lhs;                                                                                                \
    }                                                                                                              \
                                                                                                                   \
    constexpr EnumType& operator&=(EnumType& lhs, EnumType rhs)                                                    \
    {                                                                                                              \
        (lhs) = (lhs) & (rhs);                                                                                     \
        return lhs;                                                                                                \
    }                                                                                                              \
                                                                                                                   \
    constexpr EnumType& operator^=(EnumType& lhs, EnumType rhs)                                                    \
    {                                                                                                              \
        (lhs) = (lhs) ^ (rhs);                                                                                     \
        return lhs;                                                                                                \
    }                                                                                                              \
                                                                                                                   \
    constexpr bool hasFlag(EnumType value, EnumType flag)                                                          \
    {                                                                                                              \
        return ((Ark::Internal::toUnderlying((value))) & (Ark::Internal::toUnderlying((flag)))) ==                 \
               (Ark::Internal::toUnderlying((flag)));                                                              \
    }
