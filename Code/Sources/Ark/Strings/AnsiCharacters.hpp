#pragma once

#include "Ark/Core/Debug/Assert.hpp"
#include "Ark/Core/Types.hpp"
#include "Ark/Strings/Strings.hpp"

#if (ARK_USE_STANDARD_LIBRARY == ARK_STANDARD_LIBRARY_CPP) || (ARK_USE_STANDARD_LIBRARY == ARK_STANDARD_LIBRARY_C)
#define ARK_CONSTEXPR
#else
#define ARK_CONSTEXPR constexpr
#endif

namespace Ark::Characters
{
    constexpr usize AsciiMin = 0;
    constexpr usize AsciiMax = 127;

    /// Check if the character is alphabetic.
    ARK_CONSTEXPR bool isAlphabetic(AnsiChar ch);

    /// Check if the character is numeric.
    ARK_CONSTEXPR bool isNumeric(AnsiChar ch);

    /// Check if the character is alphanumeric.
    ARK_CONSTEXPR bool isAlphanumeric(AnsiChar ch);

    /// Check if the character is ASCII.
    ARK_CONSTEXPR bool isAscii(AnsiChar ch);

    /// Check if the character is a control character.
    ARK_CONSTEXPR bool isControl(AnsiChar ch);

    /// Check if the character is a digit.
    ARK_CONSTEXPR bool isDigit(AnsiChar ch);

    /// Check if the character is a hexadecimal digit.
    ARK_CONSTEXPR bool isHexDigit(AnsiChar ch);

    /// Check if the character is a punctuation character.
    ARK_CONSTEXPR bool isPunctuation(AnsiChar ch);

    /// Check if the character is a graphical character.
    ARK_CONSTEXPR bool isGraphic(AnsiChar ch);

    /// Check if the character is a whitespace character.
    ARK_CONSTEXPR bool isWhitespace(AnsiChar ch);

    /// Check if the character is uppercase.
    ARK_CONSTEXPR bool isUppercase(AnsiChar ch);

    /// Check if the character is lowercase.
    ARK_CONSTEXPR bool isLowercase(AnsiChar ch);

    /// Check if the character is an uppercase hexadecimal digit.
    ARK_CONSTEXPR bool isHexUppercase(AnsiChar ch);

    /// Check if the character is a lowercase hexadecimal digit.
    ARK_CONSTEXPR bool isHexLowercase(AnsiChar ch);

    /// Convert the character to a digit.
    /// @param ch The character to convert.
    /// @param radix The radix (base) for the conversion (default is 10).
    /// @return The digit value, or -1 if conversion is not possible.
    ARK_CONSTEXPR int toDigit(AnsiChar ch, int radix = 10);

    /// Convert the character to uppercase.
    ARK_CONSTEXPR AnsiChar toUppercase(AnsiChar ch);

    /// Convert the character to lowercase.
    ARK_CONSTEXPR AnsiChar toLowercase(AnsiChar ch);

    /// Converts a digit value to its character representation.
    constexpr AnsiChar digitToChar(int digit, int radix = 10, Casing casing = Casing::Uppercase)
    {
        ARK_ASSERT_MSG(digit >= 0 && digit < radix, "Digit must be in the range 0 to radix-1");
        ARK_ASSERT_MSG(radix >= 2 && radix <= 36, "Radix must be in the range 2-36");

        if (digit < 10)
        {
            return static_cast<AnsiChar>('0' + digit);
        }
        else
        {
            return static_cast<AnsiChar>((casing == Casing::Uppercase ? 'A' : 'a') + (digit - 10));
        }
    }
}

#if (ARK_USE_STANDARD_LIBRARY == ARK_STANDARD_LIBRARY_CPP)
#include "Ark/Strings/Platform/CppStd/AnsiCharacters.hpp"
#elif (ARK_USE_STANDARD_LIBRARY == ARK_STANDARD_LIBRARY_C)
#include "Ark/Strings/Platform/CStd/AnsiCharacters.hpp"
#else
#include "Ark/Strings/Platform/Generic/AnsiCharacters.hpp"
#endif

#undef ARK_CONSTEXPR
