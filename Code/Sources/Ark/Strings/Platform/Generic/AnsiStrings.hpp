#pragma once

#include "Ark/Strings/AnsiCharacters.hpp"
#include "Ark/Strings/AnsiStrings.hpp"

namespace Ark::AnsiStrings
{
    constexpr usize getLengthUnsafe(AnsiChar const* str)
    {
        ARK_ASSERT(str != nullptr);

        AnsiChar const* ptr = str;

        while (*ptr != NullCharacter)
        {
            ptr++;
        }

        return ptr - str;
    }

    constexpr usize getLength(AnsiChar const* str, usize maxLength)
    {
        ARK_ASSERT(str != nullptr);
        ARK_ASSERT(maxLength != 0);

        AnsiChar const* ptr = str;

        while (*ptr != NullCharacter && maxLength != 0)
        {
            maxLength--;
            ptr++;
        }

        return ptr - str;
    }

    constexpr void copyUnsafe(AnsiChar const* source, AnsiChar* destination)
    {
        ARK_ASSERT(source != nullptr);
        ARK_ASSERT(destination != nullptr);

        while (*source != NullCharacter)
        {
            *destination = *source;

            source++;
            destination++;
        }

        *destination = NullCharacter;
    }

    constexpr bool copy(AnsiChar const* source, AnsiChar* destination, usize maxLength)
    {
        ARK_ASSERT(source != nullptr);
        ARK_ASSERT(destination != nullptr);
        ARK_ASSERT(maxLength != 0);

        if (source == nullptr || destination == nullptr || maxLength == 0)
        {
            return false;
        }

        AnsiChar* destPtr = destination;
        usize n = maxLength;

        while ((n != 0) && (*source != NullCharacter))
        {
            *destPtr = *source;

            n--;
            source++;
            destPtr++;
        }

        if (n == 0)
        {
            destination[maxLength - 1] = NullCharacter;
            return false;
        }

        *destPtr = NullCharacter;
        return true;
    }

    constexpr void copyUnsafe(AnsiChar const* source, AnsiChar* destination, usize count)
    {
        ARK_ASSERT(source != nullptr);
        ARK_ASSERT(destination != nullptr);
        ARK_ASSERT(count != 0);

        usize i = 0;
        for (; i < count && source[i] != NullCharacter; i++)
        {
            destination[i] = source[i];
        }

        for (; i < count; i++)
        {
            destination[i] = NullCharacter;
        }
    }

    constexpr bool copy(AnsiChar const* source, AnsiChar* destination, usize maxLength, usize count)
    {
        ARK_ASSERT(source != nullptr);
        ARK_ASSERT(destination != nullptr);
        ARK_ASSERT(maxLength != 0);
        ARK_ASSERT(count != 0);

        if (source == nullptr)
        {
            destination[0] = NullCharacter;
            return false;
        }

        if (destination == nullptr || maxLength == 0)
        {
            return false;
        }

        usize i = 0;
        for (; i < count && i < maxLength - 1 && source[i] != NullCharacter; i++)
        {
            destination[i] = source[i];
        }

        if (i == maxLength - 1 || i == count)
        {
            destination[i] = NullCharacter;

            if (i < count && source[i] != NullCharacter)
            {
                return false;
            }
        }
        else
        {
            for (; i < maxLength; i++)
            {
                destination[i] = NullCharacter;
            }
        }

        return true;
    }

    constexpr void appendUnsafe(AnsiChar const* source, AnsiChar* destination)
    {
        ARK_ASSERT(source != nullptr);
        ARK_ASSERT(destination != nullptr);

        AnsiChar* destPtr = destination;

        while (*destPtr != NullCharacter)
        {
            destPtr++;
        }

        while (*source != NullCharacter)
        {
            *destPtr = *source;

            source++;
            destPtr++;
        }

        *destPtr = NullCharacter;
    }

    constexpr bool append(AnsiChar const* source, AnsiChar* destination, usize maxLength)
    {
        ARK_ASSERT(source != nullptr);
        ARK_ASSERT(destination != nullptr);
        ARK_ASSERT(maxLength != 0);

        if (source == nullptr || destination == nullptr || maxLength == 0)
        {
            return false;
        }

        AnsiChar* destPtr = destination;
        usize n = maxLength;

        while (n != 0 && *destPtr != NullCharacter)
        {
            n--;
            destPtr++;
        }

        if (n == 0)
        {
            return false;
        }

        while (n != 0 && *source != NullCharacter)
        {
            *destPtr = *source;

            n--;
            source++;
            destPtr++;
        }

        if (n == 0)
        {
            destination[maxLength - 1] = NullCharacter;
            return false;
        }

        *destPtr = NullCharacter;
        return true;
    }

    constexpr void appendUnsafe(AnsiChar const* source, AnsiChar* destination, usize count)
    {
        ARK_ASSERT(source != nullptr);
        ARK_ASSERT(destination != nullptr);
        ARK_ASSERT(count != 0);

        AnsiChar* destPtr = destination;

        while (*destPtr != NullCharacter)
        {
            destPtr++;
        }

        usize i = 0;

        for (; i < count && source[i] != NullCharacter; i++)
        {
            destPtr[i] = source[i];
        }

        destPtr[i] = NullCharacter;
    }

    constexpr bool append(AnsiChar const* source, AnsiChar* destination, usize maxLength, usize count)
    {
        ARK_ASSERT(source != nullptr);
        ARK_ASSERT(destination != nullptr);
        ARK_ASSERT(maxLength != 0);
        ARK_ASSERT(count != 0);

        if (source == nullptr || destination == nullptr || maxLength == 0)
        {
            return false;
        }

        AnsiChar* destPtr = destination;
        usize remaining = maxLength;

        while (remaining > 1 && *destPtr)
        {
            destPtr++;
            remaining--;
        }

        if (remaining <= 1)
        {
            return false;
        }

        usize i = 0;

        for (; i < count && source[i] != NullCharacter && remaining > 1; i++)
        {
            *destPtr++ = source[i];
            remaining--;
        }

        *destPtr = NullCharacter;

        if (i < count && source[i] != NullCharacter)
        {
            return false;
        }

        return true;
    }

    constexpr AnsiChar const* findFirstCharacterUnsafe(AnsiChar const* str, AnsiChar ch)
    {
        ARK_ASSERT(str != nullptr);

        while (*str != NullCharacter && *str != ch)
        {
            str++;
        }

        if (*str == ch)
        {
            return const_cast<AnsiChar const*>(str);
        }
        else
        {
            return nullptr;
        }
    }

    constexpr AnsiChar const* findLastCharacterUnsafe(AnsiChar const* str, AnsiChar ch)
    {
        ARK_ASSERT(str != nullptr);

        AnsiChar const* last = nullptr;

        while (*str != NullCharacter)
        {
            if (*str == ch)
            {
                last = str;
            }

            str++;
        }

        return const_cast<AnsiChar const*>(last);
    }

    constexpr AnsiChar const* findFirstUnsafe(AnsiChar const* str, AnsiChar const* substring)
    {
        ARK_ASSERT(str != nullptr);
        ARK_ASSERT(substring != nullptr);

        if (*substring == NullCharacter)
        {
            return str;
        }

        for (; *str != NullCharacter; str++)
        {
            AnsiChar const* strPtr = str;
            AnsiChar const* subPtr = substring;

            while (*strPtr != NullCharacter && *subPtr != NullCharacter && (*strPtr == *subPtr))
            {
                strPtr++;
                subPtr++;
            }

            if (*subPtr == NullCharacter)
            {
                return str;
            }
        }

        return nullptr;
    }

    constexpr AnsiChar const* findFirstUnsafe(AnsiChar const* str, AnsiChar const* substring, usize count)
    {
        ARK_ASSERT(str != nullptr);
        ARK_ASSERT(substring != nullptr);

        usize subLength = getLengthUnsafe(substring);

        if (subLength == 0)
        {
            return str;
        }

        if (subLength > count)
        {
            return nullptr;
        }

        for (usize i = 0; i <= count - subLength; i++)
        {
            AnsiChar const* strPtr = str + i;
            AnsiChar const* subPtr = substring;
            bool matches = true;

            for (usize j = 0; j < subLength; j++)
            {
                if (strPtr[j] != subPtr[j])
                {
                    matches = false;
                    break;
                }
            }

            if (matches)
            {
                return static_cast<AnsiChar const*>(str + i);
            }
        }

        return nullptr;
    }

    constexpr AnsiChar const* findLastUnsafe(AnsiChar const* str, AnsiChar const* substring)
    {
        ARK_ASSERT(str != nullptr);
        ARK_ASSERT(substring != nullptr);

        if (*substring == NullCharacter)
        {
            return str;
        }

        AnsiChar const* result = nullptr;
        AnsiChar const* current = findFirstUnsafe(str, substring);

        while (current != nullptr)
        {
            result = current;
            current = findFirstUnsafe(current + 1, substring);
        }

        return result;
    }

    constexpr AnsiChar const* findLastUnsafe(AnsiChar const* str, AnsiChar const* substring, usize count)
    {
        ARK_ASSERT(str != nullptr);
        ARK_ASSERT(substring != nullptr);

        usize subLength = getLengthUnsafe(substring);

        if (subLength == 0)
        {
            return str;
        }

        if (subLength > count)
        {
            return nullptr;
        }

        AnsiChar const* result = nullptr;
        AnsiChar const* current = findFirstUnsafe(str, substring, count);

        while (current != nullptr)
        {
            result = current;
            // Calculate remaining count from current + 1
            usize remainingCount = count - (current - str + 1);
            if (remainingCount < subLength)
            {
                break;
            }
            current = findFirstUnsafe(current + 1, substring, remainingCount);
        }

        return result;
    }

    constexpr Ordering compareUnsafe(AnsiChar const* lhs, AnsiChar const* rhs)
    {
        if (lhs == nullptr && rhs == nullptr)
        {
            return Ordering::Equal;
        }
        if (lhs == nullptr)
        {
            return Ordering::Less;
        }
        if (rhs == nullptr)
        {
            return Ordering::Greater;
        }

        while (*lhs != NullCharacter && (*lhs == *rhs))
        {
            lhs++;
            rhs++;
        }

        const AnsiChar delta = *lhs - *rhs;
        return orderingFromInt(delta);
    }

    constexpr Ordering compareUnsafe(AnsiChar const* lhs, AnsiChar const* rhs, usize count)
    {
        if (lhs == nullptr && rhs == nullptr)
        {
            return Ordering::Equal;
        }
        if (lhs == nullptr)
        {
            return Ordering::Less;
        }
        if (rhs == nullptr)
        {
            return Ordering::Greater;
        }

        if (count == 0)
        {
            return Ordering::Equal;
        }

        while (count != 0 && *lhs != NullCharacter && (*lhs == *rhs))
        {
            count--;
            lhs++;
            rhs++;
        }

        if (count == 0)
        {
            return Ordering::Equal;
        }

        const AnsiChar delta = *lhs - *rhs;
        return orderingFromInt(delta);
    }

    constexpr Ordering compareCaseInsensitiveUnsafe(AnsiChar const* lhs, AnsiChar const* rhs)
    {
        if (lhs == nullptr && rhs == nullptr)
        {
            return Ordering::Equal;
        }
        if (lhs == nullptr)
        {
            return Ordering::Less;
        }
        if (rhs == nullptr)
        {
            return Ordering::Greater;
        }

        while (*lhs != NullCharacter &&
               Characters::toLowercase(*lhs) == Characters::toLowercase(*rhs))
        {
            lhs++;
            rhs++;
        }

        const AnsiChar lhsLower = Characters::toLowercase(*lhs);
        const AnsiChar rhsLower = Characters::toLowercase(*rhs);
        const AnsiChar delta = lhsLower - rhsLower;
        return orderingFromInt(delta);
    }

    constexpr Ordering compareCaseInsensitiveUnsafe(AnsiChar const* lhs, AnsiChar const* rhs, usize count)
    {
        ARK_ASSERT(lhs != nullptr);
        ARK_ASSERT(rhs != nullptr);

        if (count == 0)
        {
            return Ordering::Equal;
        }

        while (count != 0 &&
               *lhs != NullCharacter &&
               Characters::toLowercase(*lhs) == Characters::toLowercase(*rhs))
        {
            count--;
            lhs++;
            rhs++;
        }

        if (count == 0)
        {
            return Ordering::Equal;
        }

        const AnsiChar lhsLower = Characters::toLowercase(*lhs);
        const AnsiChar rhsLower = Characters::toLowercase(*rhs);
        const AnsiChar delta = lhsLower - rhsLower;
        return orderingFromInt(delta);
    }

    constexpr bool isWhitespace(AnsiChar const* str)
    {
        ARK_ASSERT(str != nullptr);

        if (str == nullptr)
        {
            return false;
        }

        while (*str != NullCharacter)
        {
            if (!Characters::isWhitespace(*str))
            {
                return false;
            }

            str++;
        }

        return true;
    }

    inline Option<AnsiChar const*> findFirstCharacter(AnsiChar const* str, AnsiChar ch, usize maxLength)
    {
        ARK_ASSERT(str != nullptr);
        ARK_ASSERT(maxLength != 0);

        if (str == nullptr || maxLength == 0)
        {
            return none;
        }

        while (maxLength != 0 && *str != NullCharacter)
        {
            if (*str == ch)
            {
                return str;
            }

            maxLength--;
            str++;
        }

        return none;
    }

    inline Option<AnsiChar const*> findLastCharacter(AnsiChar const* str, AnsiChar ch, usize maxLength)
    {
        ARK_ASSERT(str != nullptr);
        ARK_ASSERT(maxLength != 0);

        if (str == nullptr || maxLength == 0)
        {
            return none;
        }

        AnsiChar const* end = str + getLength(str, maxLength);

        while (end > str)
        {
            end--;

            if (*end == ch)
            {
                return end;
            }
        }

        return none;
    }

    inline Option<AnsiChar const*> findFirst(AnsiChar const* str, AnsiChar const* substring, usize maxLength)
    {
        ARK_ASSERT(str != nullptr);
        ARK_ASSERT(substring != nullptr);
        ARK_ASSERT(maxLength != 0);

        if (str == nullptr || substring == nullptr || maxLength == 0)
        {
            return none;
        }

        AnsiChar const* const result = findFirstUnsafe(str, substring, maxLength);
        if (result == nullptr)
        {
            return none;
        }

        return result;
    }

    inline Option<AnsiChar const*> findLast(AnsiChar const* str, AnsiChar const* substring, usize maxLength)
    {
        ARK_ASSERT(str != nullptr);
        ARK_ASSERT(substring != nullptr);
        ARK_ASSERT(maxLength != 0);

        if (str == nullptr || substring == nullptr || maxLength == 0)
        {
            return none;
        }

        AnsiChar const* const result = findLastUnsafe(str, substring, maxLength);
        if (result == nullptr)
        {
            return none;
        }

        return result;
    }

    inline Ordering compare(AnsiChar const* lhs, AnsiChar const* rhs, usize maxLength)
    {
        ARK_ASSERT(lhs != nullptr);
        ARK_ASSERT(rhs != nullptr);

        if (lhs == nullptr)
        {
            return (rhs == nullptr) ? Ordering::Equal : Ordering::Less;
        }
        if (rhs == nullptr)
        {
            return Ordering::Greater;
        }

        while (maxLength != 0 && *lhs != NullCharacter && (*lhs == *rhs))
        {
            maxLength--;
            lhs++;
            rhs++;
        }

        if (maxLength == 0)
        {
            return Ordering::Equal;
        }

        return orderingFromInt(static_cast<int>(*lhs) - static_cast<int>(*rhs));
    }

    inline Ordering compare(AnsiChar const* lhs, AnsiChar const* rhs, usize maxLength, usize count)
    {
        ARK_ASSERT(lhs != nullptr);
        ARK_ASSERT(rhs != nullptr);
        ARK_ASSERT(maxLength != 0);

        if (lhs == nullptr || rhs == nullptr || maxLength == 0)
        {
            if (lhs == nullptr && rhs == nullptr)
            {
                return Ordering::Equal;
            }
            return (lhs == nullptr) ? Ordering::Less : Ordering::Greater;
        }

        if (count == 0)
        {
            return Ordering::Equal;
        }

        while (maxLength != 0 && count != 0 && *lhs != NullCharacter && (*lhs == *rhs))
        {
            maxLength--;
            count--;
            lhs++;
            rhs++;
        }

        if (maxLength == 0 || count == 0)
        {
            return Ordering::Equal;
        }

        return orderingFromInt(static_cast<int>(*lhs) - static_cast<int>(*rhs));
    }

    inline Ordering compareCaseInsensitive(AnsiChar const* lhs, AnsiChar const* rhs, usize maxLength)
    {
        ARK_ASSERT(lhs != nullptr);
        ARK_ASSERT(rhs != nullptr);

        if (lhs == nullptr)
        {
            return (rhs == nullptr) ? Ordering::Equal : Ordering::Less;
        }
        if (rhs == nullptr)
        {
            return Ordering::Greater;
        }

        while (maxLength != 0 &&
               *lhs != NullCharacter &&
               Characters::toLowercase(*lhs) == Characters::toLowercase(*rhs))
        {
            maxLength--;
            lhs++;
            rhs++;
        }

        if (maxLength == 0)
        {
            return Ordering::Equal;
        }

        AnsiChar const lhsLower = Characters::toLowercase(*lhs);
        AnsiChar const rhsLower = Characters::toLowercase(*rhs);
        return orderingFromInt(static_cast<int>(lhsLower) - static_cast<int>(rhsLower));
    }

    inline Ordering compareCaseInsensitive(AnsiChar const* lhs, AnsiChar const* rhs, usize maxLength, usize count)
    {
        ARK_ASSERT(lhs != nullptr);
        ARK_ASSERT(rhs != nullptr);
        ARK_ASSERT(maxLength != 0);

        if (lhs == nullptr || rhs == nullptr || maxLength == 0)
        {
            if (lhs == nullptr && rhs == nullptr)
            {
                return Ordering::Equal;
            }
            return (lhs == nullptr) ? Ordering::Less : Ordering::Greater;
        }

        if (count == 0)
        {
            return Ordering::Equal;
        }

        while (maxLength != 0 &&
               count != 0 &&
               *lhs != NullCharacter &&
               Characters::toLowercase(*lhs) == Characters::toLowercase(*rhs))
        {
            maxLength--;
            count--;
            lhs++;
            rhs++;
        }

        if (maxLength == 0 || count == 0)
        {
            return Ordering::Equal;
        }

        AnsiChar const lhsLower = Characters::toLowercase(*lhs);
        AnsiChar const rhsLower = Characters::toLowercase(*rhs);
        return orderingFromInt(static_cast<int>(lhsLower) - static_cast<int>(rhsLower));
    }
}
