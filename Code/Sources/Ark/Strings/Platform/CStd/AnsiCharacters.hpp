#pragma once

#include "Ark/Strings/AnsiCharacters.hpp"

#include <ctype.h>

namespace Ark::Characters
{
    inline ARK_CONSTEXPR bool isAlphabetic(AnsiChar ch)
    {
        return isalpha(static_cast<uint8>(ch)) != 0;
    }

    inline ARK_CONSTEXPR bool isNumeric(AnsiChar ch)
    {
        return isdigit(static_cast<uint8>(ch)) != 0;
    }

    inline ARK_CONSTEXPR bool isAlphanumeric(AnsiChar ch)
    {
        return isalnum(static_cast<uint8>(ch)) != 0;
    }

    inline ARK_CONSTEXPR bool isAscii(AnsiChar ch)
    {
        return static_cast<uint8>(ch) <= AsciiMax;
    }

    inline ARK_CONSTEXPR bool isControl(AnsiChar ch)
    {
        return iscntrl(static_cast<uint8>(ch)) != 0;
    }

    inline ARK_CONSTEXPR bool isDigit(AnsiChar ch)
    {
        return isdigit(static_cast<uint8>(ch)) != 0;
    }

    inline ARK_CONSTEXPR bool isHexDigit(AnsiChar ch)
    {
        return isxdigit(static_cast<uint8>(ch)) != 0;
    }

    inline ARK_CONSTEXPR bool isPunctuation(AnsiChar ch)
    {
        return ispunct(static_cast<uint8>(ch)) != 0;
    }

    inline ARK_CONSTEXPR bool isGraphic(AnsiChar ch)
    {
        return isgraph(static_cast<uint8>(ch)) != 0;
    }

    inline ARK_CONSTEXPR bool isWhitespace(AnsiChar ch)
    {
        return isspace(static_cast<uint8>(ch)) != 0;
    }

    inline ARK_CONSTEXPR bool isUppercase(AnsiChar ch)
    {
        return isupper(static_cast<uint8>(ch)) != 0;
    }

    inline ARK_CONSTEXPR bool isLowercase(AnsiChar ch)
    {
        return islower(static_cast<uint8>(ch)) != 0;
    }

    inline ARK_CONSTEXPR bool isHexUppercase(AnsiChar ch)
    {
        uint8 value = static_cast<uint8>(ch);

        return (value >= 'A' && value <= 'F');
    }

    inline ARK_CONSTEXPR bool isHexLowercase(AnsiChar ch)
    {
        uint8 value = static_cast<uint8>(ch);

        return (value >= 'a' && value <= 'f');
    }

    int toDigit(AnsiChar ch, int radix)
    {
        uint8 value = static_cast<uint8>(ch);

        if (radix == 10 && isdigit(static_cast<uint8>(value)))
        {
            return value - '0';
        }
        else if (radix == 16 && isxdigit(static_cast<uint8>(value)))
        {
            return isdigit(static_cast<uint8>(value)) ? value - '0' : tolower(static_cast<uint8>(value)) - 'a' + 10;
        }

        return -1;
    }

    inline ARK_CONSTEXPR AnsiChar toUppercase(AnsiChar ch)
    {
        return static_cast<AnsiChar>(toupper(static_cast<uint8>(ch)));
    }

    inline ARK_CONSTEXPR AnsiChar toLowercase(AnsiChar ch)
    {
        return static_cast<AnsiChar>(tolower(static_cast<uint8>(ch)));
    }
}
