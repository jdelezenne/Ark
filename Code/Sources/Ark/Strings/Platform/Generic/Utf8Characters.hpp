#pragma once

#include "Ark/Core/Types.hpp"
#include "Ark/Strings/AnsiCharacters.hpp"

namespace Ark::Utf8Characters
{
    namespace
    {
        // Helper functions for Unicode codepoint range checks
        constexpr bool isInRange(UnicodeChar codepoint, UnicodeChar min, UnicodeChar max)
        {
            return codepoint >= min && codepoint <= max;
        }

        constexpr bool isLatin1SupplementUppercase(UnicodeChar codepoint)
        {
            return isInRange(codepoint, 0x00C0, 0x00DE) && codepoint != 0x00D7 && codepoint != 0x00F7;
        }

        constexpr bool isLatin1SupplementLowercase(UnicodeChar codepoint)
        {
            return isInRange(codepoint, 0x00E0, 0x00FF) && codepoint != 0x00F7;
        }

        constexpr bool isGreekUppercase(UnicodeChar codepoint)
        {
            return isInRange(codepoint, 0x0391, 0x03A1) || isInRange(codepoint, 0x03A3, 0x03AB);
        }

        constexpr bool isGreekLowercase(UnicodeChar codepoint)
        {
            return isInRange(codepoint, 0x03B1, 0x03C9) || isInRange(codepoint, 0x03CB, 0x03CE);
        }

        constexpr bool isCyrillicUppercase(UnicodeChar codepoint)
        {
            return isInRange(codepoint, 0x0400, 0x042F);
        }

        constexpr bool isCyrillicLowercase(UnicodeChar codepoint)
        {
            return isInRange(codepoint, 0x0430, 0x044F);
        }

        constexpr bool isLatinExtended(UnicodeChar codepoint)
        {
            return isInRange(codepoint, 0x00C0, 0x024F);
        }

        constexpr bool isGreekAndCoptic(UnicodeChar codepoint)
        {
            if (!isInRange(codepoint, 0x0370, 0x03FF))
            {
                return false;
            }
            return isInRange(codepoint, 0x0370, 0x0373) ||
                   isInRange(codepoint, 0x0376, 0x0377) ||
                   isInRange(codepoint, 0x037A, 0x037D) ||
                   isInRange(codepoint, 0x037F, 0x0383) ||
                   isInRange(codepoint, 0x0386, 0x038A) ||
                   codepoint == 0x038C ||
                   isInRange(codepoint, 0x038E, 0x03A1) ||
                   isInRange(codepoint, 0x03A3, 0x03FF);
        }

        constexpr bool isCyrillic(UnicodeChar codepoint)
        {
            if (!isInRange(codepoint, 0x0400, 0x04FF))
            {
                return false;
            }
            return codepoint != 0x0482 && codepoint != 0x0483 && codepoint != 0x0484 &&
                   codepoint != 0x0485 && codepoint != 0x0486 && codepoint != 0x0487 &&
                   codepoint != 0x0488 && codepoint != 0x0489;
        }

        constexpr bool isC1Control(UnicodeChar codepoint)
        {
            return isInRange(codepoint, 0x0080, 0x009F);
        }
    }

    constexpr bool isAscii(UnicodeChar codepoint)
    {
        return codepoint <= 0x7F;
    }

    constexpr bool isAlphabetic(UnicodeChar codepoint)
    {
        // Delegate ASCII range to Characters namespace
        if (isAscii(codepoint))
        {
            return Characters::isAlphabetic(static_cast<AnsiChar>(codepoint));
        }

        // Latin-1 Supplement and Extended ranges (U+00C0 to U+024F)
        if (isLatinExtended(codepoint))
        {
            // Latin Extended-A and Latin Extended-B ranges contain mostly alphabetic characters
            return true;
        }

        // Greek and Coptic (U+0370 to U+03FF)
        if (isGreekAndCoptic(codepoint))
        {
            return true;
        }

        // Cyrillic (U+0400 to U+04FF)
        if (isCyrillic(codepoint))
        {
            return true;
        }

        // For now, return false for other ranges
        // Full Unicode support would require Unicode data tables
        return false;
    }

    constexpr bool isNumeric(UnicodeChar codepoint)
    {
        // Delegate ASCII range to Characters namespace
        if (isAscii(codepoint))
        {
            return Characters::isNumeric(static_cast<AnsiChar>(codepoint));
        }

        // Unicode has many numeric codepoints (e.g., Roman numerals, fractions, etc.)
        // For now, only handle ASCII digits
        // Full Unicode support would require Unicode data tables
        return false;
    }

    constexpr bool isAlphanumeric(UnicodeChar codepoint)
    {
        return isAlphabetic(codepoint) || isNumeric(codepoint);
    }

    constexpr bool isControl(UnicodeChar codepoint)
    {
        // Delegate ASCII range to Characters namespace
        if (isAscii(codepoint))
        {
            return Characters::isControl(static_cast<AnsiChar>(codepoint));
        }

        // C1 Controls (U+0080 to U+009F)
        return isC1Control(codepoint);
    }

    constexpr bool isDigit(UnicodeChar codepoint)
    {
        // Delegate ASCII range to Characters namespace
        if (isAscii(codepoint))
        {
            return Characters::isDigit(static_cast<AnsiChar>(codepoint));
        }

        return false;
    }

    constexpr bool isHexDigit(UnicodeChar codepoint)
    {
        // Delegate ASCII range to Characters namespace
        if (isAscii(codepoint))
        {
            return Characters::isHexDigit(static_cast<AnsiChar>(codepoint));
        }

        return false;
    }

    constexpr bool isPunctuation(UnicodeChar codepoint)
    {
        // Delegate ASCII range to Characters namespace
        if (isAscii(codepoint))
        {
            return Characters::isPunctuation(static_cast<AnsiChar>(codepoint));
        }

        // Unicode has many punctuation ranges
        // For now, only handle ASCII punctuation
        // Full Unicode support would require Unicode data tables
        return false;
    }

    constexpr bool isGraphic(UnicodeChar codepoint)
    {
        // Delegate ASCII range to Characters namespace
        if (isAscii(codepoint))
        {
            return Characters::isGraphic(static_cast<AnsiChar>(codepoint));
        }

        // Most non-control, non-whitespace Unicode characters are graphic
        return !isControl(codepoint) && !isWhitespace(codepoint);
    }

    constexpr bool isWhitespace(UnicodeChar codepoint)
    {
        // Delegate ASCII range to Characters namespace
        if (isAscii(codepoint))
        {
            return Characters::isWhitespace(static_cast<AnsiChar>(codepoint));
        }

        // Common Unicode whitespace characters
        return codepoint == 0x00A0 || // Non-breaking space
               codepoint == 0x1680 || // Ogham space mark
               codepoint == 0x2000 || // En quad
               codepoint == 0x2001 || // Em quad
               codepoint == 0x2002 || // En space
               codepoint == 0x2003 || // Em space
               codepoint == 0x2004 || // Three-per-em space
               codepoint == 0x2005 || // Four-per-em space
               codepoint == 0x2006 || // Six-per-em space
               codepoint == 0x2007 || // Figure space
               codepoint == 0x2008 || // Punctuation space
               codepoint == 0x2009 || // Thin space
               codepoint == 0x200A || // Hair space
               codepoint == 0x2028 || // Line separator
               codepoint == 0x2029 || // Paragraph separator
               codepoint == 0x202F || // Narrow no-break space
               codepoint == 0x205F || // Medium mathematical space
               codepoint == 0x3000;   // Ideographic space
    }

    constexpr bool isUppercase(UnicodeChar codepoint)
    {
        // Delegate ASCII range to Characters namespace
        if (isAscii(codepoint))
        {
            return Characters::isUppercase(static_cast<AnsiChar>(codepoint));
        }

        return isLatin1SupplementUppercase(codepoint) ||
               isGreekUppercase(codepoint) ||
               isCyrillicUppercase(codepoint);
    }

    constexpr bool isLowercase(UnicodeChar codepoint)
    {
        // Delegate ASCII range to Characters namespace
        if (isAscii(codepoint))
        {
            return Characters::isLowercase(static_cast<AnsiChar>(codepoint));
        }

        return isLatin1SupplementLowercase(codepoint) ||
               isGreekLowercase(codepoint) ||
               isCyrillicLowercase(codepoint);
    }

    constexpr bool isHexUppercase(UnicodeChar codepoint)
    {
        // Delegate ASCII range to Characters namespace
        if (isAscii(codepoint))
        {
            return Characters::isHexUppercase(static_cast<AnsiChar>(codepoint));
        }

        return false;
    }

    constexpr bool isHexLowercase(UnicodeChar codepoint)
    {
        // Delegate ASCII range to Characters namespace
        if (isAscii(codepoint))
        {
            return Characters::isHexLowercase(static_cast<AnsiChar>(codepoint));
        }

        return false;
    }

    constexpr int toDigit(UnicodeChar codepoint, int radix)
    {
        // Delegate ASCII range to Characters namespace
        if (isAscii(codepoint))
        {
            return Characters::toDigit(static_cast<AnsiChar>(codepoint), radix);
        }

        return -1;
    }

    constexpr UnicodeChar toLowercase(UnicodeChar codepoint)
    {
        // Delegate ASCII range to Characters namespace
        if (isAscii(codepoint))
        {
            return static_cast<UnicodeChar>(Characters::toLowercase(static_cast<AnsiChar>(codepoint)));
        }

        // Latin-1 Supplement uppercase to lowercase (U+00C0-U+00DE to U+00E0-U+00FE)
        if (isLatin1SupplementUppercase(codepoint))
        {
            return codepoint + 0x20;
        }

        // Greek uppercase to lowercase (U+0391-U+03A1 to U+03B1-U+03C1, U+03A3-U+03AB to U+03C3-U+03CB)
        if (isGreekUppercase(codepoint))
        {
            return codepoint + 0x20;
        }

        // Cyrillic uppercase to lowercase (U+0400-U+042F to U+0430-U+044F)
        if (isCyrillicUppercase(codepoint))
        {
            return codepoint + 0x20;
        }

        // For other ranges, return as-is (full Unicode support would require case mapping tables)
        return codepoint;
    }

    constexpr UnicodeChar toUppercase(UnicodeChar codepoint)
    {
        // Delegate ASCII range to Characters namespace
        if (isAscii(codepoint))
        {
            return static_cast<UnicodeChar>(Characters::toUppercase(static_cast<AnsiChar>(codepoint)));
        }

        // Latin-1 Supplement lowercase to uppercase (U+00E0-U+00FE to U+00C0-U+00DE)
        if (isLatin1SupplementLowercase(codepoint))
        {
            return codepoint - 0x20;
        }

        // Greek lowercase to uppercase (U+03B1-U+03C1 to U+0391-U+03A1, U+03C3-U+03CB to U+03A3-U+03AB)
        if (isGreekLowercase(codepoint))
        {
            return codepoint - 0x20;
        }

        // Cyrillic lowercase to uppercase (U+0430-U+044F to U+0400-U+042F)
        if (isCyrillicLowercase(codepoint))
        {
            return codepoint - 0x20;
        }

        // For other ranges, return as-is (full Unicode support would require case mapping tables)
        return codepoint;
    }
}
