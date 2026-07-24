#pragma once

#include "Ark/Core/Traits.hpp"

namespace Ark::Concepts
{
    /// Checks if the type is an enum type.
    template <typename T>
    concept Enum = Traits::isEnum<T>;

    /// Checks if the type is an integral type.
    template <typename T>
    concept Integral = Traits::isIntegral<T>;

    /// Checks if the type is an integral type and is signed.
    template <typename T>
    concept Signed = Traits::isSigned<T>;

    /// Checks if the type is an integral type and is unsigned.
    template <typename T>
    concept Unsigned = Traits::isUnsigned<T>;

    /// Checks if the type is an integral type and is signed.
    template <typename T>
    concept SignedIntegral = Traits::signedIntegral<T>;

    /// Checks if the type is an integral type and is unsigned.
    template <typename T>
    concept UnsignedIntegral = Traits::unsignedIntegral<T>;

    /// Checks if the type is a floating-point type.
    template <typename T>
    concept FloatingPoint = Traits::isFloatingPoint<T>;

    /// Checks if the type is an arithmetic type (integral or floating-point).
    template <typename T>
    concept Arithmetic = Traits::isArithmetic<T>;

    template <typename T1, typename T2>
    concept SameAs = Traits::isSame<T1, T2> && Traits::isSame<T2, T1>;

    template <typename Derived, typename Base>
    concept DerivedFrom =
        __is_base_of(Base, Derived) && __is_convertible_to(const volatile Derived*, const volatile Base*);

    template <typename From, typename To>
    concept ConvertibleTo = __is_convertible_to(From, To) && requires { static_cast<To>(Traits::declval<From>()); };

    template <typename T1, typename T2>
    concept CommonReferenceWith =
        requires {
            typename Traits::CommonReferenceType<T1, T2>;
            typename Traits::CommonReferenceType<T2, T1>;
        } && SameAs<Traits::CommonReferenceType<T1, T2>, Traits::CommonReferenceType<T2, T1>> &&
        ConvertibleTo<T1, Traits::CommonReferenceType<T1, T2>> &&
        ConvertibleTo<T2, Traits::CommonReferenceType<T1, T2>>;

#if 0
    template <typename LType, typename RType>
    concept AssignableFrom = Traits::isLvalueReference<LType> &&
                             CommonReferenceWith<const Traits::RemoveReferenceType<LType>&, const Traits::RemoveReferenceType<RType>&> &&
                             requires(LType Left, RType&& Right) {
                                 { Left = static_cast<RType &&>(Right) } -> SameAs<LType>;
                             };
#else
    template <typename LType, typename RType>
    concept AssignableFrom = requires(LType left, RType&& right) {
        left = static_cast<RType&&>(right);
    };
#endif

    template <typename T>
    concept Destructible = __is_nothrow_destructible(T);

    template <typename T, typename... Args>
    concept ConstructibleFrom = Destructible<T> && __is_constructible(T, Args...);

    template <typename T>
    concept MoveConstructible = ConstructibleFrom<T, T> && ConvertibleTo<T, T>;

    template <typename T>
    concept CopyConstructible = MoveConstructible<T> &&
                                ConstructibleFrom<T, T&> &&
                                ConvertibleTo<T&, T> &&
                                ConstructibleFrom<T, T const&> &&
                                ConvertibleTo<T const&, T> &&
                                ConstructibleFrom<T, const T> &&
                                ConvertibleTo<const T, T>;

    template <typename T>
    concept TriviallyCopyable = Traits::isTriviallyCopyable<T>;

    template <typename T>
    concept CopyAssignable = AssignableFrom<T&, T const&>;

    template <typename T>
    concept MoveAssignable = AssignableFrom<T&, T>;

    template <typename T>
    concept Swappable = requires(T& a, T& b) {
        swap(a, b);
    };

#pragma region Comparable

    namespace Internal
    {
        template <typename T>
        concept ConvertibleToBool = ConvertibleTo<T, bool>;

        template <class T>
        concept BooleanTestable = ConvertibleToBool<T> && requires(T&& t) {
            { !static_cast<T&&>(t) } -> ConvertibleToBool;
        };

        template <typename T1, typename T2>
        concept HalfEqualityComparable = requires(const Traits::RemoveReferenceType<T1>& x, const Traits::RemoveReferenceType<T2>& y) {
            { x == y } -> BooleanTestable;
            { x != y } -> BooleanTestable;
        };

        template <typename T1, typename T2>
        concept WeaklyEqualityComparableWith =
            HalfEqualityComparable<T1, T2> && HalfEqualityComparable<T2, T1>;
    }

    template <typename T>
    concept EqualityComparable = Internal::HalfEqualityComparable<T, T>;

    namespace Internal
    {
        template <typename T1, typename T2, typename _Ref = Traits::CommonReferenceType<const T1&, const T2&>>
        concept ComparisonCommonTypeWithInternal = SameAs<_Ref, Traits::CommonReferenceType<const T2&, const T1&>> && requires {
            requires ConvertibleTo<const T1&, const _Ref&> || ConvertibleTo<T1, const _Ref&>;
            requires ConvertibleTo<const T2&, const _Ref&> || ConvertibleTo<T2, const _Ref&>;
        };

        template <typename T1, typename T2>
        concept ComparisonCommonTypeWith = ComparisonCommonTypeWithInternal<Traits::RemoveCVReferenceType<T1>, Traits::RemoveCVReferenceType<T2>>;
    }

    template <typename T1, typename T2>
    concept EqualityComparableWith =
        EqualityComparable<T1> &&
        EqualityComparable<T2> &&
        Internal::ComparisonCommonTypeWith<T1, T2> &&
        EqualityComparable<Traits::CommonReferenceType<const Traits::RemoveReferenceType<T1>&, const Traits::RemoveReferenceType<T2>&>> && Internal::WeaklyEqualityComparableWith<T1, T2>;

    namespace Internal
    {
        template <class T1, class T2>
        concept HalfOrdered = requires(const Traits::RemoveReferenceType<T1>& t, const Traits::RemoveReferenceType<T2>& u) {
            { t < u } -> BooleanTestable;
            { t > u } -> BooleanTestable;
            { t <= u } -> BooleanTestable;
            { t >= u } -> BooleanTestable;
        };

        template <class T1, class T2>
        concept PartiallyOrderedWith = HalfOrdered<T1, T2> && HalfOrdered<T2, T1>;
    }

    template <class T>
    concept TotallyOrdered = EqualityComparable<T> && Internal::HalfOrdered<T, T>;

#pragma endregion
}
