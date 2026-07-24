#pragma once

#include "Ark/Core/Concepts.hpp"
#include "Ark/Core/Debug/Assert.hpp"
#include "Ark/Core/Functions.hpp"
#include "Ark/Core/Internal/StdLib.hpp"
#include "Ark/Core/Types.hpp"

#include <new>

namespace Ark
{
    struct NoneOption final
    {
        struct Tag final
        {
        };

        constexpr explicit NoneOption(Tag)
        {
        }
    };

    inline constexpr NoneOption none{NoneOption::Tag{}};

    namespace Internal
    {
        struct NonTrivialDummy final
        {
            constexpr NonTrivialDummy()
            {
            }
        };

        /// Base struct handling destruction based on trivial destructibility
        template <typename T, bool = Traits::isTriviallyDestructible<T>>
        struct OptionDestructBase
        {
            union
            {
                NonTrivialDummy dummy;
                Traits::RemoveCVType<T> value;
            };

            bool hasValue;

            constexpr OptionDestructBase()
                : dummy{}
                , hasValue{false}
            {
            }

            template <typename... Args>
            constexpr explicit OptionDestructBase(InPlace, Args&&... args)
                : value(forward<Args>(args)...)
                , hasValue{true}
            {
            }

            constexpr void reset()
            {
                hasValue = false;
            }
        };

        /// Specialization for non-trivially destructible types
        template <typename T>
        struct OptionDestructBase<T, false>
        {
            union
            {
                NonTrivialDummy dummy;
                Traits::RemoveCVType<T> value;
            };

            bool hasValue;

            constexpr ~OptionDestructBase()
            {
                if (hasValue)
                {
                    value.~T();
                }
            }

            constexpr OptionDestructBase()
                : dummy{}
                , hasValue{false}
            {
            }

            template <typename... Args>
            constexpr explicit OptionDestructBase(InPlace, Args&&... args)
                : value(forward<Args>(args)...)
                , hasValue{true}
            {
            }

            OptionDestructBase(OptionDestructBase const&) = default;
            OptionDestructBase(OptionDestructBase&&) = default;
            OptionDestructBase& operator=(OptionDestructBase const&) = default;
            OptionDestructBase& operator=(OptionDestructBase&&) = default;

            constexpr void reset()
            {
                if (hasValue)
                {
                    value.~T();
                    hasValue = false;
                }
            }
        };

        /// Base struct providing construction and assignment operations
        template <typename T>
        struct OptionConstructBase : OptionDestructBase<T>
        {
            using OptionDestructBase<T>::OptionDestructBase;

            template <typename... Args>
            constexpr T& construct(Args&&... args)
            {
                ::new (static_cast<void*>(&this->value)) T(forward<Args>(args)...);
                this->hasValue = true;
                return this->value;
            }

            template <typename U>
            constexpr void assign(U&& right)
            {
                if (this->hasValue)
                {
                    this->value = forward<U>(right);
                }
                else
                {
                    construct(forward<U>(right));
                }
            }

            template <typename Self>
            constexpr void constructFrom(Self&& right)
            {
                if (static_cast<bool>(right))
                {
                    construct(*forward<Self>(right));
                }
            }

            template <typename Self>
            constexpr void assignFrom(Self&& right)
            {
                if (static_cast<bool>(right))
                {
                    assign(*forward<Self>(right));
                }
                else
                {
                    this->reset();
                }
            }

            constexpr T& operator*() &
            {
                return this->value;
            }

            constexpr T const& operator*() const&
            {
                return this->value;
            }

            constexpr T&& operator*() &&
            {
                return Ark::move(this->value);
            }

            constexpr T const&& operator*() const&&
            {
                return Ark::move(this->value);
            }
        };
    }

    /// Option type that may or may not contain a value.
    /// @note Reference types use the `Option<T&>` specialization (nullable alias / pointer storage).
    /// `Option<T&&>` is not supported.
    template <typename T>
    struct Option : private Internal::OptionConstructBase<T>
    {
    private:
        static_assert(!Traits::isReference<T>, "Option reference types use Option<T&>; Option<T&&> is not supported");

        using Base = Internal::OptionConstructBase<T>;

        template <typename>
        friend struct Option;

    public:
        using ValueType = T;

        constexpr Option() = default;

        constexpr Option(NoneOption)
        {
        }

        template <typename... Args>
        requires Concepts::ConstructibleFrom<T, Args...>
        constexpr explicit Option(InPlace, Args&&... args)
            : Base(inPlace, forward<Args>(args)...)
        {
        }

        template <typename Elem, typename... Args>
        requires Concepts::ConstructibleFrom<T, InitializerList<Elem>&, Args...>
        constexpr explicit Option(InPlace, InitializerList<Elem> list, Args&&... args)
            : Base(inPlace, list, forward<Args>(args)...)
        {
        }

        template <typename U = Traits::RemoveCVType<T>>
        constexpr explicit(!Concepts::ConvertibleTo<U, T>) Option(U&& value)
            : Base(inPlace, Ark::forward<U>(value))
        {
        }

        template <typename U>
        constexpr explicit(!Concepts::ConvertibleTo<U const&, T>) Option(Option<U> const& other)
        {
            if (other)
            {
                this->construct(*other);
            }
        }

        template <typename U>
        constexpr explicit(!Concepts::ConvertibleTo<U, T>) Option(Option<U>&& other)
        {
            if (other)
            {
                this->construct(Ark::move(*other));
            }
        }

        // Same-type copy and move constructors, default-constructing base to avoid copying union state
        constexpr Option(Option const& other)
        requires Concepts::CopyConstructible<T>
            : Base()
        {
            this->constructFrom(other);
        }

        constexpr Option(Option&& other)
        requires Concepts::MoveConstructible<T>
            : Base()
        {
            this->constructFrom(Ark::move(other));
        }

        constexpr Option& operator=(NoneOption)
        {
            reset();
            return *this;
        }

        template <typename U = Traits::RemoveCVType<T>>
        requires(!Traits::isSame<Option<T>, Traits::RemoveCVReferenceType<U>>) &&
                Concepts::ConstructibleFrom<T, U> &&
                Concepts::AssignableFrom<T&, U>
        constexpr Option& operator=(U&& value)
        {
            this->assign(forward<U>(value));
            return *this;
        }

        template <typename U>
        constexpr Option& operator=(Option<U> const& other)
        {
            this->assignFrom(other);
            return *this;
        }

        template <typename U>
        constexpr Option& operator=(Option<U>&& other)
        {
            this->assignFrom(Ark::move(other));
            return *this;
        }

        // Same-type copy/move assignments
        constexpr Option& operator=(Option const& other)
        requires Concepts::CopyConstructible<T> && Concepts::AssignableFrom<T&, T const&>
        {
            if (this != &other)
            {
                this->assignFrom(other);
            }
            return *this;
        }

        constexpr Option& operator=(Option&& other)
        requires Concepts::MoveConstructible<T> && Concepts::AssignableFrom<T&, T>
        {
            if (this != &other)
            {
                this->assignFrom(Ark::move(other));
            }
            return *this;
        }

        template <typename... Args>
        requires Concepts::ConstructibleFrom<T, Args...>
        constexpr T& emplace(Args&&... args)
        {
            reset();
            return this->construct(forward<Args>(args)...);
        }

        template <typename Elem, typename... Args>
        requires Concepts::ConstructibleFrom<T, InitializerList<Elem>&, Args...>
        constexpr T& emplace(InitializerList<Elem> list, Args&&... args)
        {
            reset();
            return this->construct(list, forward<Args>(args)...);
        }

        constexpr void swap(Option& other)
        requires Concepts::MoveConstructible<T>
        {
            bool const engaged = static_cast<Base const&>(*this).hasValue;
            if (engaged == static_cast<Base const&>(other).hasValue)
            {
                if (engaged)
                {
                    // Both have values - swap them
                    T temp = Ark::move(**this);
                    **this = Ark::move(*other);
                    *other = Ark::move(temp);
                }
                // Both empty - nothing to do
            }
            else
            {
                // One has value, other doesn't
                Option& source = engaged ? *this : other;
                Option& target = engaged ? other : *this;
                target.construct(Ark::move(*source));
                source.reset();
            }
        }

        /// Access value pointer (undefined behavior if empty)
        constexpr T const* operator->() const
        {
            return &static_cast<Base const&>(*this).value;
        }

        constexpr T* operator->()
        {
            return &static_cast<Base&>(*this).value;
        }

        /// Access value reference (undefined behavior if empty)
        using Base::operator*;

        /// Check if Option contains a value
        constexpr explicit operator bool() const
        {
            return static_cast<Base const&>(*this).hasValue;
        }

        /// Check if Option contains a value
        constexpr bool hasValue() const
        {
            return static_cast<Base const&>(*this).hasValue;
        }

        /// Access value with exception on empty
        constexpr T const& getValue() const&
        {
            if (!static_cast<Base const&>(*this).hasValue)
            {
                ARK_FATAL_ERROR_MSG("Option does not contain a value.");
            }

            return static_cast<Base const&>(*this).value;
        }

        constexpr T& getValue() &
        {
            if (!static_cast<Base const&>(*this).hasValue)
            {
                ARK_FATAL_ERROR_MSG("Option does not contain a value.");
            }

            return static_cast<Base&>(*this).value;
        }

        constexpr T&& getValue() &&
        {
            if (!static_cast<Base const&>(*this).hasValue)
            {
                ARK_FATAL_ERROR_MSG("Option does not contain a value.");
            }

            return Ark::move(static_cast<Base&>(*this).value);
        }

        constexpr T const&& getValue() const&&
        {
            if (!static_cast<Base const&>(*this).hasValue)
            {
                ARK_FATAL_ERROR_MSG("Option does not contain a value.");
            }

            return Ark::move(static_cast<Base const&>(*this).value);
        }

        /// Get value or default
        template <typename U = Traits::RemoveCVType<T>>
        requires Concepts::ConvertibleTo<T const&, Traits::RemoveCVType<T>> &&
                 Concepts::ConvertibleTo<U, Traits::RemoveCVType<T>>
        constexpr Traits::RemoveCVType<T> getValueOr(U&& defaultValue) const&
        {
            return static_cast<Base const&>(*this).hasValue ? static_cast<Traits::RemoveCVType<T>>(static_cast<Base const&>(*this).value) : //
                       static_cast<Traits::RemoveCVType<T>>(forward<U>(defaultValue));
        }

        template <typename U = Traits::RemoveCVType<T>>
        requires Concepts::ConvertibleTo<T, Traits::RemoveCVType<T>> &&
                 Concepts::ConvertibleTo<U, Traits::RemoveCVType<T>>
        constexpr Traits::RemoveCVType<T> getValueOr(U&& defaultValue) &&
        {
            return static_cast<Base const&>(*this).hasValue ? static_cast<Traits::RemoveCVType<T>>(Ark::move(static_cast<Base&>(*this).value)) : //
                       static_cast<Traits::RemoveCVType<T>>(forward<U>(defaultValue));
        }

        /// Chains a function returning an Option when this Option has a value (C++23 `and_then`).
        template <typename Func>
        constexpr auto andThen(Func&& func) & -> decltype(func(Traits::declval<T&>()))
        {
            using ReturnType = decltype(func(Traits::declval<T&>()));
            if (hasValue())
            {
                return func(**this);
            }
            return ReturnType{};
        }

        template <typename Func>
        constexpr auto andThen(Func&& func) const& -> decltype(func(Traits::declval<T const&>()))
        {
            using ReturnType = decltype(func(Traits::declval<T const&>()));
            if (hasValue())
            {
                return func(**this);
            }
            return ReturnType{};
        }

        template <typename Func>
        constexpr auto andThen(Func&& func) && -> decltype(func(Traits::declval<T&&>()))
        {
            using ReturnType = decltype(func(Traits::declval<T&&>()));
            if (hasValue())
            {
                return func(Ark::move(**this));
            }
            return ReturnType{};
        }

        /// Maps the contained value into a new Option (C++23 `transform`).
        template <typename Func>
        constexpr auto transform(Func&& func) &
        {
            using U = Traits::DecayType<decltype(func(Traits::declval<T&>()))>;
            if (hasValue())
            {
                return Option<U>{func(**this)};
            }
            return Option<U>{};
        }

        template <typename Func>
        constexpr auto transform(Func&& func) const&
        {
            using U = Traits::DecayType<decltype(func(Traits::declval<T const&>()))>;
            if (hasValue())
            {
                return Option<U>{func(**this)};
            }
            return Option<U>{};
        }

        template <typename Func>
        constexpr auto transform(Func&& func) &&
        {
            using U = Traits::DecayType<decltype(func(Traits::declval<T&&>()))>;
            if (hasValue())
            {
                return Option<U>{func(Ark::move(**this))};
            }
            return Option<U>{};
        }

        /// Returns this Option if engaged, otherwise the result of `func()` (C++23 `or_else`).
        template <typename Func>
        constexpr auto orElse(Func&& func) const& -> decltype(func())
        {
            using ReturnType = decltype(func());
            if (hasValue())
            {
                return ReturnType{**this};
            }
            return func();
        }

        template <typename Func>
        constexpr auto orElse(Func&& func) && -> decltype(func())
        {
            using ReturnType = decltype(func());
            if (hasValue())
            {
                return ReturnType{Ark::move(**this)};
            }
            return func();
        }

        /// Reset to empty state
        constexpr void reset()
        {
            this->Base::reset();
        }
    };

    /// Option specialization for lvalue references: a nullable alias (stores `T*`).
    /// @details Assignment rebinds the reference; it does not assign through to the referred object.
    /// Constness of the `Option` protects the binding, not the referred object (like `T* const`).
    template <typename T>
    struct Option<T&> final
    {
    private:
        template <typename>
        friend struct Option;

        T* pointer{nullptr};

    public:
        using ValueType = T&;

        constexpr Option() = default;

        constexpr Option(NoneOption)
            : pointer{nullptr}
        {
        }

        constexpr Option(T& value)
            : pointer{&value}
        {
        }

        constexpr Option(Option const&) = default;
        constexpr Option(Option&&) = default;
        constexpr Option& operator=(Option const&) = default;
        constexpr Option& operator=(Option&&) = default;
        ~Option() = default;

        /// Rebinds to `value`, or clears when assigning `none`.
        constexpr Option& operator=(NoneOption)
        {
            pointer = nullptr;
            return *this;
        }

        /// Rebinds this option to refer to `value`.
        constexpr Option& operator=(T& value)
        {
            pointer = &value;
            return *this;
        }

        /// Rebinds to `value` (same as constructing from `T&`).
        constexpr T& emplace(T& value)
        {
            pointer = &value;
            return *pointer;
        }

        constexpr void swap(Option& other)
        {
            T* temporary = pointer;
            pointer = other.pointer;
            other.pointer = temporary;
        }

        /// Access referred object pointer (undefined behavior if empty).
        constexpr T* operator->() const
        {
            return pointer;
        }

        /// Access referred object (undefined behavior if empty).
        constexpr T& operator*() const
        {
            return *pointer;
        }

        constexpr explicit operator bool() const
        {
            return pointer != nullptr;
        }

        constexpr bool hasValue() const
        {
            return pointer != nullptr;
        }

        constexpr T& getValue() const
        {
            if (pointer == nullptr)
            {
                ARK_FATAL_ERROR_MSG("Option does not contain a value.");
            }
            return *pointer;
        }

        /// Returns the referred object, or `defaultValue` when empty (both are lvalue references).
        constexpr T& getValueOr(T& defaultValue) const
        {
            return pointer != nullptr ? *pointer : defaultValue;
        }

        /// Chains a function returning an Option when this Option has a value (C++23 `and_then`).
        template <typename Func>
        constexpr auto andThen(Func&& func) const -> decltype(func(Traits::declval<T&>()))
        {
            using ReturnType = decltype(func(Traits::declval<T&>()));
            if (hasValue())
            {
                return func(**this);
            }
            return ReturnType{};
        }

        /// Maps the referred value into a new Option (C++23 `transform`).
        template <typename Func>
        constexpr auto transform(Func&& func) const
        {
            using U = Traits::DecayType<decltype(func(Traits::declval<T&>()))>;
            if (hasValue())
            {
                return Option<U>{func(**this)};
            }
            return Option<U>{};
        }

        /// Returns this Option if engaged, otherwise the result of `func()` (C++23 `or_else`).
        template <typename Func>
        constexpr auto orElse(Func&& func) const -> decltype(func())
        {
            using ReturnType = decltype(func());
            if (hasValue())
            {
                return ReturnType{**this};
            }
            return func();
        }

        constexpr void reset()
        {
            pointer = nullptr;
        }
    };

    /// Rvalue-reference options are intentionally unsupported (dangling aliases).
    template <typename T>
    struct Option<T&&>
    {
        static_assert(Traits::isSame<T, void>, "Option<T&&> is not supported; use Option<T&> for lvalue references");
    };

    template <typename T1, typename T2>
    constexpr bool operator==(Option<T1> const& left, Option<T2> const& right)
    {
        bool const leftHasValue = left.hasValue();
        bool const rightHasValue = right.hasValue();
        if (leftHasValue && rightHasValue)
        {
            return *left == *right;
        }
        return leftHasValue == rightHasValue;
    }

    template <typename T1, typename T2>
    constexpr bool operator!=(Option<T1> const& left, Option<T2> const& right)
    {
        return !(left == right);
    }

    template <typename T1, typename T2>
    constexpr bool operator<(Option<T1> const& left, Option<T2> const& right)
    {
        bool const leftHasValue = left.hasValue();
        bool const rightHasValue = right.hasValue();
        if (leftHasValue && rightHasValue)
        {
            return *left < *right;
        }
        return leftHasValue < rightHasValue;
    }

    template <typename T1, typename T2>
    constexpr bool operator>(Option<T1> const& left, Option<T2> const& right)
    {
        return right < left;
    }

    template <typename T1, typename T2>
    constexpr bool operator<=(Option<T1> const& left, Option<T2> const& right)
    {
        return !(right < left);
    }

    template <typename T1, typename T2>
    constexpr bool operator>=(Option<T1> const& left, Option<T2> const& right)
    {
        return !(left < right);
    }

    template <typename T>
    constexpr bool operator==(Option<T> const& option, NoneOption)
    {
        return !option.hasValue();
    }

    template <typename T>
    constexpr bool operator==(NoneOption, Option<T> const& option)
    {
        return !option.hasValue();
    }

    template <typename T>
    constexpr bool operator!=(Option<T> const& option, NoneOption)
    {
        return option.hasValue();
    }

    template <typename T>
    constexpr bool operator!=(NoneOption, Option<T> const& option)
    {
        return option.hasValue();
    }

    template <typename T>
    constexpr bool operator<(Option<T> const&, NoneOption)
    {
        return false;
    }

    template <typename T>
    constexpr bool operator<(NoneOption, Option<T> const& option)
    {
        return option.hasValue();
    }

    template <typename T>
    constexpr bool operator>(Option<T> const& option, NoneOption)
    {
        return option.hasValue();
    }

    template <typename T>
    constexpr bool operator>(NoneOption, Option<T> const&)
    {
        return false;
    }

    template <typename T>
    constexpr bool operator<=(Option<T> const& option, NoneOption)
    {
        return !option.hasValue();
    }

    template <typename T>
    constexpr bool operator<=(NoneOption, Option<T> const&)
    {
        return true;
    }

    template <typename T>
    constexpr bool operator>=(Option<T> const&, NoneOption)
    {
        return true;
    }

    template <typename T>
    constexpr bool operator>=(NoneOption, Option<T> const& option)
    {
        return !option.hasValue();
    }

    template <typename T1, typename T2>
    constexpr bool operator==(Option<T1> const& option, T2 const& value)
    {
        return option ? *option == value : false;
    }

    template <typename T1, typename T2>
    constexpr bool operator==(T1 const& value, Option<T2> const& option)
    {
        return option ? value == *option : false;
    }

    template <typename T1, typename T2>
    constexpr bool operator!=(Option<T1> const& option, T2 const& value)
    {
        return option ? *option != value : true;
    }

    template <typename T1, typename T2>
    constexpr bool operator!=(T1 const& value, Option<T2> const& option)
    {
        return option ? value != *option : true;
    }

    template <typename T1, typename T2>
    constexpr bool operator<(Option<T1> const& option, T2 const& value)
    {
        return option ? *option < value : true;
    }

    template <typename T1, typename T2>
    constexpr bool operator<(T1 const& value, Option<T2> const& option)
    {
        return option ? value < *option : false;
    }

    template <typename T1, typename T2>
    constexpr bool operator>(Option<T1> const& option, T2 const& value)
    {
        return option ? *option > value : false;
    }

    template <typename T1, typename T2>
    constexpr bool operator>(T1 const& value, Option<T2> const& option)
    {
        return option ? value > *option : true;
    }

    template <typename T1, typename T2>
    constexpr bool operator<=(Option<T1> const& option, T2 const& value)
    {
        return option ? *option <= value : true;
    }

    template <typename T1, typename T2>
    constexpr bool operator<=(T1 const& value, Option<T2> const& option)
    {
        return option ? value <= *option : false;
    }

    template <typename T1, typename T2>
    constexpr bool operator>=(Option<T1> const& option, T2 const& value)
    {
        return option ? *option >= value : false;
    }

    template <typename T1, typename T2>
    constexpr bool operator>=(T1 const& value, Option<T2> const& option)
    {
        return option ? value >= *option : true;
    }

    template <typename T>
    requires Concepts::MoveConstructible<T>
    constexpr void swap(Option<T>& left, Option<T>& right)
    {
        left.swap(right);
    }

    template <typename T>
    constexpr void swap(Option<T&>& left, Option<T&>& right)
    {
        left.swap(right);
    }

    template <typename T>
    constexpr Option<Traits::DecayType<T>> makeOption(T&& value)
    {
        return Option<Traits::DecayType<T>>{forward<T>(value)};
    }

    /// Creates an `Option<T&>` bound to `value` (does not decay or copy).
    template <typename T>
    constexpr Option<T&> makeOptionRef(T& value)
    {
        return Option<T&>{value};
    }

    template <typename T, typename... Args>
    requires Concepts::ConstructibleFrom<T, Args...>
    constexpr Option<T> makeOption(Args&&... args)
    {
        return Option<T>{inPlace, forward<Args>(args)...};
    }

    template <typename T, typename Elem, typename... Args>
    requires Concepts::ConstructibleFrom<T, InitializerList<Elem>&, Args...>
    constexpr Option<T> makeOption(InitializerList<Elem> list, Args&&... args)
    {
        return Option<T>{inPlace, list, forward<Args>(args)...};
    }
}
