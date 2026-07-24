#pragma once

#include "Ark/Core/Comparison.hpp"
#include "Ark/Core/Debug/Assert.hpp"
#include "Ark/Core/Option.hpp"
#include "Ark/Core/Types.hpp"
#include "Ark/Strings/Strings.hpp"

#if (ARK_USE_STANDARD_LIBRARY == ARK_STANDARD_LIBRARY_CPP) || (ARK_USE_STANDARD_LIBRARY == ARK_STANDARD_LIBRARY_C)
#define ARK_CONSTEXPR
#else
#define ARK_CONSTEXPR constexpr
#endif

namespace Ark::AnsiStrings
{
    static constexpr AnsiChar NullCharacter = '\0';
    static constexpr AnsiChar const* NullString = "";

    /// Get the length of a string.
    /// @param str The string.
    /// @return The length of the string.
    /// @note The function does not check for buffer overflows.
    ARK_CONSTEXPR usize getLengthUnsafe(AnsiChar const* str);

    /// Get the length of a string.
    /// @param str The string.
    /// @param maxLength The maximum number of characters to count.
    /// @return The length of the string.
    /// @note The function stops counting at the null character or when maxLength is reached.
    ARK_CONSTEXPR usize getLength(AnsiChar const* str, usize maxLength);

    /// Copy a string from source to destination.
    /// @param  source The source string.
    /// @param  destination The destination string.
    /// @note The destination string must be large enough to hold the source string.
    ///       The function does not check for buffer overflows.
    ///       The function does not append a null character to the destination string.
    ARK_CONSTEXPR void copyUnsafe(AnsiChar const* source, AnsiChar* destination);

    /// Copy a string from source to destination.
    /// @param source The source string.
    /// @param destination The destination string.
    /// @param maxLength The maximum number of characters to copy.
    /// @return `true` if the string was copied successfully; otherwise, `false`.
    ARK_CONSTEXPR bool copy(AnsiChar const* source, AnsiChar* destination, usize maxLength);

    /// Copy a string from source to destination.
    /// @param  source The source string.
    /// @param  destination The destination string.
    /// @param  count The maximum number of characters to copy.
    /// @note The function copies the initial `count` characters.
    ///       If `count` is less than or equal to the length of `source`,
    ///         a null character isn't appended automatically to the copied string.
    ///       If `count` is greater than the length of `source`,
    ///         the destination string is padded with null characters up to length count.
    ARK_CONSTEXPR void copyUnsafe(AnsiChar const* source, AnsiChar* destination, usize count);

    /// Copy a string from source to destination.
    /// @param source The source string.
    /// @param destination The destination string.
    /// @param maxLength The maximum number of characters to copy.
    /// @param count The number of characters to copy.
    /// @return `true` if the string was copied successfully; otherwise, `false`.
    ARK_CONSTEXPR bool copy(AnsiChar const* source, AnsiChar* destination, usize maxLength, usize count);

    ARK_CONSTEXPR void appendUnsafe(AnsiChar const* source, AnsiChar* destination);

    ARK_CONSTEXPR bool append(AnsiChar const* source, AnsiChar* destination, usize maxLength);

    ARK_CONSTEXPR void appendUnsafe(AnsiChar const* source, AnsiChar* destination, usize count);

    ARK_CONSTEXPR bool append(AnsiChar const* source, AnsiChar* destination, usize maxLength, usize count);

    ARK_CONSTEXPR AnsiChar const* findFirstCharacterUnsafe(AnsiChar const* str, AnsiChar ch);
    Option<AnsiChar const*> findFirstCharacter(AnsiChar const* str, AnsiChar ch, usize maxLength);

    ARK_CONSTEXPR AnsiChar const* findLastCharacterUnsafe(AnsiChar const* str, AnsiChar ch);
    Option<AnsiChar const*> findLastCharacter(AnsiChar const* str, AnsiChar ch, usize maxLength);

    ARK_CONSTEXPR AnsiChar const* findFirstUnsafe(AnsiChar const* str, AnsiChar const* substring);
    ARK_CONSTEXPR AnsiChar const* findFirstUnsafe(AnsiChar const* str, AnsiChar const* substring, usize count);
    Option<AnsiChar const*> findFirst(AnsiChar const* str, AnsiChar const* substring, usize maxLength);

    ARK_CONSTEXPR AnsiChar const* findLastUnsafe(AnsiChar const* str, AnsiChar const* substring);
    ARK_CONSTEXPR AnsiChar const* findLastUnsafe(AnsiChar const* str, AnsiChar const* substring, usize count);
    Option<AnsiChar const*> findLast(AnsiChar const* str, AnsiChar const* substring, usize maxLength);

    ARK_CONSTEXPR Ordering compareUnsafe(AnsiChar const* lhs, AnsiChar const* rhs);
    Ordering compare(AnsiChar const* lhs, AnsiChar const* rhs, usize maxLength);
    ARK_CONSTEXPR Ordering compareUnsafe(AnsiChar const* lhs, AnsiChar const* rhs, usize count);
    Ordering compare(AnsiChar const* lhs, AnsiChar const* rhs, usize maxLength, usize count);

    ARK_CONSTEXPR Ordering compareCaseInsensitiveUnsafe(AnsiChar const* lhs, AnsiChar const* rhs);
    Ordering compareCaseInsensitive(AnsiChar const* lhs, AnsiChar const* rhs, usize maxLength);
    ARK_CONSTEXPR Ordering compareCaseInsensitiveUnsafe(AnsiChar const* lhs, AnsiChar const* rhs, usize count);
    Ordering compareCaseInsensitive(AnsiChar const* lhs, AnsiChar const* rhs, usize maxLength, usize count);

    /// Check if a string is empty.
    /// @param str The string.
    /// @return `true` if the string is empty; otherwise, `false`.
    ARK_CONSTEXPR bool isWhitespace(AnsiChar const* str);
}

#if (ARK_USE_STANDARD_LIBRARY == ARK_STANDARD_LIBRARY_CPP)
#include "Ark/Strings/Platform/CppStd/AnsiStrings.hpp"
#elif (ARK_USE_STANDARD_LIBRARY == ARK_STANDARD_LIBRARY_C)
#include "Ark/Strings/Platform/CStd/AnsiStrings.hpp"
#else
#include "Ark/Strings/Platform/Generic/AnsiStrings.hpp"
#endif

#undef ARK_CONSTEXPR
