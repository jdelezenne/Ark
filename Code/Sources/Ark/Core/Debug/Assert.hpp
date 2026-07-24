#pragma once

#include "Ark/Core/Types.hpp"

#include <cstdlib>
#include <type_traits>

#if defined(ARK_COMPILER_MSVC)

#include <intrin.h>

#define ARK_DEBUG_BREAK() \
    __debugbreak()

#else

#include <csignal>

#define ARK_DEBUG_BREAK() \
    std::raise(SIGINT)

#endif

#define ARK_ABORT() \
    std::abort()

namespace Ark
{
    /// @name Assert Action
    /// Defines the possible actions to take when an assertion fails.
    enum class AssertAction
    {
        Abort,    ///< Abort the program.
        Break,    ///< Break into the debugger.
        Continue, ///< Continue the program.
    };

    /// @typedef Assert Handler
    /// Defines a function callback for custom assertion handlers.
    /// @param expression The expression that failed the assertion.
    /// @param message The message associated with the assertion.
    /// @param fileName The name of the file where the assertion failed.
    /// @param lineNumber The line number where the assertion failed.
    /// @return The action to take when the assertion fails.
    using AssertHandler = AssertAction (*)(char const* expression, char const* message, char const* fileName, uint lineNumber);

    /// Gets the custom assertion handler.
    AssertHandler getAssertHandler();

    /// Sets the custom assertion handler.
    void setAssertHandler(AssertHandler value);

    AssertAction defaultAssertHandler(const char* expression, const char* message, const char* fileName, uint lineNumber);

    [[noreturn]]
    void fatalError(const char* message, const char* fileName, uint lineNumber);

    /// Handles an assertion based on the given condition.
    /// @param condition The condition to check.
    /// @param expression The expression that failed the assertion.
    /// @param message The message associated with the assertion.
    /// @param fileName The name of the file where the assertion failed.
    /// @param lineNumber The line number where the assertion failed.
    /// @return The action taken for the assertion.
    AssertAction handleAssert(bool condition, char const* expression, char const* message, char const* fileName, uint lineNumber);

    constexpr void onAssert(bool condition, char const* expression, char const* message, char const* fileName, uint lineNumber)
    {
        if (!condition)
        {
            AssertAction action = AssertAction::Abort;

            if (!std::is_constant_evaluated())
            {
                action = handleAssert(condition, expression, message, fileName, lineNumber);
            }

            switch (action)
            {
                case AssertAction::Abort:
                    ARK_ABORT();
                    break;
                case AssertAction::Break:
                    ARK_DEBUG_BREAK();
                    break;
                case AssertAction::Continue:
                    // Do nothing, continue execution
                    break;
                default:
                    ARK_ABORT();
                    break;
            }
        }
    }
}

// Conditional assert, always active.
#define ARK_ASSERT(condition)                                                \
    do                                                                       \
    {                                                                        \
        Ark::onAssert((condition), #condition, nullptr, __FILE__, __LINE__); \
    }                                                                        \
    while (0);

#define ARK_ASSERT_MSG(condition, message)                                   \
    do                                                                       \
    {                                                                        \
        Ark::onAssert((condition), #condition, message, __FILE__, __LINE__); \
    }                                                                        \
    while (0);

// Conditional assert, debug only.
#ifndef NDEBUG

#define ARK_DEBUG_ASSERT(condition)                                          \
    do                                                                       \
    {                                                                        \
        Ark::onAssert((condition), #condition, nullptr, __FILE__, __LINE__); \
    }                                                                        \
    while (0);

#define ARK_DEBUG_ASSERT_MSG(condition, message)                             \
    do                                                                       \
    {                                                                        \
        Ark::onAssert((condition), #condition, message, __FILE__, __LINE__); \
    }                                                                        \
    while (0);

#else

#define ARK_DEBUG_ASSERT(condition) ((void)0)

#define ARK_DEBUG_ASSERT_MSG(condition, message) ((void)0)

#endif

// Unconditional assert, always active.
#define ARK_ASSERT_FAIL()                                                \
    do                                                                   \
    {                                                                    \
        Ark::onAssert(false, "ARK assert", nullptr, __FILE__, __LINE__); \
    }                                                                    \
    while (0);

#define ARK_ASSERT_FAIL_MSG(message)                                     \
    do                                                                   \
    {                                                                    \
        Ark::onAssert(false, "ARK assert", message, __FILE__, __LINE__); \
    }                                                                    \
    while (0);

// Unconditional assert, debug only.
#ifndef NDEBUG

#define ARK_DEBUG_ASSERT_FAIL()                                          \
    do                                                                   \
    {                                                                    \
        Ark::onAssert(false, "ARK assert", nullptr, __FILE__, __LINE__); \
    }                                                                    \
    while (0);

#define ARK_DEBUG_ASSERT_FAIL_MSG(message)                               \
    do                                                                   \
    {                                                                    \
        Ark::onAssert(false, "ARK assert", message, __FILE__, __LINE__); \
    }                                                                    \
    while (0);

#else

#define ARK_DEBUG_ASSERT_FAIL() ((void)0)

#define ARK_DEBUG_ASSERT_FAIL_MSG(message) ((void)0)

#endif

// Fatal error, always active.
#define ARK_FATAL_ERROR()                             \
    do                                                \
    {                                                 \
        Ark::fatalError(nullptr, __FILE__, __LINE__); \
    }                                                 \
    while (0);

#define ARK_FATAL_ERROR_MSG(message)                  \
    do                                                \
    {                                                 \
        Ark::fatalError(message, __FILE__, __LINE__); \
    }                                                 \
    while (0);

// Unreachable code marker, always active.
#if defined(ARK_COMPILER_MSVC)

#define ARK_UNREACHABLE()                                        \
    do                                                           \
    {                                                            \
        Ark::fatalError("Unreachable code", __FILE__, __LINE__); \
        __assume(0);                                             \
    }                                                            \
    while (0);

#elif defined(__GNUC__) || defined(__clang__)

#define ARK_UNREACHABLE()                                        \
    do                                                           \
    {                                                            \
        Ark::fatalError("Unreachable code", __FILE__, __LINE__); \
        __builtin_unreachable();                                 \
    }                                                            \
    while (0);

#else

#define ARK_UNREACHABLE()                                        \
    do                                                           \
    {                                                            \
        Ark::fatalError("Unreachable code", __FILE__, __LINE__); \
    }                                                            \
    while (0);

#endif

// Conditional assert helpers that return from the current function on failure.
// `ARK_ASSERT_PRE` / `ARK_ASSERT_PRE_ERROR` require `Ark/Core/Result.hpp` for `makeError()`.
#define ARK_ASSERT_PRE(condition)    \
    if (!(condition))                \
    {                                \
        ARK_DEBUG_ASSERT(condition); \
        return ::Ark::makeError();   \
    }

#define ARK_ASSERT_PRE_RETURN(condition) \
    if (!(condition))                    \
    {                                    \
        ARK_DEBUG_ASSERT(condition);     \
        return;                          \
    }

#define ARK_ASSERT_PRE_RETURN_MSG(condition, message) \
    if (!(condition))                                 \
    {                                                 \
        ARK_DEBUG_ASSERT_MSG(condition, message);     \
        return;                                       \
    }

#define ARK_ASSERT_PRE_RETURN_VALUE(condition, value) \
    if (!(condition))                                 \
    {                                                 \
        ARK_DEBUG_ASSERT(condition);                  \
        return value;                                 \
    }

#define ARK_ASSERT_PRE_RETURN_VALUE_MSG(condition, value, message) \
    if (!(condition))                                              \
    {                                                              \
        ARK_DEBUG_ASSERT_MSG(condition, message);                  \
        return value;                                              \
    }

#define ARK_ASSERT_PRE_ERROR(condition, error_expression) \
    if (!(condition))                                     \
    {                                                     \
        ARK_DEBUG_ASSERT(condition);                      \
        return (error_expression);                        \
    }
