#pragma once

#include "Ark/Core/Concepts.hpp"
#include "Ark/Core/Configuration.hpp"
#include "Ark/Core/Debug/Assert.hpp"
#include "Ark/Core/Macros.hpp"
#include "Ark/Core/Traits.hpp"
#include "Ark/Core/Types.hpp"
#include "Ark/Core/Utilities.hpp"

#include <new>

namespace Ark
{
    /// Represents an unexpected error value
    /// @tparam TError The error type
    template <typename TError>
    class Error
    {
        static_assert(Traits::isObject<TError>, "Error type must be an object type");
        static_assert(!Traits::isArray<TError>, "Error type must not be an array type");
        static_assert(!Traits::isConst<TError>, "Error type must not be const");
        static_assert(!Traits::isVolatile<TError>, "Error type must not be volatile");

        template <typename T, typename E>
        friend struct Result;

    public:
        /// Constructs an unexpected error value
        /// @tparam UError Error type for construction
        /// @param error The error value
        template <typename UError = TError>
        requires(!Concepts::SameAs<Traits::RemoveCVReferenceType<UError>, Error> &&
                 Concepts::ConstructibleFrom<TError, UError>)
        explicit Error(UError&& error)
            : error(Ark::forward<UError>(error))
        {
        }

        /// In-place construction of error
        /// @tparam Args Constructor argument types
        /// @param args Constructor arguments
        template <typename... Args>
        requires Concepts::ConstructibleFrom<TError, Args...>
        explicit Error(Args&&... args)
            : error(Ark::forward<Args>(args)...)
        {
        }

        /// Gets the error value (const lvalue reference)
        TError const& getError() const&
        {
            return error;
        }

        /// Gets the error value (lvalue reference)
        TError& getError() &
        {
            return error;
        }

        /// Gets the error value (const rvalue reference)
        TError const&& getError() const&&
        {
            return Ark::move(error);
        }

        /// Gets the error value (rvalue reference)
        TError&& getError() &&
        {
            return Ark::move(error);
        }

        /// Swaps two unexpected values
        /// @param other The other unexpected value
        void swap(Error& other)
        {
            using ::Ark::swap;
            swap(error, other.error);
        }

        /// Equality comparison
        /// @tparam UError Other error type
        /// @param other The other unexpected value
        template <typename UError>
        bool operator==(Error<UError> const& other) const
        {
            return error == other.getError();
        }

        /// Inequality comparison
        /// @tparam UError Other error type
        /// @param other The other unexpected value
        template <typename UError>
        bool operator!=(Error<UError> const& other) const
        {
            return !(*this == other);
        }

    private:
        TError error;
    };

    /// Deduction guide for Error
    template <typename TError>
    Error(TError) -> Error<TError>;

    /// Tag type for in-place error construction of `Result` (like `std::unexpect`).
    struct UnexpectedResultTag
    {
        explicit UnexpectedResultTag() = default;
    };

    /// Tag constant for constructing a `Result` in the error state.
    /// Prefer `Error{…}` when the error value is already built.
    inline constexpr UnexpectedResultTag unexpectedResult{};

    /// Alias for `unexpectedResult`.
    inline constexpr UnexpectedResultTag asError{};

    namespace Internal
    {
        /// Tag for constructing Result from invoke result
        struct ConstructResultFromInvokeTag
        {
            explicit ConstructResultFromInvokeTag() = default;
        };

        /// Checks if a type is a valid Result value type
        template <typename T>
        struct CheckResultArgument : Traits::TrueType
        {
            static_assert(!Traits::isReference<T>, "Result value type must not be a reference");
            static_assert(!Traits::isFunction<T>, "Result value type must not be a function");
            static_assert(!Traits::isArray<T>, "Result value type must not be an array");
        };

        /// Helper to reinitialize Result storage with the strongest available exception safety.
        template <typename First, typename Second, typename... Args>
        void reinitResult(First& newVal, Second& oldVal, Args&&... args)
        {
            if constexpr (Traits::isNothrowConstructible<First, Args...>)
            {
                oldVal.~Second();
                ::new (static_cast<void*>(&newVal)) First(Ark::forward<Args>(args)...);
            }
            else if constexpr (Traits::isMoveConstructible<First>)
            {
                First tmp(Ark::forward<Args>(args)...);
                oldVal.~Second();
                ::new (static_cast<void*>(&newVal)) First(Ark::move(tmp));
            }
            else
            {
                Second tmp(Ark::move(oldVal));
                oldVal.~Second();
                ::new (static_cast<void*>(&newVal)) First(Ark::forward<Args>(args)...);
            }
        }
    }

    /// A type that represents either a successful value or an error
    /// @tparam T The value type
    /// @tparam TError The error type
    template <typename T, typename TError = Void>
    struct Result
    {
        static_assert(Internal::CheckResultArgument<T>::value);
        static_assert(Internal::CheckResultArgument<TError>::value);

        template <typename U, typename UError>
        friend struct Result;

    public:
        using ValueType = T;
        using ErrorType = TError;
        using UnexpectedType = Error<TError>;

    private:
        union
        {
            T value;
            TError error;
        };

        bool hasValue;

    public:
        Result()
        requires Concepts::ConstructibleFrom<T>
            : value()
            , hasValue(true)
        {
        }

        Result(Result const& other)
        requires(Concepts::ConstructibleFrom<T, T const&> &&
                 Concepts::ConstructibleFrom<TError, TError const&>)
            : hasValue(other.hasValue)
        {
            if (hasValue)
            {
                new (&value) T(other.value);
            }
            else
            {
                new (&error) TError(other.error);
            }
        }

        Result(Result&& other)
        requires(Concepts::ConstructibleFrom<T, T &&> &&
                 Concepts::ConstructibleFrom<TError, TError &&>)
            : hasValue(other.hasValue)
        {
            if (hasValue)
            {
                new (&value) T(Ark::move(other.value));
            }
            else
            {
                new (&error) TError(Ark::move(other.error));
            }
        }

        /// Construct from value
        /// @tparam U Value type
        /// @param val The value
        template <typename U = T>
        requires(!Concepts::SameAs<Traits::RemoveCVReferenceType<U>, Result> &&
                 Concepts::ConstructibleFrom<T, U>)
        Result(U&& val)
            : value(Ark::forward<U>(val))
            , hasValue(true)
        {
        }

        /// Construct from unexpected error
        /// @tparam UError Error type
        /// @param unexp The unexpected error
        template <typename UError>
        requires Concepts::ConstructibleFrom<TError, UError const&>
        Result(Error<UError> const& unexp)
            : error(unexp.getError())
            , hasValue(false)
        {
        }

        /// Construct from unexpected error (move)
        /// @tparam UError Error type
        /// @param unexp The unexpected error
        template <typename UError>
        requires Concepts::ConstructibleFrom<TError, UError&&>
        Result(Error<UError>&& unexp)
            : error(Ark::move(unexp.getError()))
            , hasValue(false)
        {
        }

        /// In-place value construction
        /// @tparam Args Constructor argument types
        /// @param args Constructor arguments
        template <typename... Args>
        requires Concepts::ConstructibleFrom<T, Args...>
        explicit Result(Args&&... args)
            : value(Ark::forward<Args>(args)...)
            , hasValue(true)
        {
        }

        /// In-place error construction
        /// @tparam Args Constructor argument types
        /// @param args Constructor arguments
        template <typename... Args>
        requires Concepts::ConstructibleFrom<TError, Args...>
        explicit Result(UnexpectedResultTag, Args&&... args)
            : error(Ark::forward<Args>(args)...)
            , hasValue(false)
        {
        }

        ~Result()
        {
            if (hasValue)
            {
                value.~T();
            }
            else
            {
                error.~TError();
            }
        }

        /// Copy assignment
        Result& operator=(Result const& other)
        requires(Concepts::ConstructibleFrom<T, T const&> &&
                 Concepts::ConstructibleFrom<TError, TError const&>)
        {
            if (hasValue && other.hasValue)
            {
                value = other.value;
            }
            else if (hasValue)
            {
                Internal::reinitResult(error, value, other.error);
            }
            else if (other.hasValue)
            {
                Internal::reinitResult(value, error, other.value);
            }
            else
            {
                error = other.error;
            }

            hasValue = other.hasValue;
            return *this;
        }

        /// Move assignment
        Result& operator=(Result&& other)
        requires(Concepts::ConstructibleFrom<T, T &&> &&
                 Concepts::ConstructibleFrom<TError, TError &&>)
        {
            if (hasValue && other.hasValue)
            {
                value = Ark::move(other.value);
            }
            else if (hasValue)
            {
                Internal::reinitResult(error, value, Ark::move(other.error));
            }
            else if (other.hasValue)
            {
                Internal::reinitResult(value, error, Ark::move(other.value));
            }
            else
            {
                error = Ark::move(other.error);
            }

            hasValue = other.hasValue;
            return *this;
        }

        /// Value assignment
        /// @tparam U Value type
        /// @param val The value
        template <typename U>
        requires(!Concepts::SameAs<Traits::RemoveCVReferenceType<U>, Result> &&
                 Concepts::ConstructibleFrom<T, U>)
        Result& operator=(U&& val)
        {
            if (hasValue)
            {
                value = Ark::forward<U>(val);
            }
            else
            {
                Internal::reinitResult(value, error, Ark::forward<U>(val));
                hasValue = true;
            }
            return *this;
        }

        /// Error assignment
        /// @tparam UError Error type
        /// @param unexp The unexpected error
        template <typename UError>
        requires Concepts::ConstructibleFrom<TError, UError const&>
        Result& operator=(Error<UError> const& unexp)
        {
            if (hasValue)
            {
                Internal::reinitResult(error, value, unexp.getError());
                hasValue = false;
            }
            else
            {
                error = unexp.getError();
            }
            return *this;
        }

        /// Error assignment (move)
        /// @tparam UError Error type
        /// @param unexp The unexpected error
        template <typename UError>
        requires Concepts::ConstructibleFrom<TError, UError&&>
        Result& operator=(Error<UError>&& unexp)
        {
            if (hasValue)
            {
                Internal::reinitResult(error, value, Ark::move(unexp.getError()));
                hasValue = false;
            }
            else
            {
                error = Ark::move(unexp.getError());
            }
            return *this;
        }

        /// Swaps two Results
        /// @param other The other Result
        void swap(Result& other)
        {
            using ::Ark::swap;
            if (hasValue && other.hasValue)
            {
                swap(value, other.value);
            }
            else if (hasValue)
            {
                TError tmp(Ark::move(other.error));
                other.error.~TError();
                new (&other.value) T(Ark::move(value));
                value.~T();
                new (&error) TError(Ark::move(tmp));
                hasValue = false;
                other.hasValue = true;
            }
            else if (other.hasValue)
            {
                other.swap(*this);
            }
            else
            {
                swap(error, other.error);
            }
        }

        /// Checks if the Result contains a value
        bool isOk() const
        {
            return hasValue;
        }

        /// Checks if the Result contains an error
        bool isError() const
        {
            return !hasValue;
        }

        /// Bool conversion operator
        explicit operator bool() const
        {
            return hasValue;
        }

        /// Dereference operator (const)
        T const& operator*() const&
        {
            ARK_ASSERT(hasValue);
            return value;
        }

        /// Dereference operator
        T& operator*() &
        {
            ARK_ASSERT(hasValue);
            return value;
        }

        /// Dereference operator (const rvalue)
        T const&& operator*() const&&
        {
            ARK_ASSERT(hasValue);
            return Ark::move(value);
        }

        /// Dereference operator (rvalue)
        T&& operator*() &&
        {
            ARK_ASSERT(hasValue);
            return Ark::move(value);
        }

        /// Arrow operator (const)
        T const* operator->() const
        {
            ARK_ASSERT(hasValue);
            return &value;
        }

        /// Arrow operator
        T* operator->()
        {
            ARK_ASSERT(hasValue);
            return &value;
        }

        /// Gets the value (const lvalue)
        T const& getValue() const&
        {
            ARK_ASSERT(hasValue);
            return value;
        }

        /// Gets the value (lvalue)
        T& getValue() &
        {
            ARK_ASSERT(hasValue);
            return value;
        }

        /// Gets the value (const rvalue)
        T const&& getValue() const&&
        {
            ARK_ASSERT(hasValue);
            return Ark::move(value);
        }

        /// Gets the value (rvalue)
        T&& getValue() &&
        {
            ARK_ASSERT(hasValue);
            return Ark::move(value);
        }

        /// Gets the error (const lvalue)
        TError const& getError() const&
        {
            ARK_ASSERT(!hasValue);
            return error;
        }

        /// Gets the error (lvalue)
        TError& getError() &
        {
            ARK_ASSERT(!hasValue);
            return error;
        }

        /// Gets the error (const rvalue)
        TError const&& getError() const&&
        {
            ARK_ASSERT(!hasValue);
            return Ark::move(error);
        }

        /// Gets the error (rvalue)
        TError&& getError() &&
        {
            ARK_ASSERT(!hasValue);
            return Ark::move(error);
        }

        /// Gets the value or a default
        /// @tparam U Default value type
        /// @param defaultValue The default value
        template <typename U = T>
        T getValueOr(U&& defaultValue) const&
        {
            if (hasValue)
            {
                return value;
            }
            else
            {
                return static_cast<T>(Ark::forward<U>(defaultValue));
            }
        }

        /// Gets the value or a default (rvalue)
        /// @tparam U Default value type
        /// @param defaultValue The default value
        template <typename U = T>
        T getValueOr(U&& defaultValue) &&
        {
            if (hasValue)
            {
                return Ark::move(value);
            }
            else
            {
                return static_cast<T>(Ark::forward<U>(defaultValue));
            }
        }

        /// Gets the error or a default
        /// @tparam U Default error type
        /// @param defaultError The default error
        template <typename U = TError>
        TError errorOr(U&& defaultError) const&
        {
            if (hasValue)
            {
                return Ark::forward<U>(defaultError);
            }
            else
            {
                return error;
            }
        }

        /// Gets the error or a default (rvalue)
        /// @tparam U Default error type
        /// @param defaultError The default error
        template <typename U = TError>
        TError errorOr(U&& defaultError) &&
        {
            if (hasValue)
            {
                return Ark::forward<U>(defaultError);
            }
            else
            {
                return Ark::move(error);
            }
        }

        /// Maps the success value into a new Result (C++23 `transform`).
        template <typename Func>
        auto transform(Func&& func) & -> Result<decltype(func(Traits::declval<T&>())), TError>
        {
            using ReturnType = decltype(func(Traits::declval<T&>()));
            if (hasValue)
            {
                return Result<ReturnType, TError>(func(value));
            }
            return Result<ReturnType, TError>(unexpectedResult, error);
        }

        template <typename Func>
        auto transform(Func&& func) const& -> Result<decltype(func(Traits::declval<T const&>())), TError>
        {
            using ReturnType = decltype(func(Traits::declval<T const&>()));
            if (hasValue)
            {
                return Result<ReturnType, TError>(func(value));
            }
            return Result<ReturnType, TError>(unexpectedResult, error);
        }

        template <typename Func>
        auto transform(Func&& func) && -> Result<decltype(func(Traits::declval<T&&>())), TError>
        {
            using ReturnType = decltype(func(Traits::declval<T&&>()));
            if (hasValue)
            {
                return Result<ReturnType, TError>(func(Ark::move(value)));
            }
            return Result<ReturnType, TError>(unexpectedResult, Ark::move(error));
        }

        /// Maps the error value into a new Result (C++23 `transform_error`).
        template <typename Func>
        auto transformError(Func&& func) & -> Result<T, decltype(func(Traits::declval<TError&>()))>
        {
            using ErrorReturnType = decltype(func(Traits::declval<TError&>()));
            if (hasValue)
            {
                return Result<T, ErrorReturnType>(value);
            }
            return Result<T, ErrorReturnType>(unexpectedResult, func(error));
        }

        template <typename Func>
        auto transformError(Func&& func) const& -> Result<T, decltype(func(Traits::declval<TError const&>()))>
        {
            using ErrorReturnType = decltype(func(Traits::declval<TError const&>()));
            if (hasValue)
            {
                return Result<T, ErrorReturnType>(value);
            }
            return Result<T, ErrorReturnType>(unexpectedResult, func(error));
        }

        template <typename Func>
        auto transformError(Func&& func) && -> Result<T, decltype(func(Traits::declval<TError&&>()))>
        {
            using ErrorReturnType = decltype(func(Traits::declval<TError&&>()));
            if (hasValue)
            {
                return Result<T, ErrorReturnType>(Ark::move(value));
            }
            return Result<T, ErrorReturnType>(unexpectedResult, func(Ark::move(error)));
        }

        /// Chains a function returning a Result when this Result is ok (C++23 `and_then`).
        template <typename Func>
        auto andThen(Func&& func) & -> decltype(func(Traits::declval<T&>()))
        {
            using ReturnType = decltype(func(Traits::declval<T&>()));
            if (hasValue)
            {
                return func(value);
            }
            return ReturnType(unexpectedResult, error);
        }

        template <typename Func>
        auto andThen(Func&& func) const& -> decltype(func(Traits::declval<T const&>()))
        {
            using ReturnType = decltype(func(Traits::declval<T const&>()));
            if (hasValue)
            {
                return func(value);
            }
            return ReturnType(unexpectedResult, error);
        }

        template <typename Func>
        auto andThen(Func&& func) && -> decltype(func(Traits::declval<T&&>()))
        {
            using ReturnType = decltype(func(Traits::declval<T&&>()));
            if (hasValue)
            {
                return func(Ark::move(value));
            }
            return ReturnType(unexpectedResult, Ark::move(error));
        }

        /// Returns this Result if ok, otherwise the result of `func(error)` (C++23 `or_else`).
        template <typename Func>
        auto orElse(Func&& func) & -> decltype(func(Traits::declval<TError&>()))
        {
            using ReturnType = decltype(func(Traits::declval<TError&>()));
            if (hasValue)
            {
                return ReturnType(value);
            }
            return func(error);
        }

        template <typename Func>
        auto orElse(Func&& func) const& -> decltype(func(Traits::declval<TError const&>()))
        {
            using ReturnType = decltype(func(Traits::declval<TError const&>()));
            if (hasValue)
            {
                return ReturnType(value);
            }
            return func(error);
        }

        template <typename Func>
        auto orElse(Func&& func) && -> decltype(func(Traits::declval<TError&&>()))
        {
            using ReturnType = decltype(func(Traits::declval<TError&&>()));
            if (hasValue)
            {
                return ReturnType(Ark::move(value));
            }
            return func(Ark::move(error));
        }

        /// Alias for `transform` (historical name).
        template <typename Func>
        auto map(Func&& func) &
        {
            return transform(Ark::forward<Func>(func));
        }

        template <typename Func>
        auto map(Func&& func) const&
        {
            return transform(Ark::forward<Func>(func));
        }

        template <typename Func>
        auto map(Func&& func) &&
        {
            return Ark::move(*this).transform(Ark::forward<Func>(func));
        }

        /// Alias for `transformError` (historical name).
        template <typename Func>
        auto mapError(Func&& func) &
        {
            return transformError(Ark::forward<Func>(func));
        }

        template <typename Func>
        auto mapError(Func&& func) const&
        {
            return transformError(Ark::forward<Func>(func));
        }

        template <typename Func>
        auto mapError(Func&& func) &&
        {
            return Ark::move(*this).transformError(Ark::forward<Func>(func));
        }

        /// Equality comparison
        /// @tparam U Other value type
        /// @tparam UError Other error type
        /// @param other The other Result
        template <typename U, typename UError>
        bool operator==(Result<U, UError> const& other) const
        {
            if (hasValue != other.isOk())
            {
                return false;
            }
            else if (hasValue)
            {
                return value == other.getValue();
            }
            else
            {
                return error == other.getError();
            }
        }

        /// Inequality comparison
        /// @tparam U Other value type
        /// @tparam UError Other error type
        /// @param other The other Result
        template <typename U, typename UError>
        bool operator!=(Result<U, UError> const& other) const
        {
            return !(*this == other);
        }

        /// Equality comparison with value
        /// @tparam U Value type
        /// @param val The value
        template <typename U>
        bool operator==(U const& val) const
        {
            return hasValue && value == val;
        }

        /// Inequality comparison with value
        /// @tparam U Value type
        /// @param val The value
        template <typename U>
        bool operator!=(U const& val) const
        {
            return !(*this == val);
        }

        /// Equality comparison with unexpected
        /// @tparam UError Error type
        /// @param unexp The unexpected error
        template <typename UError>
        bool operator==(Error<UError> const& unexp) const
        {
            return !hasValue && error == unexp.getError();
        }

        /// Inequality comparison with unexpected
        /// @tparam UError Error type
        /// @param unexp The unexpected error
        template <typename UError>
        bool operator!=(Error<UError> const& unexp) const
        {
            return !(*this == unexp);
        }
    };

    template <typename TError>
    struct Result<Void, TError>
    {
        static_assert(Internal::CheckResultArgument<TError>::value);

        using ValueType = Void;
        using ErrorType = TError;
        using UnexpectedType = Error<TError>;

    private:
        union
        {
            TError error;
            char empty;
        };

        bool hasValue;

    public:
        Result()
            : empty{}
            , hasValue(true)
        {
        }

        Result(Result const& other)
            : empty{}
            , hasValue(other.hasValue)
        {
            if (!hasValue)
            {
                ::new (static_cast<void*>(&error)) TError(other.error);
            }
        }

        Result(Result&& other)
            : empty{}
            , hasValue(other.hasValue)
        {
            if (!hasValue)
            {
                ::new (static_cast<void*>(&error)) TError(Ark::move(other.error));
            }
        }

        template <typename UError>
        requires(!Concepts::SameAs<Traits::RemoveCVReferenceType<UError>, Result> &&
                 !Concepts::SameAs<Traits::RemoveCVReferenceType<UError>, UnexpectedResultTag> &&
                 !Concepts::SameAs<Traits::RemoveCVReferenceType<UError>, Error<TError>> &&
                 Concepts::ConstructibleFrom<TError, UError>)
        Result(UError&& errorValue)
            : error(Ark::forward<UError>(errorValue))
            , hasValue(false)
        {
        }

        template <typename UError>
        requires Concepts::ConstructibleFrom<TError, UError const&>
        Result(Error<UError> const& unexp)
            : error(unexp.getError())
            , hasValue(false)
        {
        }

        template <typename UError>
        requires Concepts::ConstructibleFrom<TError, UError&&>
        Result(Error<UError>&& unexp)
            : error(Ark::move(unexp.getError()))
            , hasValue(false)
        {
        }

        template <typename... Args>
        requires Concepts::ConstructibleFrom<TError, Args...>
        explicit Result(UnexpectedResultTag, Args&&... args)
            : error(Ark::forward<Args>(args)...)
            , hasValue(false)
        {
        }

        ~Result()
        {
            if (!hasValue)
            {
                error.~TError();
            }
        }

        Result& operator=(Result const& other)
        {
            if (this == &other)
            {
                return *this;
            }

            if (!hasValue && !other.hasValue)
            {
                error = other.error;
            }
            else if (!hasValue && other.hasValue)
            {
                error.~TError();
                hasValue = true;
            }
            else if (hasValue && !other.hasValue)
            {
                ::new (static_cast<void*>(&error)) TError(other.error);
                hasValue = false;
            }

            return *this;
        }

        Result& operator=(Result&& other)
        {
            if (this == &other)
            {
                return *this;
            }

            if (!hasValue && !other.hasValue)
            {
                error = Ark::move(other.error);
            }
            else if (!hasValue && other.hasValue)
            {
                error.~TError();
                hasValue = true;
            }
            else if (hasValue && !other.hasValue)
            {
                ::new (static_cast<void*>(&error)) TError(Ark::move(other.error));
                hasValue = false;
            }

            return *this;
        }

        template <typename UError>
        requires Concepts::ConstructibleFrom<TError, UError const&>
        Result& operator=(Error<UError> const& unexp)
        {
            if (hasValue)
            {
                ::new (static_cast<void*>(&error)) TError(unexp.getError());
                hasValue = false;
            }
            else
            {
                error = unexp.getError();
            }
            return *this;
        }

        template <typename UError>
        requires Concepts::ConstructibleFrom<TError, UError&&>
        Result& operator=(Error<UError>&& unexp)
        {
            if (hasValue)
            {
                ::new (static_cast<void*>(&error)) TError(Ark::move(unexp.getError()));
                hasValue = false;
            }
            else
            {
                error = Ark::move(unexp.getError());
            }
            return *this;
        }

        bool isOk() const
        {
            return hasValue;
        }

        bool isError() const
        {
            return !hasValue;
        }

        explicit operator bool() const
        {
            return hasValue;
        }

        TError const& getError() const&
        {
            ARK_ASSERT(!hasValue);
            return error;
        }

        TError& getError() &
        {
            ARK_ASSERT(!hasValue);
            return error;
        }

        TError const&& getError() const&&
        {
            ARK_ASSERT(!hasValue);
            return Ark::move(error);
        }

        TError&& getError() &&
        {
            ARK_ASSERT(!hasValue);
            return Ark::move(error);
        }
    };

    template <>
    struct Result<Void, Void>
    {
    private:
        bool hasValue;

    public:
        Result()
            : hasValue(true)
        {
        }

        Result(Result const& other)
            : hasValue(other.hasValue)
        {
        }

        Result(Result&& other)
            : hasValue(other.hasValue)
        {
        }

        explicit Result(UnexpectedResultTag)
            : hasValue(false)
        {
        }

        ~Result()
        {
        }

        bool isOk() const
        {
            return hasValue;
        }

        bool isError() const
        {
            return !hasValue;
        }

        explicit operator bool() const
        {
            return hasValue;
        }
    };

    /// Alias for Result with void value and error types
    using Outcome = Result<Void, Void>;

    /// Global swap function for Results
    /// @tparam T Value type
    /// @tparam TError Error type
    /// @param left Left Result
    /// @param right Right Result
    template <typename T, typename TError>
    void swap(Result<T, TError>& left, Result<T, TError>& right)
    {
        left.swap(right);
    }

    /// Global swap function for Error
    /// @tparam TError Error type
    /// @param left Left Error
    /// @param right Right Error
    template <typename TError>
    void swap(Error<TError>& left, Error<TError>& right)
    {
        left.swap(right);
    }

    /// Creates an unexpected error value.
    /// @tparam TError Error type
    /// @param error The error value
    template <typename TError>
    auto makeError(TError&& error) -> Error<Traits::RemoveCVReferenceType<TError>>
    {
        return Error<Traits::RemoveCVReferenceType<TError>>(Ark::forward<TError>(error));
    }

    /// Creates a successful Result.
    /// @tparam T Value type
    /// @param value The value
    template <typename T>
    auto makeResult(T&& value) -> Result<Traits::RemoveCVReferenceType<T>, Void>
    {
        return Result<Traits::RemoveCVReferenceType<T>, Void>(Ark::forward<T>(value));
    }

    /// Creates a successful Result with a void value.
    /// @tparam TError Error type
    template <typename TError = Void>
    inline auto makeResult() -> Result<Void, Traits::RemoveCVReferenceType<TError>>
    {
        return Result<Void, Traits::RemoveCVReferenceType<TError>>();
    }

    /// Creates an error Result with a void value.
    /// @tparam T Value type retained as Void
    /// @tparam TError Error type
    /// @param error The error value
    template <typename TError>
    auto makeErrorResult(TError&& error) -> Result<Void, Traits::RemoveCVReferenceType<TError>>
    {
        return Result<Void, Traits::RemoveCVReferenceType<TError>>(unexpectedResult, Ark::forward<TError>(error));
    }

    inline auto makeOutcome() -> Outcome
    {
        return Outcome();
    }

    inline auto makeError() -> Outcome
    {
        return Outcome(unexpectedResult);
    }
}
