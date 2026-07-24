#pragma once

#include "Ark/Strings/AnsiStrings.hpp"

#include <string.h>

namespace Ark::AnsiStrings
{
    static inline int min(int a, int b)
    {
        return (a < b) ? a : b;
    }

    static inline int max(int a, int b)
    {
        return (a > b) ? a : b;
    }

    inline ARK_CONSTEXPR usize getLengthUnsafe(AnsiChar const* str)
    {
        ARK_ASSERT(str != nullptr);

        return strlen(static_cast<char const*>(str));
    }

    inline ARK_CONSTEXPR usize getLength(AnsiChar const* str, usize maxLength)
    {
        ARK_ASSERT(str != nullptr);
        ARK_ASSERT(maxLength != 0);

        return strnlen_s(static_cast<char const*>(str), maxLength);
    }

    inline ARK_CONSTEXPR void copyUnsafe(AnsiChar const* source, AnsiChar* destination)
    {
        ARK_ASSERT(source != nullptr);
        ARK_ASSERT(destination != nullptr);

        strcpy(static_cast<char*>(destination), static_cast<char const*>(source));
    }

    inline ARK_CONSTEXPR bool copy(AnsiChar const* source, AnsiChar* destination, usize maxLength)
    {
        ARK_ASSERT(source != nullptr);
        ARK_ASSERT(destination != nullptr);
        ARK_ASSERT(maxLength != 0);

        const auto result = strcpy_s(
            static_cast<char*>(destination),
            static_cast<rusize>(maxLength),
            static_cast<char const*>(source));

        return result == 0;
    }

    inline ARK_CONSTEXPR void copyUnsafe(AnsiChar const* source, AnsiChar* destination, usize count)
    {
        ARK_ASSERT(source != nullptr);
        ARK_ASSERT(destination != nullptr);
        ARK_ASSERT(count != 0);

        strncpy(
            static_cast<char*>(destination),
            static_cast<char const*>(source),
            static_cast<usize>(count));
    }

    inline ARK_CONSTEXPR bool copy(AnsiChar const* source, AnsiChar* destination, usize maxLength, usize count)
    {
        ARK_ASSERT(source != nullptr);
        ARK_ASSERT(destination != nullptr);
        ARK_ASSERT(maxLength != 0);
        ARK_ASSERT(count != 0);

        const auto result = strncpy_s(
            static_cast<char*>(destination),
            static_cast<rusize>(maxLength),
            static_cast<char const*>(source),
            static_cast<rusize>(count));

        return result == 0;
    }

    inline ARK_CONSTEXPR void appendUnsafe(AnsiChar const* source, AnsiChar* destination)
    {
        ARK_ASSERT(source != nullptr);
        ARK_ASSERT(destination != nullptr);

        const auto result = strcat(
            static_cast<char*>(destination),
            static_cast<char const*>(source));
    }

    inline ARK_CONSTEXPR bool append(AnsiChar const* source, AnsiChar* destination, usize maxLength)
    {
        ARK_ASSERT(source != nullptr);
        ARK_ASSERT(destination != nullptr);
        ARK_ASSERT(maxLength != 0);

        const auto result = strcat_s(
            static_cast<char*>(destination),
            static_cast<rusize>(maxLength),
            static_cast<char const*>(source));

        return result == 0;
    }

    inline ARK_CONSTEXPR void appendUnsafe(AnsiChar const* source, AnsiChar* destination, usize count)
    {
        ARK_ASSERT(source != nullptr);
        ARK_ASSERT(destination != nullptr);
        ARK_ASSERT(count != 0);

        strncat(
            static_cast<char*>(destination),
            static_cast<char const*>(source),
            static_cast<usize>(count));
    }

    inline ARK_CONSTEXPR bool append(AnsiChar const* source, AnsiChar* destination, usize maxLength, usize count)
    {
        ARK_ASSERT(source != nullptr);
        ARK_ASSERT(destination != nullptr);
        ARK_ASSERT(maxLength != 0);
        ARK_ASSERT(count != 0);

        const auto result = strncat_s(
            static_cast<char*>(destination),
            static_cast<rusize>(maxLength),
            static_cast<char const*>(source),
            static_cast<rusize>(count));

        return result == 0;
    }

    inline ARK_CONSTEXPR AnsiChar const* findFirstCharacterUnsafe(AnsiChar const* str, AnsiChar ch)
    {
        ARK_ASSERT(str != nullptr);

        const auto result = strchr(
            static_cast<char const*>(str),
            static_cast<int>(ch));

        return static_cast<AnsiChar const*>(result);
    }

    inline Option<AnsiChar const*> findFirstCharacter(AnsiChar const* str, AnsiChar ch, usize maxLength)
    {
        ARK_ASSERT(str != nullptr);
        ARK_ASSERT(maxLength != 0);

        if (str == nullptr || maxLength == 0)
        {
            return none;
        }

        const auto result = strchr(
            static_cast<char const*>(str),
            static_cast<int>(ch));

        if (result == nullptr || (result - str) >= maxLength)
        {
            return nullptr;
        }

        return static_cast<AnsiChar const*>(result);
    }

    inline ARK_CONSTEXPR AnsiChar const* findLastCharacterUnsafe(AnsiChar const* str, AnsiChar ch)
    {
        ARK_ASSERT(str != nullptr);

        const auto result = strrchr(
            static_cast<char const*>(str),
            static_cast<int>(ch));

        return static_cast<AnsiChar const*>(result);
    }

    inline Option<AnsiChar const*> findLastCharacter(AnsiChar const* str, AnsiChar ch, usize maxLength)
    {
        ARK_ASSERT(str != nullptr);
        ARK_ASSERT(maxLength != 0);

        if (str == nullptr || maxLength == 0)
        {
            return none;
        }

        const auto result = strrchr(
            static_cast<char const*>(str),
            static_cast<int>(ch));

        if (result == nullptr || (result - str) >= maxLength)
        {
            return nullptr;
        }

        return static_cast<AnsiChar const*>(result);
    }

    inline ARK_CONSTEXPR Ordering compareUnsafe(AnsiChar const* lhs, AnsiChar const* rhs)
    {
        ARK_ASSERT(lhs != nullptr);
        ARK_ASSERT(rhs != nullptr);

        const auto result = strcmp(
            static_cast<char const*>(lhs),
            static_cast<char const*>(rhs));

        return orderingFromInt(result);
    }

    inline Ordering compare(AnsiChar const* lhs, AnsiChar const* rhs, usize maxLength)
    {
        ARK_ASSERT(lhs != nullptr);
        ARK_ASSERT(rhs != nullptr);

        const auto result = strncmp(
            static_cast<char const*>(lhs),
            static_cast<char const*>(rhs),
            static_cast<usize>(maxLength));

        return orderingFromInt(result);
    }

    inline ARK_CONSTEXPR Ordering compareUnsafe(AnsiChar const* lhs, AnsiChar const* rhs, usize count)
    {
        ARK_ASSERT(lhs != nullptr);
        ARK_ASSERT(rhs != nullptr);

        const auto result = strncmp(
            static_cast<char const*>(lhs),
            static_cast<char const*>(rhs),
            static_cast<usize>(count));

        return orderingFromInt(result);
    }

    inline Ordering compare(AnsiChar const* lhs, AnsiChar const* rhs, usize maxLength, usize count)
    {
        ARK_ASSERT(lhs != nullptr);
        ARK_ASSERT(rhs != nullptr);
        ARK_ASSERT(maxLength != 0);

        const usize length = min(maxLength, count);

        const auto result = strncmp(
            static_cast<char const*>(lhs),
            static_cast<char const*>(rhs),
            static_cast<usize>(length));

        return orderingFromInt(result);
    }

    inline ARK_CONSTEXPR Ordering compareCaseInsensitiveUnsafe(AnsiChar const* lhs, AnsiChar const* rhs)
    {
        ARK_ASSERT(lhs != nullptr);
        ARK_ASSERT(rhs != nullptr);

        const auto result = stricmp(
            static_cast<char const*>(lhs),
            static_cast<char const*>(rhs));

        return orderingFromInt(result);
    }

    inline ARK_CONSTEXPR Ordering compareCaseInsensitive(AnsiChar const* lhs, AnsiChar const* rhs, usize maxLength)
    {
        ARK_ASSERT(lhs != nullptr);
        ARK_ASSERT(rhs != nullptr);

        const auto result = strnicmp(
            static_cast<char const*>(lhs),
            static_cast<char const*>(rhs),
            static_cast<usize>(maxLength));

        if (result == _NLSCMPERROR)
        {
            return Ordering::Less;
        }

        return orderingFromInt(result);
    }

    inline ARK_CONSTEXPR Ordering compareCaseInsensitiveUnsafe(AnsiChar const* lhs, AnsiChar const* rhs, usize count)
    {
        ARK_ASSERT(lhs != nullptr);
        ARK_ASSERT(rhs != nullptr);

        const auto result = strnicmp(
            static_cast<char const*>(lhs),
            static_cast<char const*>(rhs),
            static_cast<usize>(count));

        return orderingFromInt(result);
    }

    inline ARK_CONSTEXPR Ordering compareCaseInsensitive(AnsiChar const* lhs, AnsiChar const* rhs, usize maxLength, usize count)
    {
        ARK_ASSERT(lhs != nullptr);
        ARK_ASSERT(rhs != nullptr);

        const usize length = min(maxLength, count);

        const auto result = strnicmp(
            static_cast<char const*>(lhs),
            static_cast<char const*>(rhs),
            static_cast<usize>(length));

        if (result == _NLSCMPERROR)
        {
            return Ordering::Less;
        }

        return orderingFromInt(result);
    }

    inline ARK_CONSTEXPR bool isWhitespace(AnsiChar const* str)
    {
        ARK_ASSERT(str != nullptr);

        for (usize i = 0; i < getLengthUnsafe(str); ++i)
        {
            if (!Character::isWhitespace(str[i]))
            {
                return false;
            }
        }

        return true;
    }
}
