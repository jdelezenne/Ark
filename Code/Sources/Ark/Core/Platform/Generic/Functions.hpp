#pragma once

#include "Ark/Core/Concepts.hpp"

namespace Ark
{
    template <typename Type>
    [[nodiscard]]
    constexpr Type&& forward(Traits::RemoveReferenceType<Type>& arg)
    {
        return static_cast<Type&&>(arg);
    }

    template <typename Type>
    requires(!Traits::isLvalueReference<Type>)
    [[nodiscard]]
    constexpr Type&& forward(Traits::RemoveReferenceType<Type>&& arg)
    {
        return static_cast<Type&&>(arg);
    }

    template <typename Type>
    [[nodiscard]]
    constexpr Traits::RemoveReferenceType<Type>&& move(Type&& arg)
    {
        return static_cast<Traits::RemoveReferenceType<Type>&&>(arg);
    }

    template <typename T, typename U = T>
    requires Concepts::AssignableFrom<T&, U>
    constexpr T exchange(T& obj, U&& newValue)
    {
        T oldValue = Ark::move(obj);
        obj = Ark::forward<U>(newValue);
        return oldValue;
    }

    template <typename T>
    requires requires(T& lhs, T& rhs) {
        lhs.swap(rhs);
    }
    constexpr void swap(T& lhs, T& rhs)
    {
        lhs.swap(rhs);
    }

    template <typename T>
    requires(!requires(T& lhs, T& rhs) {
        lhs.swap(rhs);
    } && Concepts::MoveConstructible<T> && Concepts::MoveAssignable<T>)
    constexpr void swap(T& lhs, T& rhs)
    {
        T temp = Ark::move(lhs);
        lhs = Ark::move(rhs);
        rhs = Ark::move(temp);
    }

    template <typename T, usize N>
    requires Concepts::Swappable<T>
    constexpr void swap(T (&lhs)[N], T (&rhs)[N])
    {
        for (usize i = 0; i < N; ++i)
        {
            swap(lhs[i], rhs[i]);
        }
    }

    template <typename T>
    [[nodiscard]]
    constexpr T const& max(T const& lhs, T const& rhs)
    {
        return lhs < rhs ? rhs : lhs;
    }

    template <typename T, typename Predicate>
    [[nodiscard]]
    constexpr T const& max(T const& lhs, T const& rhs, Predicate predicate)
    {
        return predicate(lhs, rhs) ? rhs : lhs;
    }

    template <typename T>
    [[nodiscard]]
    constexpr T const&(min)(T const& lhs, T const& rhs)
    {
        return rhs < lhs ? rhs : lhs;
    }

    template <typename T, typename Predicate>
    [[nodiscard]]
    constexpr T const& min(T const& lhs, T const& rhs, Predicate predicate)
    {
        return predicate(rhs, lhs) ? rhs : lhs;
    }
}
