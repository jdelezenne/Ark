#pragma once

#include "Ark/Core/Types.hpp"

namespace Ark::Traits
{
    namespace Internal
    {
        /// Compile-time constant wrapper.
        /// @tparam T Constant value type.
        /// @tparam V Constant value.
        template <typename T, T V>
        struct IntegralConstant
        {
            static constexpr T value = V;

            using ValueType = T;
            using Type = IntegralConstant;

            constexpr operator ValueType() const
            {
                return value;
            }

            constexpr ValueType operator()() const
            {
                return value;
            }
        };

        template <bool V>
        using BoolConstant = IntegralConstant<bool, V>;
    }

    /// Trait type representing boolean true.
    using TrueType = Internal::BoolConstant<true>;
    /// Trait type representing boolean false.
    using FalseType = Internal::BoolConstant<false>;

#pragma region Integer Sequence

    template <typename T, T... Values>
    struct IntegerSequence
    {
        using ValueType = T;

        static constexpr usize size() noexcept
        {
            return sizeof...(Values);
        }
    };

    template <usize... Values>
    using IndexSequence = IntegerSequence<usize, Values...>;

    namespace Internal
    {
        template <typename T, T Count, bool IsZero, T... Values>
        struct MakeIntegerSequenceInternal;

        template <typename T, T Count, T... Values>
        struct MakeIntegerSequenceInternal<T, Count, false, Values...>
            : MakeIntegerSequenceInternal<T, Count - 1, (Count - 1) == 0, Count - 1, Values...>
        {
        };

        template <typename T, T Count, T... Values>
        struct MakeIntegerSequenceInternal<T, Count, true, Values...>
        {
            using Type = IntegerSequence<T, Values...>;
        };
    }

    template <typename T, T Count>
    using MakeIntegerSequence = typename Internal::MakeIntegerSequenceInternal<T, Count, Count == 0>::Type;

    template <usize Count>
    using MakeIndexSequence = MakeIntegerSequence<usize, Count>;

    template <typename... Types>
    using IndexSequenceFor = MakeIndexSequence<sizeof...(Types)>;

#pragma endregion

#pragma region Enable If, Conditional

    namespace Internal
    {
        template <bool Test, typename T = void>
        struct EnableIf
        {
        };

        template <typename T>
        struct EnableIf<true, T>
        {
            using Type = T;
        };
    }

    template <bool Test, typename T = void>
    /// Alias that exposes `T` only when `Test` is true.
    using EnableIfType = typename Internal::EnableIf<Test, T>::Type;

    namespace Internal
    {
        template <bool Test, typename T1, typename T2>
        struct Conditional
        {
            using Type = T1;
        };

        template <typename T1, typename T2>
        struct Conditional<false, T1, T2>
        {
            using Type = T2;
        };
    }

    template <bool Test, typename T1, typename T2>
    /// Chooses `T1` when `Test` is true, otherwise `T2`.
    using ConditionalType = typename Internal::Conditional<Test, T1, T2>::Type;

#pragma endregion

#if defined(ARK_COMPILER_CLANG)

    template <typename T1, typename T2>
    constexpr bool isSame = __is_same(T1, T2);

    namespace Internal
    {
        template <typename T1, typename T2>
        struct IsSame : BoolConstant<__is_same(T1, T2)>
        {
        };
    }

#elif defined(ARK_COMPILER_MSVC)

    template <typename, typename>
    constexpr bool isSame = false;

    template <typename T>
    constexpr bool isSame<T, T> = true;

    namespace Internal
    {
        template <typename T1, typename T2>
        struct IsSame : BoolConstant<isSame<T1, T2>>
        {
        };
    }

#endif

#pragma region Extent

    namespace Internal
    {
        template <typename T>
        struct RemoveExtent
        {
            using Type = T;
        };

        template <typename T, usize I>
        struct RemoveExtent<T[I]>
        {
            using Type = T;
        };

        template <typename T>
        struct RemoveExtent<T[]>
        {
            using Type = T;
        };
    }

    template <typename T>
    /// Removes a single array extent from a type.
    using RemoveExtentType = typename Internal::RemoveExtent<T>::Type;

#pragma endregion

#pragma region Array

    template <typename>
    constexpr bool isArray = false;

    template <typename T, usize N>
    constexpr bool isArray<T[N]> = true;

    template <typename T>
    constexpr bool isArray<T[]> = true;

    namespace Internal
    {
        template <typename T>
        struct IsArray : Internal::BoolConstant<isArray<T>>
        {
        };
    }

    template <typename>
    constexpr bool isBoundedArray = false;

    template <typename T, usize N>
    constexpr bool isBoundedArray<T[N]> = true;

    template <typename T>
    struct IsBoundedArray : Internal::BoolConstant<isBoundedArray<T>>
    {
    };

    template <typename>
    constexpr bool isUnboundedArray = false;

    template <typename T>
    constexpr bool isUnboundedArray<T[]> = true;

    template <typename T>
    struct IsUnboundedArray : Internal::BoolConstant<isUnboundedArray<T>>
    {
    };

#pragma endregion

#pragma region Remove

    namespace Internal
    {
        template <typename T>
        struct RemoveConst
        {
            using Type = T;
        };

        template <typename T>
        struct RemoveConst<T const>
        {
            using Type = T;
        };

        template <typename T>
        struct RemoveVolatile
        {
            using Type = T;
        };

        template <typename T>
        struct RemoveVolatile<T volatile>
        {
            using Type = T;
        };

        template <typename T>
        struct RemoveCV
        {
            using Type = T;

            template <template <typename> typename Func>
            using Apply = Func<T>;
        };

        template <typename T>
        struct RemoveCV<T const>
        {
            using Type = T;

            template <template <typename> typename Func>
            using Apply = Func<T> const;
        };

        template <typename T>
        struct RemoveCV<T volatile>
        {
            using Type = T;

            template <template <typename> typename Func>
            using Apply = Func<T> volatile;
        };

        template <typename T>
        struct RemoveCV<T const volatile>
        {
            using Type = T;

            template <template <typename> typename Func>
            using Apply = Func<T> const volatile;
        };
    }

    template <typename T>
    /// Removes `const` qualifier from a type.
    using RemoveConstType = typename Internal::RemoveConst<T>::Type;

    template <typename T>
    /// Removes `volatile` qualifier from a type.
    using RemoveVolatileType = typename Internal::RemoveVolatile<T>::Type;

    template <typename T>
    /// Removes both `const` and `volatile` qualifiers from a type.
    using RemoveCVType = typename Internal::RemoveCV<T>::Type;

#pragma endregion

#pragma region Void

    template <typename T>
    constexpr bool isVoid = isSame<RemoveCVType<T>, void>;

    namespace Internal
    {
        template <typename T>
        struct IsVoid : Internal::BoolConstant<isVoid<T>>
        {
        };
    }

    template <typename... _Types>
    /// Alias that always resolves to `void`.
    using VoidType = void;

#pragma endregion

#pragma region Reference

    namespace Internal
    {
        template <typename T, typename = void>
        struct AddReference
        {
            using Lvalue = T;
            using Rvalue = T;
        };

        template <typename T>
        struct AddReference<T, VoidType<T&>>
        {
            using Lvalue = T&;
            using Rvalue = T&&;
        };

        template <typename T>
        struct AddLvalueReference
        {
            using Type = typename AddReference<T>::Lvalue;
        };

        template <typename T>
        struct AddRvalueReference
        {
            using Type = typename AddReference<T>::Rvalue;
        };
    }

    template <typename T>
    /// Adds lvalue reference qualifier when possible.
    using AddLvalueReferenceType = typename Internal::AddReference<T>::Lvalue;

    template <typename T>
    /// Adds rvalue reference qualifier when possible.
    using AddRvalueReferenceType = typename Internal::AddReference<T>::Rvalue;

    template <typename T>
    AddRvalueReferenceType<T> declval()
    {
        static_assert(false);
    }

    template <typename>
    constexpr bool isLvalueReference = false;

    template <typename T>
    constexpr bool isLvalueReference<T&> = true;

    template <typename>
    constexpr bool isRvalueReference = false;

    template <typename T>
    constexpr bool isRvalueReference<T&&> = true;

    template <typename>
    constexpr bool isReference = false;

    template <typename T>
    constexpr bool isReference<T&> = true;

    template <typename T>
    constexpr bool isReference<T&&> = true;

    namespace Internal
    {
        template <typename T>
        struct IsLvalueReference : BoolConstant<isLvalueReference<T>>
        {
        };

        template <typename T>
        struct IsRvalueReference : BoolConstant<isRvalueReference<T>>
        {
        };

        template <typename T>
        struct IsReference : BoolConstant<isReference<T>>
        {
        };
    }

#pragma endregion

    template <typename T, typename... Types>
    constexpr bool isAnyOf =
        (isSame<T, Types> || ...);

    namespace Internal
    {
        template <typename T>
        struct IsEnum : BoolConstant<__is_enum(T)>
        {
        };
    }

    template <typename T>
    constexpr bool isEnum = __is_enum(T);

    template <typename>
    constexpr bool isConst = false;

    template <typename T>
    constexpr bool isConst<T const> = true;

    namespace Internal
    {
        template <typename T>
        struct IsConst : Internal::BoolConstant<isConst<T>>
        {
        };
    }

    template <typename>
    constexpr bool isVolatile = false;

    template <typename T>
    constexpr bool isVolatile<volatile T> = true;

    namespace Internal
    {
        template <typename T>
        struct IsVolatile : Internal::BoolConstant<isVolatile<T>>
        {
        };
    }

    template <typename T>
    constexpr bool isFunction =
        !isConst<T const> && !isReference<T>;

    namespace Internal
    {
        template <typename T>
        struct IsFunction : Internal::BoolConstant<isFunction<T>>
        {
        };
    }

    template <typename T>
    constexpr bool isObject =
        isConst<T const> && !isVoid<T>;

    template <typename T>
    struct IsObject : Internal::BoolConstant<isObject<T>>
    {
    };

    template <typename T>
    constexpr bool isEmpty = __is_empty(T);

    template <typename T>
    struct IsEmpty : Internal::BoolConstant<__is_empty(T)>
    {
    };

    template <typename T>
    constexpr bool isPolymorphic = __is_polymorphic(T);

    template <typename T>
    struct IsPolymorphic : Internal::BoolConstant<__is_polymorphic(T)>
    {
    };

    template <typename T>
    constexpr bool isAbstract = __is_abstract(T);

    template <typename T>
    struct IsAbstract : Internal::BoolConstant<__is_abstract(T)>
    {
    };

    template <typename T>
    constexpr bool isFinal = __is_final(T);

    template <typename T>
    struct IsFinal : Internal::BoolConstant<__is_final(T)>
    {
    };

    template <typename T>
    constexpr bool isTrivial = __is_trivial(T);

    template <typename T>
    struct IsTrivial : Internal::BoolConstant<__is_trivial(T)>
    {
    };

    namespace Internal
    {
        template <typename T>
        struct IsTriviallyCopyable : Internal::BoolConstant<__is_trivially_copyable(T)>
        {
        };
    }

    template <typename T>
    constexpr bool isTriviallyCopyable = __is_trivially_copyable(T);

#if defined(ARK_COMPILER_CLANG)

    template <typename T>
    struct IsIntegral : Internal::BoolConstant<__is_integral(T)>
    {
    };

    template <typename T>
    constexpr bool isIntegral = __is_integral(T);

#elif defined(ARK_COMPILER_MSVC)

    template <typename T>
    constexpr bool isIntegral = isAnyOf<
        RemoveCVType<T>,
        bool,
        char,
        signed char,
        unsigned char,
        wchar_t,
        short,
        unsigned short,
        int,
        unsigned int,
        long,
        unsigned long,
        long long,
        unsigned long long>;

    template <typename T>
    struct IsIntegral : Internal::BoolConstant<isIntegral<T>>
    {
    };

#endif

#pragma region Arithmetic

    template <typename T>
    inline constexpr bool isFloatingPoint = isAnyOf<
        RemoveCVType<T>,
        float,
        double,
        long double>;

    template <typename T>
    struct IsFloatingPoint : Internal::BoolConstant<isFloatingPoint<T>>
    {
    };

    namespace Internal
    {
        template <typename T, bool = isIntegral<T>>
        struct SignBase
        {
            using U = typename RemoveCV<T>::Type;

            static constexpr bool Signed = U(-1) < U(0);
            static constexpr bool Unsigned = !Signed;
        };

        template <typename T>
        struct SignBase<T, false>
        {
            static constexpr bool Signed = isFloatingPoint<T>;
            static constexpr bool Unsigned = false;
        };

        template <typename T>
        struct IsSigned : BoolConstant<SignBase<T>::Signed>
        {
        };

        template <typename T>
        struct IsUnsigned : BoolConstant<Internal::SignBase<T>::Unsigned>
        {
        };
    }

    template <typename T>
    constexpr bool isSigned = Internal::SignBase<T>::Signed;

    template <typename T>
    constexpr bool isUnsigned = Internal::SignBase<T>::Unsigned;

    template <typename T>
    constexpr bool signedIntegral = isIntegral<T> && isSigned<T>;

    template <typename T>
    constexpr bool unsignedIntegral = isIntegral<T> && isUnsigned<T>;

    template <typename T>
    constexpr bool isArithmetic = isIntegral<T> || isFloatingPoint<T>;

    template <typename T>
    struct IsArithmetic : Internal::BoolConstant<isArithmetic<T>>
    {
    };

    namespace Internal
    {
        template <typename T>
        struct MakeUnsigned;

        template <>
        struct MakeUnsigned<char>
        {
            using Type = unsigned char;
        };

        template <>
        struct MakeUnsigned<signed char>
        {
            using Type = unsigned char;
        };

        template <>
        struct MakeUnsigned<unsigned char>
        {
            using Type = unsigned char;
        };

        template <>
        struct MakeUnsigned<short>
        {
            using Type = unsigned short;
        };

        template <>
        struct MakeUnsigned<unsigned short>
        {
            using Type = unsigned short;
        };

        template <>
        struct MakeUnsigned<int>
        {
            using Type = unsigned int;
        };

        template <>
        struct MakeUnsigned<unsigned int>
        {
            using Type = unsigned int;
        };

        template <>
        struct MakeUnsigned<long>
        {
            using Type = unsigned long;
        };

        template <>
        struct MakeUnsigned<unsigned long>
        {
            using Type = unsigned long;
        };

        template <>
        struct MakeUnsigned<long long>
        {
            using Type = unsigned long long;
        };

        template <>
        struct MakeUnsigned<unsigned long long>
        {
            using Type = unsigned long long;
        };
    }

    /// Unsigned counterpart of an integral type (Ark equivalent of `std::make_unsigned_t`).
    template <typename T>
    using MakeUnsignedType = typename Internal::MakeUnsigned<RemoveCVType<T>>::Type;

#pragma endregion

#pragma region Pointer

    template <typename>
    constexpr bool isPointer = false;

    template <typename T>
    constexpr bool isPointer<T*> = true;

    template <typename T>
    constexpr bool isPointer<T* const> = true;

    template <typename T>
    constexpr bool isPointer<T* volatile> = true;

    template <typename T>
    constexpr bool isPointer<T* const volatile> = true;

    template <typename T>
    struct IsPointer : Internal::BoolConstant<isPointer<T>>
    {
    };

    template <typename T>
    constexpr bool isNullPointer =
        isSame<RemoveCVType<T>, NullType>;

    template <typename T>
    struct IsNullPointer : Internal::BoolConstant<isNullPointer<T>>
    {
    };

#pragma endregion

    template <typename T>
    struct IsUnion : Internal::BoolConstant<__is_union(T)>
    {
    };

    template <typename T>
    constexpr bool isUnion = __is_union(T);

    template <typename T>
    struct IsClass : Internal::BoolConstant<__is_class(T)>
    {
    };

    template <typename T>
    constexpr bool isClass = __is_class(T);

    template <typename T>
    constexpr bool isFundamental = isArithmetic<T> || isVoid<T> || isNullPointer<T>;

    template <typename T>
    struct IsFundamental : Internal::BoolConstant<isFundamental<T>>
    {
    };

    template <typename From, typename To>
    struct IsConvertible : Internal::BoolConstant<__is_convertible_to(From, To)>
    {
    };

    template <typename From, typename To>
    constexpr bool isConvertible = __is_convertible_to(From, To);

    template <typename T>
    struct IsConvertible<T&, volatile T&> : TrueType
    {
    };

    template <typename T>
    struct IsConvertible<volatile T&, volatile T&> : TrueType
    {
    };

    template <typename T>
    struct IsConvertible<T&, const volatile T&> : TrueType
    {
    };

    template <typename T>
    struct IsConvertible<volatile T&, const volatile T&> : TrueType
    {
    };

    template <typename T>
    constexpr bool isConvertible<T&, volatile T&> = true;

    template <typename T>
    constexpr bool isConvertible<volatile T&, volatile T&> = true;

    template <typename T>
    constexpr bool isConvertible<T&, const volatile T&> = true;

    template <typename T>
    constexpr bool isConvertible<volatile T&, const volatile T&> = true;

#pragma endregion

    namespace Internal
    {
        template <bool>
        struct Select
        {
            template <typename T1, typename>
            using Apply = T1;
        };

        template <>
        struct Select<false>
        {
            template <typename, typename T2>
            using Apply = T2;
        };
    }

#pragma region Reference

    namespace Internal
    {
        template <typename T>
        struct RemoveReference
        {
            using Type = T;
            using ConstThruRefType = T const;
        };

        template <typename T>
        struct RemoveReference<T&>
        {
            using Type = T;
            using ConstThruRefType = T const&;
        };

        template <typename T>
        struct RemoveReference<T&&>
        {
            using Type = T;
            using ConstThruReferenceType = T const&&;
        };

        template <typename T>
        using ConstThruReference = typename RemoveReference<T>::ConstThruReferenceType;
    }

    template <typename T>
    using RemoveReferenceType = typename Internal::RemoveReference<T>::Type;

    namespace Internal
    {
        template <typename T>
        using _RemoveCVReferenceType = RemoveCVType<RemoveReferenceType<T>>;
    }

    template <typename T>
    using RemoveCVReferenceType = Internal::_RemoveCVReferenceType<T>;

    template <typename T>
    struct RemoveCVReference
    {
        using Type = RemoveCVReferenceType<T>;
    };

#pragma endregion

#pragma region Constructible, Assignable, Destructible

    namespace Internal
    {
        template <typename T, typename... Args>
        struct IsConstructible : BoolConstant<__is_constructible(T, Args...)>
        {
        };

        template <typename T>
        struct IsCopyConstructible : BoolConstant<__is_constructible(T, AddLvalueReferenceType<T const>)>
        {
        };

        template <typename T>
        struct IsDefaultConstructible : BoolConstant<__is_constructible(T)>
        {
        };

        template <typename T, typename = void>
        struct IsImplicitlyDefaultConstructible : FalseType
        {
        };

        template <typename T>
        void ImplicitlyDefaultConstruct(T const&);

        template <typename T>
        struct IsImplicitlyDefaultConstructible<T, VoidType<decltype(ImplicitlyDefaultConstruct<T>({}))>> : TrueType
        {
        };

        template <typename T>
        struct IsMoveConstructible : BoolConstant<__is_constructible(T, T)>
        {
        };

        template <typename To, typename From>
        struct IsAssignable : BoolConstant<__is_assignable(To, From)>
        {
        };

        template <typename T>
        struct IsCopyAssignable
            : BoolConstant<__is_assignable(AddLvalueReferenceType<T>, AddLvalueReferenceType<T const>)>
        {
        };

        template <typename T>
        struct IsMoveAssignable : BoolConstant<__is_assignable(AddLvalueReferenceType<T>, T)>
        {
        };

        template <typename T>
        struct IsDestructible : BoolConstant<__is_destructible(T)>
        {
        };

        template <typename T, typename... Args>
        struct IsTriviallyConstructible : BoolConstant<__is_trivially_constructible(T, Args...)>
        {
        };

        template <typename T>
        struct IsTriviallyCopyConstructible
            : BoolConstant<__is_trivially_constructible(T, AddLvalueReferenceType<T const>)>
        {
        };

        template <typename T>
        struct IsTriviallyDefaultConstructible : BoolConstant<__is_trivially_constructible(T)>
        {
        };

        template <typename T>
        struct IsTriviallyMoveConstructible : BoolConstant<__is_trivially_constructible(T, T)>
        {
        };

        template <typename To, typename From>
        struct IsTriviallyAssignable : BoolConstant<__is_trivially_assignable(To, From)>
        {
        };

        template <typename T>
        struct IsTriviallyCopyAssignable
            : BoolConstant<__is_trivially_assignable(AddLvalueReferenceType<T>, AddLvalueReferenceType<T const>)>
        {
        };

        template <typename T>
        struct IsTriviallyMoveAssignable : BoolConstant<__is_trivially_assignable(AddLvalueReferenceType<T>, T)>
        {
        };

        template <typename T>
        struct IsTriviallyDestructible : BoolConstant<__is_trivially_destructible(T)>
        {
        };
    }

    template <typename T, typename... Args>
    constexpr bool isConstructible = __is_constructible(T, Args...);

    template <typename T, typename... Args>
    constexpr bool isNothrowConstructible = __is_nothrow_constructible(T, Args...);

    template <typename T>
    constexpr bool isCopyConstructible = __is_constructible(T, AddLvalueReferenceType<T const>);

    template <typename T>
    constexpr bool isDefaultConstructible = __is_constructible(T);

    template <typename T>
    constexpr bool isMoveConstructible = __is_constructible(T, T);

    template <typename To, typename From>
    constexpr bool isAssignable = __is_assignable(To, From);

    template <typename T>
    constexpr bool isCopyAssignable = __is_assignable(AddLvalueReferenceType<T>, AddLvalueReferenceType<T const>);

    template <typename T>
    constexpr bool isMoveAssignable = __is_assignable(AddLvalueReferenceType<T>, T);

    template <typename T>
    constexpr bool isDestructible = __is_destructible(T);

    template <typename T, typename... Args>
    constexpr bool isTriviallyConstructible = __is_trivially_constructible(T, Args...);

    template <typename T>
    constexpr bool isTriviallyCopyConstructible = __is_trivially_constructible(T, AddLvalueReferenceType<T const>);

    template <typename T>
    constexpr bool isTriviallyDefaultConstructible = __is_trivially_constructible(T);

    template <typename T>
    constexpr bool isTriviallyMoveConstructible = __is_trivially_constructible(T, T);

    template <typename To, typename From>
    constexpr bool isTriviallyAssignable = __is_trivially_assignable(To, From);

    template <typename T>
    constexpr bool IsTriviallyCopyAssignable =
        __is_trivially_assignable(AddLvalueReferenceType<T>, AddLvalueReferenceType<T const>);

    template <typename T>
    constexpr bool isTriviallyMoveAssignable = __is_trivially_assignable(AddLvalueReferenceType<T>, T);

    template <typename T>
    constexpr bool isTriviallyDestructible = __is_trivially_destructible(T);

#pragma endregion

#pragma region Pointer

    namespace Internal
    {
        template <typename T>
        struct RemovePointer
        {
            using Type = T;
        };

        template <typename T>
        struct RemovePointer<T*>
        {
            using Type = T;
        };

        template <typename T>
        struct RemovePointer<T* const>
        {
            using Type = T;
        };

        template <typename T>
        struct RemovePointer<T* volatile>
        {
            using Type = T;
        };

        template <typename T>
        struct RemovePointer<T* const volatile>
        {
            using Type = T;
        };

        template <typename T, typename = void>
        struct _AddPointer
        {
            using Type = T;
        };

        template <typename T>
        struct _AddPointer<T, VoidType<RemoveReferenceType<T>*>>
        {
            using Type = RemoveReferenceType<T>*;
        };

        template <typename T>
        struct AddPointer
        {
            using Type = typename _AddPointer<T>::Type;
        };
    }

    template <typename T>
    using RemovePointerType = typename Internal::RemovePointer<T>::Type;

    template <typename T>
    using AddPointerType = typename Internal::_AddPointer<T>::Type;

#pragma endregion

    namespace Internal
    {
        template <typename T, bool = isEnum<T>>
        struct Underlying
        {
            using Type = __underlying_type(T);
        };

        template <typename T>
        struct Underlying<T, false>
        {
        };
    }

    template <typename T>
    using UnderlyingType = typename Internal::Underlying<T>::Type;

    namespace Internal
    {
        template <typename T>
        struct Decay
        {
            using T1 = RemoveReferenceType<T>;
            using T2 = typename Select<isFunction<T1>>::template Apply<AddPointer<T1>, RemoveCV<T1>>;
            using Type = typename Select<isArray<T1>>::template Apply<AddPointer<RemoveExtentType<T1>>, T2>::Type;
        };
    }

    template <typename T>
    using DecayType = typename Internal::Decay<T>::Type;

#pragma region Common Reference

    namespace Internal
    {
        template <typename...>
        struct CommonReference;

        template <>
        struct CommonReference<>
        {
        };

        template <typename T>
        struct CommonReference<T>
        {
            using Type = T;
        };
    }

    template <typename... Types>
    using CommonReferenceType = Internal::CommonReference<Types...>::Type;

#pragma endregion
}
