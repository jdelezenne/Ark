#pragma once

#include "Ark/Core/Concepts.hpp"
#include "Ark/Core/Debug/Assert.hpp"
#include "Ark/Core/Functions.hpp"
#include "Ark/Core/Monostate.hpp"
#include "Ark/Core/Traits.hpp"
#include "Ark/Core/Types.hpp"

#include <new>

namespace Ark
{
    namespace Internal
    {
        template <typename... Types>
        struct VariantStorageTraits;

        template <typename T, typename... Types>
        struct VariantStorageTraits<T, Types...>
        {
            static constexpr usize Size = sizeof(T) > VariantStorageTraits<Types...>::Size ? sizeof(T) : VariantStorageTraits<Types...>::Size;
            static constexpr usize Align = alignof(T) > VariantStorageTraits<Types...>::Align ? alignof(T) : VariantStorageTraits<Types...>::Align;
        };

        template <typename T>
        struct VariantStorageTraits<T>
        {
            static constexpr usize Size = sizeof(T);
            static constexpr usize Align = alignof(T);
        };

        template <usize I, typename... Types>
        struct VariantTypeAt;

        template <typename T, typename... Types>
        struct VariantTypeAt<0, T, Types...>
        {
            using Type = T;
        };

        template <usize I, typename T, typename... Types>
        struct VariantTypeAt<I, T, Types...>
        {
            using Type = typename VariantTypeAt<I - 1, Types...>::Type;
        };

        template <typename T, typename... Types>
        struct VariantIndexOf;

        template <typename T, typename First, typename... Rest>
        struct VariantIndexOf<T, First, Rest...>
        {
            static constexpr usize Value = Traits::isSame<T, First> ? 0 : (1 + VariantIndexOf<T, Rest...>::Value);
        };

        template <typename T>
        struct VariantIndexOf<T>
        {
            static constexpr usize Value = 0;
        };
    }

    /// Tagged union type that stores one alternative type at a time.
    /// @tparam Types Alternative value types.
    template <typename... Types>
    class Variant final
    {
        static_assert(sizeof...(Types) > 0, "Variant must have at least one alternative type");

        template <typename T, typename... Ts>
        friend constexpr T* getIf(Variant<Ts...>* variant);

        template <typename T, typename... Ts>
        friend constexpr T const* getIf(Variant<Ts...> const* variant);

        template <typename Visitor, typename... Ts>
        friend constexpr decltype(auto) visit(Visitor&& visitor, Variant<Ts...>& variant);

        template <typename Visitor, typename... Ts>
        friend constexpr decltype(auto) visit(Visitor&& visitor, Variant<Ts...> const& variant);

    public:
        static constexpr usize AlternativeCount = sizeof...(Types);
        static constexpr usize NPos = static_cast<usize>(-1);

        template <usize I>
        using AlternativeType = typename Internal::VariantTypeAt<I, Types...>::Type;

    private:
        static constexpr usize StorageSize = Internal::VariantStorageTraits<Types...>::Size;
        static constexpr usize StorageAlign = Internal::VariantStorageTraits<Types...>::Align;

        alignas(StorageAlign) byte storage[StorageSize];
        usize activeIndex;

        template <usize I>
        constexpr AlternativeType<I>* getPointer()
        {
            return reinterpret_cast<AlternativeType<I>*>(storage);
        }

        template <usize I>
        constexpr AlternativeType<I> const* getPointer() const
        {
            return reinterpret_cast<AlternativeType<I> const*>(storage);
        }

        template <usize I, typename... Args>
        constexpr void constructAlternative(Args&&... args)
        {
            ::new (static_cast<void*>(storage)) AlternativeType<I>(Ark::forward<Args>(args)...);
            activeIndex = I;
        }

        constexpr void destroy()
        {
            if (activeIndex == NPos)
            {
                return;
            }

            dispatch(activeIndex, [this]<usize I>()
                     {
                         getPointer<I>()->~AlternativeType<I>();
                     });
            activeIndex = NPos;
        }

        constexpr void copyFrom(Variant const& other)
        {
            if (other.activeIndex == NPos)
            {
                return;
            }

            dispatch(other.activeIndex, [this, &other]<usize I>()
                     {
                         constructAlternative<I>(*other.template getPointer<I>());
                     });
        }

        constexpr void moveFrom(Variant&& other)
        {
            if (other.activeIndex == NPos)
            {
                return;
            }

            dispatch(other.activeIndex, [this, &other]<usize I>()
                     {
                         constructAlternative<I>(Ark::move(*other.template getPointer<I>()));
                     });
        }

        template <typename Fn, usize I = 0>
        static constexpr decltype(auto) dispatch(usize index, Fn&& fn)
        {
            if constexpr (I < AlternativeCount)
            {
                if (index == I)
                {
                    return Ark::forward<Fn>(fn).template operator()<I>();
                }

                return dispatch<Fn, I + 1>(index, Ark::forward<Fn>(fn));
            }
            else
            {
                ARK_FATAL_ERROR_MSG("Variant index out of bounds");

                using ReturnType = decltype(Ark::forward<Fn>(fn).template operator()<0>());
                if constexpr (Traits::isVoid<ReturnType>)
                {
                    return;
                }
                else if constexpr (Traits::isReference<ReturnType>)
                {
                    return *static_cast<Traits::RemoveReferenceType<ReturnType>*>(nullptr);
                }
                else
                {
                    return ReturnType{};
                }
            }
        }

        template <typename U>
        static constexpr usize indexOf = Internal::VariantIndexOf<Traits::RemoveCVReferenceType<U>, Types...>::Value;

    public:
        constexpr Variant()
        requires Concepts::ConstructibleFrom<AlternativeType<0>>
            : activeIndex(NPos)
        {
            constructAlternative<0>();
        }

        constexpr ~Variant()
        {
            destroy();
        }

        constexpr Variant(Variant const& other)
            : activeIndex(NPos)
        {
            copyFrom(other);
        }

        constexpr Variant(Variant&& other)
            : activeIndex(NPos)
        {
            moveFrom(Ark::move(other));
        }

        template <typename U = AlternativeType<0>>
        requires(Traits::isAnyOf<Traits::RemoveCVReferenceType<U>, Types...> &&
                 !Traits::isSame<Traits::RemoveCVReferenceType<U>, Variant>)
        constexpr Variant(U&& value)
            : activeIndex(NPos)
        {
            constructAlternative<indexOf<U>>(Ark::forward<U>(value));
        }

        constexpr Variant& operator=(Variant const& other)
        {
            if (this == &other)
            {
                return *this;
            }

            if (activeIndex == other.activeIndex)
            {
                if (activeIndex != NPos)
                {
                    dispatch(activeIndex, [this, &other]<usize I>()
                             {
                                 *getPointer<I>() = *other.template getPointer<I>();
                             });
                }
            }
            else
            {
                destroy();
                copyFrom(other);
            }

            return *this;
        }

        constexpr Variant& operator=(Variant&& other)
        {
            if (this == &other)
            {
                return *this;
            }

            if (activeIndex == other.activeIndex)
            {
                if (activeIndex != NPos)
                {
                    dispatch(activeIndex, [this, &other]<usize I>()
                             {
                                 *getPointer<I>() = Ark::move(*other.template getPointer<I>());
                             });
                }
            }
            else
            {
                destroy();
                moveFrom(Ark::move(other));
            }

            return *this;
        }

        template <typename U>
        requires(Traits::isAnyOf<Traits::RemoveCVReferenceType<U>, Types...> &&
                 !Traits::isSame<Traits::RemoveCVReferenceType<U>, Variant>)
        constexpr Variant& operator=(U&& value)
        {
            constexpr usize i = indexOf<U>;
            if (activeIndex == i)
            {
                *getPointer<i>() = Ark::forward<U>(value);
            }
            else
            {
                destroy();
                constructAlternative<i>(Ark::forward<U>(value));
            }
            return *this;
        }

        constexpr usize index() const
        {
            return activeIndex;
        }

        constexpr bool valueless() const
        {
            return activeIndex == NPos;
        }

        template <typename T>
        constexpr bool is() const
        {
            static_assert(Traits::isAnyOf<T, Types...>, "Requested type is not an alternative of Variant");
            return activeIndex == Internal::VariantIndexOf<T, Types...>::Value;
        }

        template <typename T>
        constexpr T& get() &
        {
            static_assert(Traits::isAnyOf<T, Types...>, "Requested type is not an alternative of Variant");
            ARK_ASSERT_MSG(is<T>(), "Variant does not hold the requested alternative");
            return *reinterpret_cast<T*>(storage);
        }

        template <typename T>
        constexpr T const& get() const&
        {
            static_assert(Traits::isAnyOf<T, Types...>, "Requested type is not an alternative of Variant");
            ARK_ASSERT_MSG(is<T>(), "Variant does not hold the requested alternative");
            return *reinterpret_cast<T const*>(storage);
        }

        template <typename T>
        constexpr T&& get() &&
        {
            static_assert(Traits::isAnyOf<T, Types...>, "Requested type is not an alternative of Variant");
            ARK_ASSERT_MSG(is<T>(), "Variant does not hold the requested alternative");
            return Ark::move(*reinterpret_cast<T*>(storage));
        }

        template <usize I, typename... Args>
        requires Concepts::ConstructibleFrom<AlternativeType<I>, Args...>
        constexpr AlternativeType<I>& emplace(Args&&... args)
        {
            destroy();
            constructAlternative<I>(Ark::forward<Args>(args)...);
            return *getPointer<I>();
        }

        template <typename T, typename... Args>
        requires(Traits::isAnyOf<T, Types...> && Concepts::ConstructibleFrom<T, Args...>)
        constexpr T& emplace(Args&&... args)
        {
            constexpr usize i = Internal::VariantIndexOf<T, Types...>::Value;
            return emplace<i>(Ark::forward<Args>(args)...);
        }

        constexpr bool operator==(Variant const& other) const
        {
            if (activeIndex != other.activeIndex)
            {
                return false;
            }

            if (activeIndex == NPos)
            {
                return true;
            }

            return dispatch(activeIndex, [this, &other]<usize I>()
                            {
                                return *getPointer<I>() == *other.template getPointer<I>();
                            });
        }

        constexpr bool operator!=(Variant const& other) const
        {
            return !(*this == other);
        }

        constexpr void swap(Variant& other)
        {
            if (this == &other)
            {
                return;
            }

            Variant tmp(Ark::move(other));
            other = Ark::move(*this);
            *this = Ark::move(tmp);
        }
    };

    template <typename... Types>
    constexpr void swap(Variant<Types...>& lhs, Variant<Types...>& rhs)
    {
        lhs.swap(rhs);
    }

    template <typename T, typename... Types>
    constexpr T* getIf(Variant<Types...>* variant)
    {
        static_assert(Traits::isAnyOf<T, Types...>, "Requested type is not an alternative of Variant");

        if (variant == nullptr)
        {
            return nullptr;
        }

        constexpr usize i = Internal::VariantIndexOf<T, Types...>::Value;
        if (variant->index() != i)
        {
            return nullptr;
        }

        return reinterpret_cast<T*>(variant->storage);
    }

    template <typename T, typename... Types>
    constexpr T const* getIf(Variant<Types...> const* variant)
    {
        static_assert(Traits::isAnyOf<T, Types...>, "Requested type is not an alternative of Variant");

        if (variant == nullptr)
        {
            return nullptr;
        }

        constexpr usize i = Internal::VariantIndexOf<T, Types...>::Value;
        if (variant->index() != i)
        {
            return nullptr;
        }

        return reinterpret_cast<T const*>(variant->storage);
    }

    template <typename T, typename... Types>
    constexpr T& get(Variant<Types...>& variant)
    {
        return variant.template get<T>();
    }

    template <typename T, typename... Types>
    constexpr T const& get(Variant<Types...> const& variant)
    {
        return variant.template get<T>();
    }

    template <typename T, typename... Types>
    constexpr T&& get(Variant<Types...>&& variant)
    {
        return Ark::move(variant).template get<T>();
    }

    template <typename Visitor, typename... Types>
    constexpr decltype(auto) visit(Visitor&& visitor, Variant<Types...>& variant)
    {
        ARK_ASSERT_MSG(!variant.valueless(), "Cannot visit a valueless Variant");
        return Variant<Types...>::dispatch(variant.index(), [&]<usize I>() -> decltype(auto)
                                           {
                                               using Alt = typename Variant<Types...>::template AlternativeType<I>;
                                               return Ark::forward<Visitor>(visitor)(*reinterpret_cast<Alt*>(variant.storage));
                                           });
    }

    template <typename Visitor, typename... Types>
    constexpr decltype(auto) visit(Visitor&& visitor, Variant<Types...> const& variant)
    {
        ARK_ASSERT_MSG(!variant.valueless(), "Cannot visit a valueless Variant");
        return Variant<Types...>::dispatch(variant.index(), [&]<usize I>() -> decltype(auto)
                                           {
                                               using Alt = typename Variant<Types...>::template AlternativeType<I>;
                                               return Ark::forward<Visitor>(visitor)(*reinterpret_cast<Alt const*>(variant.storage));
                                           });
    }
}
