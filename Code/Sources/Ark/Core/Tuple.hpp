#pragma once

#include "Ark/Core/Functions.hpp"
#include "Ark/Core/Traits.hpp"
#include "Ark/Core/Types.hpp"

namespace Ark
{
    template <typename... Types>
    struct Tuple;

    template <usize Index, typename Head, typename... Tail>
    constexpr auto& get(Tuple<Head, Tail...>& tuple);

    template <usize Index, typename Head, typename... Tail>
    constexpr auto const& get(Tuple<Head, Tail...> const& tuple);

    template <usize Index, typename Head, typename... Tail>
    constexpr auto&& get(Tuple<Head, Tail...>&& tuple);

    template <usize Index, typename Head, typename... Tail>
    constexpr auto const&& get(Tuple<Head, Tail...> const&& tuple);

    namespace Internal
    {
        template <typename FunctionType, typename TupleType, usize... Indices>
        constexpr decltype(auto) applyInternal(FunctionType&& function, TupleType&& tuple, Traits::IndexSequence<Indices...>)
        {
            return Ark::forward<FunctionType>(function)(Ark::get<Indices>(Ark::forward<TupleType>(tuple))...);
        }
    }

    template <>
    struct Tuple<> final
    {
        constexpr Tuple() = default;

        constexpr void swap(Tuple&) noexcept
        {
        }
    };

    template <typename Head, typename... Tail>
    struct Tuple<Head, Tail...> final
    {
        Head head;
        Tuple<Tail...> tail;

        constexpr Tuple() = default;
        constexpr Tuple(Tuple const&) = default;
        constexpr Tuple(Tuple&&) = default;
        constexpr Tuple& operator=(Tuple const&) = default;
        constexpr Tuple& operator=(Tuple&&) = default;

        constexpr Tuple(Head const& headValue, Tail const&... tailValues)
            : head(headValue)
            , tail(tailValues...)
        {
        }

        template <typename UHead, typename... UTail>
        constexpr Tuple(UHead&& headValue, UTail&&... tailValues)
            : head(Ark::forward<UHead>(headValue))
            , tail(Ark::forward<UTail>(tailValues)...)
        {
        }

        constexpr void swap(Tuple& other)
        {
            using ::Ark::swap;
            swap(head, other.head);
            tail.swap(other.tail);
        }
    };

    template <typename TupleType>
    struct TupleSize;

    template <typename... Types>
    struct TupleSize<Tuple<Types...>> : Traits::Internal::IntegralConstant<usize, sizeof...(Types)>
    {
    };

    template <typename TupleType>
    struct TupleSize<TupleType const> : TupleSize<TupleType>
    {
    };

    template <typename TupleType>
    struct TupleSize<TupleType volatile> : TupleSize<TupleType>
    {
    };

    template <typename TupleType>
    struct TupleSize<TupleType const volatile> : TupleSize<TupleType>
    {
    };

    template <typename TupleType>
    constexpr usize tupleSize = TupleSize<TupleType>::value;

    template <usize Index, typename TupleType>
    struct TupleElement;

    template <typename Head, typename... Tail>
    struct TupleElement<0, Tuple<Head, Tail...>>
    {
        using Type = Head;
    };

    template <usize Index, typename Head, typename... Tail>
    struct TupleElement<Index, Tuple<Head, Tail...>>
    {
        using Type = typename TupleElement<Index - 1, Tuple<Tail...>>::Type;
    };

    template <usize Index, typename TupleType>
    using TupleElementType = typename TupleElement<Index, TupleType>::Type;

    template <usize Index, typename Head, typename... Tail>
    constexpr auto& get(Tuple<Head, Tail...>& tuple)
    {
        if constexpr (Index == 0)
        {
            return tuple.head;
        }
        else
        {
            return get<Index - 1>(tuple.tail);
        }
    }

    template <usize Index, typename Head, typename... Tail>
    constexpr auto const& get(Tuple<Head, Tail...> const& tuple)
    {
        if constexpr (Index == 0)
        {
            return tuple.head;
        }
        else
        {
            return get<Index - 1>(tuple.tail);
        }
    }

    template <usize Index, typename Head, typename... Tail>
    constexpr auto&& get(Tuple<Head, Tail...>&& tuple)
    {
        if constexpr (Index == 0)
        {
            return Ark::move(tuple.head);
        }
        else
        {
            return get<Index - 1>(Ark::move(tuple.tail));
        }
    }

    template <usize Index, typename Head, typename... Tail>
    constexpr auto const&& get(Tuple<Head, Tail...> const&& tuple)
    {
        if constexpr (Index == 0)
        {
            return Ark::move(tuple.head);
        }
        else
        {
            return get<Index - 1>(Ark::move(tuple.tail));
        }
    }

    template <typename... Types>
    constexpr Tuple<Traits::RemoveCVReferenceType<Types>...> makeTuple(Types&&... values)
    {
        return Tuple<Traits::RemoveCVReferenceType<Types>...>(Ark::forward<Types>(values)...);
    }

    template <typename... Types>
    constexpr Tuple<Types&...> tie(Types&... values)
    {
        return Tuple<Types&...>(values...);
    }

    template <typename... Types>
    constexpr Tuple<Types&&...> forwardAsTuple(Types&&... values)
    {
        return Tuple<Types&&...>(Ark::forward<Types>(values)...);
    }

    template <typename FunctionType, typename TupleType>
    constexpr decltype(auto) apply(FunctionType&& function, TupleType&& tuple)
    {
        return Internal::applyInternal(
            Ark::forward<FunctionType>(function),
            Ark::forward<TupleType>(tuple),
            Traits::MakeIndexSequence<tupleSize<Traits::RemoveReferenceType<TupleType>>>{});
    }
}