#pragma once

#include "Ark/Strings/AnsiCharacters.hpp"

namespace Ark::Characters
{
    constexpr bool isAlphabetic(AnsiChar ch)
    {
        uint8 value = static_cast<uint8>(ch);

        return (value >= 'A' && value <= 'Z') || (value >= 'a' && value <= 'z');
    }

    constexpr bool isNumeric(AnsiChar ch)
    {
        uint8 value = static_cast<uint8>(ch);

        return value >= '0' && value <= '9';
    }

    constexpr bool isAlphanumeric(AnsiChar ch)
    {
        return isAlphabetic(ch) || isNumeric(ch);
    }

    constexpr bool isAscii(AnsiChar ch)
    {
        return static_cast<uint8>(ch) <= AsciiMax;
    }

    constexpr bool isControl(AnsiChar ch)
    {
        uint8 value = static_cast<uint8>(ch);

        return value < 32 || value == 127;
    }

    constexpr bool isDigit(AnsiChar ch)
    {
        uint8 value = static_cast<uint8>(ch);

        return value >= '0' && value <= '9';
    }

    constexpr bool isHexDigit(AnsiChar ch)
    {
        uint8 value = static_cast<uint8>(ch);

        return (value >= '0' && value <= '9') ||
               (value >= 'A' && value <= 'F') ||
               (value >= 'a' && value <= 'f');
    }

    constexpr bool isPunctuation(AnsiChar ch)
    {
        uint8 value = static_cast<uint8>(ch);

        return (value >= 33 && value <= 47) ||
               (value >= 58 && value <= 64) ||
               (value >= 91 && value <= 96) ||
               (value >= 123 && value <= 126);
    }

    constexpr bool isGraphic(AnsiChar ch)
    {
        uint8 value = static_cast<uint8>(ch);

        return value >= 33 && value <= 126;
    }

    constexpr bool isWhitespace(AnsiChar ch)
    {
        uint8 value = static_cast<uint8>(ch);

        return value == ' ' || value == '\t' || value == '\n' ||
               value == '\v' || value == '\f' || value == '\r';
    }

    constexpr bool isUppercase(AnsiChar ch)
    {
        uint8 value = static_cast<uint8>(ch);

        return value >= 'A' && value <= 'Z';
    }

    constexpr bool isLowercase(AnsiChar ch)
    {
        uint8 value = static_cast<uint8>(ch);

        return value >= 'a' && value <= 'z';
    }

    constexpr bool isHexUppercase(AnsiChar ch)
    {
        uint8 value = static_cast<uint8>(ch);

        return value >= 'A' && value <= 'F';
    }

    constexpr bool isHexLowercase(AnsiChar ch)
    {
        uint8 value = static_cast<uint8>(ch);

        return value >= 'a' && value <= 'f';
    }

    constexpr int toDigit(AnsiChar ch, int radix)
    {
        uint8 value = static_cast<uint8>(ch);

        if (radix == 10 && value >= '0' && value <= '9')
        {
            return value - '0';
        }
        else if (radix == 16)
        {
            if (value >= '0' && value <= '9')
            {
                return value - '0';
            }
            else if (value >= 'A' && value <= 'F')
            {
                return value - 'A' + 10;
            }
            else if (value >= 'a' && value <= 'f')
            {
                return value - 'a' + 10;
            }
        }

        return -1;
    }

    constexpr AnsiChar toLowercase(AnsiChar ch)
    {
        uint8 value = static_cast<uint8>(ch);

        if (value >= 'A' && value <= 'Z')
        {
            return static_cast<AnsiChar>(value + 32);
        }

        return ch;
    }

    constexpr AnsiChar toUppercase(AnsiChar ch)
    {
        uint8 value = static_cast<uint8>(ch);

        if (value >= 'a' && value <= 'z')
        {
            return static_cast<AnsiChar>(value - 32);
        }

        return ch;
    }
}
