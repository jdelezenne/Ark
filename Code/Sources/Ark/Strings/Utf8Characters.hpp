#pragma once

#include "Ark/Core/Debug/Assert.hpp"
#include "Ark/Core/Types.hpp"
#include "Ark/Strings/Strings.hpp"

#if (ARK_USE_STANDARD_LIBRARY == ARK_STANDARD_LIBRARY_CPP) || (ARK_USE_STANDARD_LIBRARY == ARK_STANDARD_LIBRARY_C)
#define ARK_CONSTEXPR
#else
#define ARK_CONSTEXPR constexpr
#endif

namespace Ark::Utf8Characters
{
    constexpr UnicodeChar UnicodeMin = 0;
    constexpr UnicodeChar UnicodeMax = 0x10FFFF;

    /// Check if the codepoint is alphabetic.
    ARK_CONSTEXPR bool isAlphabetic(UnicodeChar codepoint);

    /// Check if the codepoint is numeric.
    ARK_CONSTEXPR bool isNumeric(UnicodeChar codepoint);

    /// Check if the codepoint is alphanumeric.
    ARK_CONSTEXPR bool isAlphanumeric(UnicodeChar codepoint);

    /// Check if the codepoint is ASCII (U+0000 to U+007F).
    ARK_CONSTEXPR bool isAscii(UnicodeChar codepoint);

    /// Check if the codepoint is a control character.
    ARK_CONSTEXPR bool isControl(UnicodeChar codepoint);

    /// Check if the codepoint is a digit.
    ARK_CONSTEXPR bool isDigit(UnicodeChar codepoint);

    /// Check if the codepoint is a hexadecimal digit.
    ARK_CONSTEXPR bool isHexDigit(UnicodeChar codepoint);

    /// Check if the codepoint is a punctuation character.
    ARK_CONSTEXPR bool isPunctuation(UnicodeChar codepoint);

    /// Check if the codepoint is a graphical character.
    ARK_CONSTEXPR bool isGraphic(UnicodeChar codepoint);

    /// Check if the codepoint is a whitespace character.
    ARK_CONSTEXPR bool isWhitespace(UnicodeChar codepoint);

    /// Check if the codepoint is uppercase.
    ARK_CONSTEXPR bool isUppercase(UnicodeChar codepoint);

    /// Check if the codepoint is lowercase.
    ARK_CONSTEXPR bool isLowercase(UnicodeChar codepoint);

    /// Check if the codepoint is an uppercase hexadecimal digit.
    ARK_CONSTEXPR bool isHexUppercase(UnicodeChar codepoint);

    /// Check if the codepoint is a lowercase hexadecimal digit.
    ARK_CONSTEXPR bool isHexLowercase(UnicodeChar codepoint);

    /// Convert the codepoint to a digit.
    /// @param codepoint The codepoint to convert.
    /// @param radix The radix (base) for the conversion (default is 10).
    /// @return The digit value, or -1 if conversion is not possible.
    ARK_CONSTEXPR int toDigit(UnicodeChar codepoint, int radix = 10);

    /// Convert the codepoint to uppercase.
    ARK_CONSTEXPR UnicodeChar toUppercase(UnicodeChar codepoint);

    /// Convert the codepoint to lowercase.
    ARK_CONSTEXPR UnicodeChar toLowercase(UnicodeChar codepoint);

    /// Convert a digit to a Unicode codepoint.
    /// @param digit The digit value (0-35).
    /// @param radix The radix (base) for the conversion (default is 10).
    /// @param casing The casing for hexadecimal digits (default is Uppercase).
    /// @return The Unicode codepoint representing the digit.
    constexpr UnicodeChar digitToCodepoint(int digit, int radix = 10, Casing casing = Casing::Uppercase)
    {
        ARK_ASSERT_MSG(digit >= 0 && digit < radix, "Digit must be in the range 0 to radix-1");
        ARK_ASSERT_MSG(radix >= 2 && radix <= 36, "Radix must be in the range 2-36");

        if (digit < 10)
        {
            return static_cast<UnicodeChar>('0' + digit);
        }
        else
        {
            return static_cast<UnicodeChar>((casing == Casing::Uppercase ? 'A' : 'a') + (digit - 10));
        }
    }
}

#include "Ark/Strings/Platform/Generic/Utf8Characters.hpp"

#undef ARK_CONSTEXPR
