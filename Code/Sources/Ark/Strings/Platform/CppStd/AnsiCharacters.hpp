#pragma once

#include "Ark/Strings/AnsiCharacters.hpp"

#include <cctype>

namespace Ark::Characters
{
    inline ARK_CONSTEXPR bool isAlphabetic(AnsiChar ch)
    {
        return std::isalpha(static_cast<uint8>(ch)) != 0;
    }

    inline ARK_CONSTEXPR bool isNumeric(AnsiChar ch)
    {
        return std::isdigit(static_cast<uint8>(ch)) != 0;
    }

    inline ARK_CONSTEXPR bool isAlphanumeric(AnsiChar ch)
    {
        return std::isalnum(static_cast<uint8>(ch)) != 0;
    }

    inline ARK_CONSTEXPR bool isAscii(AnsiChar ch)
    {
        return static_cast<uint8>(ch) <= AsciiMax;
    }

    inline ARK_CONSTEXPR bool isControl(AnsiChar ch)
    {
        return std::iscntrl(static_cast<uint8>(ch)) != 0;
    }

    inline ARK_CONSTEXPR bool isDigit(AnsiChar ch)
    {
        return std::isdigit(static_cast<uint8>(ch)) != 0;
    }

    inline ARK_CONSTEXPR bool isHexDigit(AnsiChar ch)
    {
        return std::isxdigit(static_cast<uint8>(ch)) != 0;
    }

    inline ARK_CONSTEXPR bool isPunctuation(AnsiChar ch)
    {
        return std::ispunct(static_cast<uint8>(ch)) != 0;
    }

    inline ARK_CONSTEXPR bool isGraphic(AnsiChar ch)
    {
        return std::isgraph(static_cast<uint8>(ch)) != 0;
    }

    inline ARK_CONSTEXPR bool isWhitespace(AnsiChar ch)
    {
        return std::isspace(static_cast<uint8>(ch)) != 0;
    }

    inline ARK_CONSTEXPR bool isUppercase(AnsiChar ch)
    {
        return std::isupper(static_cast<uint8>(ch)) != 0;
    }

    inline ARK_CONSTEXPR bool isLowercase(AnsiChar ch)
    {
        return std::islower(static_cast<uint8>(ch)) != 0;
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

    inline ARK_CONSTEXPR int toDigit(AnsiChar ch, int radix)
    {
        uint8 value = static_cast<uint8>(ch);

        if (radix == 10 && std::isdigit(value))
        {
            return value - '0';
        }
        else if (radix == 16 && std::isxdigit(value))
        {
            return std::isdigit(value) ? value - '0' : std::tolower(value) - 'a' + 10;
        }

        return -1;
    }

    inline ARK_CONSTEXPR AnsiChar toLowercase(AnsiChar ch)
    {
        return static_cast<AnsiChar>(std::tolower(static_cast<uint8>(ch)));
    }

    inline ARK_CONSTEXPR AnsiChar toUppercase(AnsiChar ch)
    {
        return static_cast<AnsiChar>(std::toupper(static_cast<uint8>(ch)));
    }
}
