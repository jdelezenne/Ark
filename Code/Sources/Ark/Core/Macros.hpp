#pragma once

#define ARK_UNUSED(x) static_cast<void>(x)
#define ARK_UNUSED_PARAM(x) ARK_UNUSED(x)

#define ARK_ANONYMOUS_VARIABLE(str) ARK_CONCAT(str, __LINE__)

#define ARK_CONCAT_IMPL(s1, s2) s1##s2
#define ARK_CONCAT(s1, s2) ARK_CONCAT_IMPL(s1, s2)

// Macros to count the number of arguments
#define ARK_EXPAND(x) x
#define ARK_COUNT_ARGS_IMPL(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, count, ...) count
#define ARK_COUNT_ARGS(...) ARK_EXPAND(ARK_COUNT_ARGS_IMPL(__VA_ARGS__, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0))

/// Define a macro to allow the copy constructor and operator= functions
#define ARK_COPYABLE(Type)       \
    Type(Type const&) = default; \
    Type& operator=(Type const&) = default;

/// Define a macro to allow the move constructor and operator= functions
#define ARK_MOVABLE(Type)   \
    Type(Type&&) = default; \
    Type& operator=(Type&&) = default;

/// Define a macro to disallow the copy constructor and operator= functions
#define ARK_NOT_COPYABLE(Type)  \
    Type(Type const&) = delete; \
    Type& operator=(Type const&) = delete;

/// Define a macro to disallow the move constructor and operator= functions
#define ARK_NOT_MOVABLE(Type) \
    Type(Type&&) = delete;    \
    Type& operator=(Type&&) = delete;

/// Define a macro to allow the copy and move constructor and operator= functions
#define ARK_STRUCT(Type) \
    using Self = Type;   \
    ARK_COPYABLE(Type)   \
    ARK_MOVABLE(Type)

/// Define a macro to allow the copy and move constructor and operator= functions; and define a default constructor and destructor
#define ARK_STRUCT_DEFAULT(Type) \
    ARK_STRUCT(Type)             \
    constexpr Type() = default;  \
    ~Type() = default;

/// Define a macro to disallow the default constructor
#define ARK_INTERFACE(Type) \
    Type() = delete;

/// Define a macro to disallow the copy and move constructor and operator= functions
#define ARK_CLASS(Type)    \
    using Self = Type;     \
    ARK_NOT_COPYABLE(Type) \
    ARK_MOVABLE(Type)

/// Define a macro to disallow the default constructor, copy and move constructor, and operator= functions
#define ARK_STATIC_STRUCT(Type) \
    ARK_CLASS(Type)             \
    ARK_INTERFACE(Type)

/// Define a macro to disallow the default constructor, copy and move constructor, and operator= functions
#define ARK_STATIC_CLASS(Type) \
    ARK_CLASS(Type)            \
    ARK_INTERFACE(Type)

#define ARK_RETURN(condition) \
    if (!(condition))         \
    {                         \
        return;               \
    }

#define ARK_RETURN_VALUE(condition, value) \
    if (!(condition))                      \
    {                                      \
        return value;                      \
    }

/// Guard macro that sets errorString and returns false if condition is false
#define ARK_GUARD(condition, error_msg) \
    if (!(condition))                   \
    {                                   \
        errorString = error_msg;        \
        return false;                   \
    }

/// Guard macro that sets errorString and returns a custom value if condition is false
#define ARK_GUARD_RETURN(condition, error_msg, return_value) \
    if (!(condition))                                        \
    {                                                        \
        errorString = error_msg;                             \
        return return_value;                                 \
    }

/// Guard macro with custom error variable that sets the error and returns false if condition is false
#define ARK_GUARD_WITH(condition, error_var, error_msg) \
    if (!(condition))                                   \
    {                                                   \
        error_var = error_msg;                          \
        return false;                                   \
    }

/// Guard macro with custom error variable that sets the error and returns a custom value if condition is false
#define ARK_GUARD_WITH_RETURN(condition, error_var, error_msg, return_value) \
    if (!(condition))                                                        \
    {                                                                        \
        error_var = error_msg;                                               \
        return return_value;                                                 \
    }

#define ARK_BIT(value) \
    (1u << (value))
