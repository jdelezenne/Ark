#pragma once

#include "Ark/Strings/AnsiStrings.hpp"

#include <string.h>
#include <string>

namespace Ark::AnsiStrings
{
    inline ARK_CONSTEXPR usize getLengthUnsafe(AnsiChar const* str)
    {
        ARK_ASSERT(str != nullptr);

        return static_cast<usize>(std::strlen(static_cast<const char*>(str)));
    }

    inline ARK_CONSTEXPR usize getLength(AnsiChar const* str, usize maxLength)
    {
        ARK_ASSERT(str != nullptr);
        ARK_ASSERT(maxLength != 0);

        return strnlen_s(static_cast<const char*>(str), maxLength);
    }

    inline ARK_CONSTEXPR void copyUnsafe(AnsiChar const* source, AnsiChar* destination)
    {
        ARK_ASSERT(source != nullptr);
        ARK_ASSERT(destination != nullptr);

        std::strcpy(static_cast<char*>(destination), static_cast<const char*>(source));
    }

    inline ARK_CONSTEXPR bool copy(AnsiChar const* source, AnsiChar* destination, usize maxLength)
    {
        ARK_ASSERT(source != nullptr);
        ARK_ASSERT(destination != nullptr);
        ARK_ASSERT(maxLength != 0);

        const auto result = strcpy_s(
            static_cast<char*>(destination),
            static_cast<rsize_t>(maxLength),
            static_cast<const char*>(source));

        return result == 0;
    }

    inline ARK_CONSTEXPR void copyUnsafe(AnsiChar const* source, AnsiChar* destination, usize count)
    {
        ARK_ASSERT(source != nullptr);
        ARK_ASSERT(destination != nullptr);
        ARK_ASSERT(count != 0);

        std::strncpy(
            static_cast<char*>(destination),
            static_cast<const char*>(source),
            static_cast<std::size_t>(count));
    }

    inline ARK_CONSTEXPR bool copy(AnsiChar const* source, AnsiChar* destination, usize maxLength, usize count)
    {
        ARK_ASSERT(source != nullptr);
        ARK_ASSERT(destination != nullptr);
        ARK_ASSERT(maxLength != 0);
        ARK_ASSERT(count != 0);

        const auto result = strncpy_s(
            static_cast<char*>(destination),
            static_cast<rsize_t>(maxLength),
            static_cast<const char*>(source),
            static_cast<rsize_t>(count));

        return result == 0;
    }

    inline ARK_CONSTEXPR void appendUnsafe(AnsiChar const* source, AnsiChar* destination)
    {
        ARK_ASSERT(source != nullptr);
        ARK_ASSERT(destination != nullptr);

        const auto result = std::strcat(
            static_cast<char*>(destination),
            static_cast<const char*>(source));
    }

    inline ARK_CONSTEXPR bool append(AnsiChar const* source, AnsiChar* destination, usize maxLength)
    {
        ARK_ASSERT(source != nullptr);
        ARK_ASSERT(destination != nullptr);
        ARK_ASSERT(maxLength != 0);

        const auto result = strcat_s(
            static_cast<char*>(destination),
            static_cast<rsize_t>(maxLength),
            static_cast<const char*>(source));

        return result == 0;
    }

    inline ARK_CONSTEXPR void appendUnsafe(AnsiChar const* source, AnsiChar* destination, usize count)
    {
        ARK_ASSERT(source != nullptr);
        ARK_ASSERT(destination != nullptr);
        ARK_ASSERT(count != 0);

        std::strncat(
            static_cast<char*>(destination),
            static_cast<const char*>(source),
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
            static_cast<rsize_t>(maxLength),
            static_cast<const char*>(source),
            static_cast<rsize_t>(count));

        return result == 0;
    }

    inline ARK_CONSTEXPR AnsiChar const* findFirstCharacterUnsafe(AnsiChar const* str, AnsiChar ch)
    {
        ARK_ASSERT(str != nullptr);

        const auto result = std::strchr(
            static_cast<const char*>(str),
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

        const auto result = std::strchr(
            static_cast<const char*>(str),
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

        const auto result = std::strrchr(
            static_cast<const char*>(str),
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

        const auto result = std::strrchr(
            static_cast<const char*>(str),
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

        const auto result = std::strcmp(
            static_cast<const char*>(lhs),
            static_cast<const char*>(rhs));

        return orderingFromInt(result);
    }

    inline Ordering compare(AnsiChar const* lhs, AnsiChar const* rhs, usize maxLength)
    {
        ARK_ASSERT(lhs != nullptr);
        ARK_ASSERT(rhs != nullptr);

        const auto result = std::strncmp(
            static_cast<const char*>(lhs),
            static_cast<const char*>(rhs),
            static_cast<std::size_t>(maxLength));

        return orderingFromInt(result);
    }

    inline ARK_CONSTEXPR Ordering compareUnsafe(AnsiChar const* lhs, AnsiChar const* rhs, usize count)
    {
        ARK_ASSERT(lhs != nullptr);
        ARK_ASSERT(rhs != nullptr);

        const auto result = std::strncmp(
            static_cast<const char*>(lhs),
            static_cast<const char*>(rhs),
            static_cast<std::size_t>(count));

        return orderingFromInt(result);
    }

    inline Ordering compare(AnsiChar const* lhs, AnsiChar const* rhs, usize maxLength, usize count)
    {
        ARK_ASSERT(lhs != nullptr);
        ARK_ASSERT(rhs != nullptr);
        ARK_ASSERT(maxLength != 0);

        const usize length = std::min(maxLength, count);

        const auto result = std::strncmp(
            static_cast<const char*>(lhs),
            static_cast<const char*>(rhs),
            static_cast<std::size_t>(length));

        return orderingFromInt(result);
    }

    inline ARK_CONSTEXPR Ordering compareCaseInsensitiveUnsafe(AnsiChar const* lhs, AnsiChar const* rhs)
    {
        ARK_ASSERT(lhs != nullptr);
        ARK_ASSERT(rhs != nullptr);

        const auto result = stricmp(
            static_cast<const char*>(lhs),
            static_cast<const char*>(rhs));

        return orderingFromInt(result);
    }

    inline ARK_CONSTEXPR Ordering compareCaseInsensitive(AnsiChar const* lhs, AnsiChar const* rhs, usize maxLength)
    {
        ARK_ASSERT(lhs != nullptr);
        ARK_ASSERT(rhs != nullptr);

        const auto result = strnicmp(
            static_cast<const char*>(lhs),
            static_cast<const char*>(rhs),
            static_cast<size_t>(maxLength));

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
            static_cast<const char*>(lhs),
            static_cast<const char*>(rhs),
            static_cast<size_t>(count));

        return orderingFromInt(result);
    }

    inline ARK_CONSTEXPR Ordering compareCaseInsensitive(AnsiChar const* lhs, AnsiChar const* rhs, usize maxLength, usize count)
    {
        ARK_ASSERT(lhs != nullptr);
        ARK_ASSERT(rhs != nullptr);

        const usize length = std::min(maxLength, count);

        const auto result = strnicmp(
            static_cast<const char*>(lhs),
            static_cast<const char*>(rhs),
            static_cast<size_t>(length));

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
