#pragma once

#include "Ark/Core/Concepts.hpp"
#include "Ark/Core/Functions.hpp"
#include "Ark/Core/Macros.hpp"
#include "Ark/Core/Traits.hpp"
#include "Ark/Core/Types.hpp"
#include "Ark/Core/Utilities.hpp"

namespace Ark
{
    /// A structure representing a pair of values.
    /// @tparam T1 Type of the first value.
    /// @tparam T2 Type of the second value.
    template <typename T1, typename T2>
    struct Pair final
    {
        ARK_STRUCT_DEFAULT(Pair)

    public:
        T1 first{};  ///< The first value of the pair.
        T2 second{}; ///< The second value of the pair.

    public:
        /// Constructs a Pair with the given values.
        /// @param x The first value.
        /// @param y The second value.
        constexpr Pair(const T1& x, const T2& y)
            : first{x}
            , second{y}
        {
        }

        /// Constructs a Pair with the given values, using perfect forwarding.
        /// @tparam U1 Type of the first value.
        /// @tparam U2 Type of the second value.
        /// @param x The first value.
        /// @param y The second value.
        template <typename U1, typename U2>
        requires Concepts::ConstructibleFrom<T1, U1> && Concepts::ConstructibleFrom<T2, U2>
        constexpr Pair(U1&& x, U2&& y)
            : first{Ark::forward<U1>(x)}
            , second{Ark::forward<U2>(y)}
        {
        }

        /// Assigns the values from another Pair.
        /// @tparam U1 Type of the first value of the other pair.
        /// @tparam U2 Type of the second value of the other pair.
        /// @param other The other pair.
        /// @return Reference to this pair.
        template <typename U1, typename U2>
        constexpr Pair& operator=(const Pair<U1, U2>& other)
        {
            first = other.first;
            second = other.second;
            return *this;
        }

        /// Swaps the values with another Pair.
        /// @param other The other pair.
        constexpr void swap(Pair& other)
        {
            using ::Ark::swap;
            swap(first, other.first);
            swap(second, other.second);
        }
    };

    template <typename T1, typename T2>
    constexpr bool operator==(const Pair<T1, T2>& lhs, const Pair<T1, T2>& rhs)
    {
        return (lhs.first == rhs.first) && (lhs.second == rhs.second);
    }

    template <typename T1, typename T2>
    constexpr bool operator!=(const Pair<T1, T2>& lhs, const Pair<T1, T2>& rhs)
    {
        return !(lhs == rhs);
    }

    template <typename T1, typename T2>
    constexpr bool operator<(const Pair<T1, T2>& lhs, const Pair<T1, T2>& rhs)
    {
        return lhs.first < rhs.first || (!(rhs.first < lhs.first) && lhs.second < rhs.second);
    }

    template <typename T1, typename T2>
    constexpr bool operator>(const Pair<T1, T2>& lhs, const Pair<T1, T2>& rhs)
    {
        return rhs < lhs;
    }

    template <typename T1, typename T2>
    constexpr bool operator<=(const Pair<T1, T2>& lhs, const Pair<T1, T2>& rhs)
    {
        return !(rhs < lhs);
    }

    template <typename T1, typename T2>
    constexpr bool operator>=(const Pair<T1, T2>& lhs, const Pair<T1, T2>& rhs)
    {
        return !(lhs < rhs);
    }

    /// Gets the value at the specified index from the pair.
    /// @tparam I The index (0 for first, 1 for second).
    /// @tparam T1 Type of the first value.
    /// @tparam T2 Type of the second value.
    /// @param p The pair.
    /// @return Reference to the value at the specified index.
    template <usize I, typename T1, typename T2>
    constexpr auto& get(Ark::Pair<T1, T2>& p)
    {
        if constexpr (I == 0)
        {
            return p.first;
        }
        else if constexpr (I == 1)
        {
            return p.second;
        }
    }

    /// Gets the value at the specified index from the pair.
    /// @tparam I The index (0 for first, 1 for second).
    /// @tparam T1 Type of the first value.
    /// @tparam T2 Type of the second value.
    /// @param p The pair.
    /// @return Const reference to the value at the specified index.
    template <usize I, typename T1, typename T2>
    constexpr const auto& get(const Ark::Pair<T1, T2>& p)
    {
        if constexpr (I == 0)
        {
            return p.first;
        }
        else if constexpr (I == 1)
        {
            return p.second;
        }
    }

    /// Gets the value at the specified index from the pair (rvalue version).
    /// @tparam I The index (0 for first, 1 for second).
    /// @tparam T1 Type of the first value.
    /// @tparam T2 Type of the second value.
    /// @param p The pair.
    /// @return Rvalue reference to the value at the specified index.
    template <usize I, typename T1, typename T2>
    constexpr auto&& get(Pair<T1, T2>&& p)
    {
        if constexpr (I == 0)
        {
            return Ark::move(p.first);
        }
        else if constexpr (I == 1)
        {
            return Ark::move(p.second);
        }
    }

    /// Creates a pair of references to the given values.
    /// @tparam T1 Type of the first value.
    /// @tparam T2 Type of the second value.
    /// @param t1 The first value.
    /// @param t2 The second value.
    /// @return A pair of references to the given values.
    template <typename T1, typename T2>
    constexpr Pair<T1&, T2&> tie(T1& t1, T2& t2)
    {
        return Pair<T1&, T2&>(t1, t2);
    }

    /// Creates a pair with the given values, using perfect forwarding.
    /// @tparam T1 Type of the first value.
    /// @tparam T2 Type of the second value.
    /// @param t The first value.
    /// @param u The second value.
    /// @return A pair with the given values.
    template <typename T1, typename T2>
    constexpr Pair<Traits::RemoveCVReferenceType<T1>, Traits::RemoveCVReferenceType<T2>> makePair(T1&& t, T2&& u)
    {
        using First = Traits::RemoveCVReferenceType<T1>;
        using Second = Traits::RemoveCVReferenceType<T2>;
        return Pair<First, Second>(Ark::forward<T1>(t), Ark::forward<T2>(u));
    }
}
