#pragma once

#include "Ark/Core/Concepts.hpp"
#include "Ark/Core/Configuration.hpp"
#include "Ark/Core/Functions.hpp"
#include "Ark/Core/Traits.hpp"
#include "Ark/Core/Types.hpp"

namespace Ark
{
    /// Returns the address of the given value.
    template <typename Type>
    [[nodiscard]]
    constexpr Type* addressOf(Type& value)
    {
        return __builtin_addressof(value);
    }

    /// Returns the address of the given value.
    template <typename Type>
    [[nodiscard]]
    Type const* addressOf(Type const&&) = delete;
}

namespace Ark
{
    /// Returns a bitmask with the x-th bit set to 1.
    /// @param x The bit index to set (0-based).
    /// @return A bitmask with the x-th bit set to 1.
    template <Concepts::Integral Type>
    constexpr Type bit(Type x)
    {
        return (static_cast<Type>(1) << x);
    }

    template <Concepts::Enum Type>
    constexpr typename Traits::UnderlyingType<Type> bit(Type x)
    {
        return (static_cast<typename Traits::UnderlyingType<Type>>(1) << static_cast<typename Traits::UnderlyingType<Type>>(x));
    }
}

namespace Ark
{
    template <typename Collection>
    constexpr auto countOf(Collection const& collection) noexcept(noexcept(collection.getCount()))
        -> decltype(collection.getCount())
    {
        return collection.getCount();
    }

    template <typename Type, usize Size>
    constexpr usize countOf(Type const (&)[Size]) noexcept
    {
        return Size;
    }

    template <typename Collection>
    constexpr auto sizeOf(Collection const& collection) noexcept(noexcept(collection.getCount()))
        -> decltype(collection.getCount() * sizeof(typename Collection::ValueType))
    {
        return collection.getCount() * sizeof(typename Collection::ValueType);
    }

    template <typename Type, usize Size>
    constexpr usize sizeOf(Type const (&)[Size])
    {
        return Size * sizeof(Type);
    }

    /// Rounds a value up to the next multiple of alignment.
    /// @param value The value to round up.
    /// @param alignment The target multiple. When zero, value is returned unchanged.
    /// @return The rounded value.
    constexpr usize roundUpToMultiple(usize value, usize alignment)
    {
        if (alignment == 0)
        {
            return value;
        }

        usize remainder = value % alignment;
        return remainder == 0 ? value : (value + (alignment - remainder));
    }

    namespace Traits
    {
#pragma region Comparison

        template <typename Type = void>
        struct Less final
        {
            constexpr bool operator()(Type const& Left, Type const& Right) const
            {
                return Left < Right;
            }
        };

        template <>
        struct Less<void> final
        {
            template <typename Type1, typename Type2>
            constexpr auto operator()(Type1&& Left, Type2&& Right) const
                -> decltype(static_cast<Type1&&>(Left) < static_cast<Type2&&>(Right))
            {
                return static_cast<Type1&&>(Left) < static_cast<Type2&&>(Right);
            }
        };

        template <typename T1, typename T2 = T1>
        using CompareThreeWayResultType =
            decltype(declval<Traits::RemoveReferenceType<T1> const&>() <=> declval<Traits::RemoveReferenceType<T2> const&>());

        template <typename T1, typename T2 = T1>
        struct CompareThreeWayResult final
        {
        };

        template <typename T1, typename T2>
        requires requires { typename CompareThreeWayResultType<T1, T2>; }
        struct CompareThreeWayResult<T1, T2> final
        {
            using Type = CompareThreeWayResultType<T1, T2>;
        };

        struct CompareThreeWay final
        {
            template <typename T1, typename T2>
            constexpr auto operator()(T1&& _Left, T2&& _Right) const
            {
                return forward<T1>(_Left) <=> forward<T2>(_Right);
            }
        };

#pragma endregion
    }
}
