#pragma once

#include "Ark/Strings/Utf8Characters.hpp"

#if (ARK_USE_STANDARD_LIBRARY == ARK_STANDARD_LIBRARY_CPP) || (ARK_USE_STANDARD_LIBRARY == ARK_STANDARD_LIBRARY_C)
#define ARK_CONSTEXPR
#else
#define ARK_CONSTEXPR constexpr
#endif

#include "Ark/Strings/Utf8Strings.hpp"

namespace Ark::Utf8Strings
{
    namespace
    {
        inline bool isContinuation(uint8 byte)
        {
            return (byte & 0xC0u) == 0x80u;
        }

        inline uint32 decodeLeading(uint8 b, usize& expected)
        {
            if ((b & 0x80u) == 0x00u)
            {
                expected = 0;
                return b;
            }

            if ((b & 0xE0u) == 0xC0u)
            {
                expected = 1;
                return b & 0x1Fu;
            }

            if ((b & 0xF0u) == 0xE0u)
            {
                expected = 2;
                return b & 0x0Fu;
            }

            if ((b & 0xF8u) == 0xF0u)
            {
                expected = 3;
                return b & 0x07u;
            }

            expected = static_cast<usize>(-1);
            return 0;
        }

        inline bool isOverlong(uint32 cp, usize bytes)
        {
            switch (bytes)
            {
                case 1:
                    return false;

                case 2:
                    return cp < 0x80u;

                case 3:
                    return cp < 0x800u;

                case 4:
                    return cp < 0x10000u;

                default:
                    return false;
            }
        }

        struct DecodeResult
        {
            UnicodeChar codepoint;
            usize nextIndex;
            bool valid;
        };

        inline DecodeResult decodeAt(char const* bytes, usize byteLength, usize index)
        {
            if (index >= byteLength)
            {
                return {0, index, false};
            }

            uint8 const firstByte = static_cast<uint8>(bytes[index]);
            usize expected = 0;
            uint32 codepoint = decodeLeading(firstByte, expected);
            if (expected == static_cast<usize>(-1))
            {
                return {0, index, false};
            }

            if (expected == 0)
            {
                return {static_cast<UnicodeChar>(codepoint), index + 1, true};
            }

            if (index + expected >= byteLength)
            {
                return {0, index, false};
            }

            for (usize k = 0; k < expected; ++k)
            {
                uint8 const currentByte = static_cast<uint8>(bytes[index + 1 + k]);
                if (!isContinuation(currentByte))
                {
                    return {0, index, false};
                }

                codepoint = (codepoint << 6) | (currentByte & 0x3Fu);
            }

            usize const totalBytes = expected + 1;

            if (isOverlong(codepoint, totalBytes))
            {
                return {0, index, false};
            }

            if (codepoint > 0x10FFFFu || (codepoint >= 0xD800u && codepoint <= 0xDFFFu))
            {
                return {0, index, false};
            }

            return {static_cast<UnicodeChar>(codepoint), index + totalBytes, true};
        }

        inline Option<usize> nextCharBoundary(char const* bytes, usize byteLength, usize index)
        {
            if (index > byteLength)
            {
                return none;
            }

            if (index == byteLength)
            {
                return index;
            }

            for (usize i = index; i < byteLength; ++i)
            {
                uint8 const b = static_cast<uint8>(bytes[i]);
                if (!isContinuation(b))
                {
                    return i;
                }
            }

            return none;
        }
    }
    inline ARK_CONSTEXPR usize getByteLengthUnsafe(char const* str)
    {
        ARK_ASSERT(str != nullptr);

        if (str == nullptr)
        {
            return 0;
        }

        usize length = 0;
        while (str[length] != NullCharacter)
        {
            ++length;
        }

        return length;
    }

    inline ARK_CONSTEXPR usize getByteLength(char const* str, usize maxBytes)
    {
        ARK_ASSERT(str != nullptr);
        ARK_ASSERT(maxBytes != 0);

        if (str == nullptr || maxBytes == 0)
        {
            return 0;
        }

        usize length = 0;
        while (length < maxBytes && str[length] != NullCharacter)
        {
            ++length;
        }

        return length;
    }

    inline usize getLength(char const* str)
    {
        ARK_ASSERT(str != nullptr);

        if (str == nullptr)
        {
            return 0;
        }

        // Count bytes until null to get byte length
        usize byteLength = getByteLengthUnsafe(str);

        // Count codepoints (validates UTF-8 encoding)
        return getCodepointCount(str, byteLength);
    }

    inline usize getLength(char const* str, usize maxBytes)
    {
        ARK_ASSERT(str != nullptr);
        ARK_ASSERT(maxBytes != 0);

        if (str == nullptr || maxBytes == 0)
        {
            return 0;
        }

        // Count bytes until null or maxBytes
        usize byteLength = getByteLength(str, maxBytes);

        // Count codepoints (validates UTF-8 encoding)
        return getCodepointCount(str, byteLength);
    }

    inline usize getCodepointCount(char const* str, usize byteLength)
    {
        ARK_ASSERT(str != nullptr);

        if (str == nullptr || byteLength == 0)
        {
            return 0;
        }

        usize count = 0;
        usize index = 0;
        while (index < byteLength)
        {
            auto result = decodeAt(str, byteLength, index);
            if (!result.valid)
            {
                break;
            }
            ++count;
            index = result.nextIndex;
        }
        return count;
    }

    inline Ordering compare(char const* lhs, usize lhsByteLength, char const* rhs, usize rhsByteLength)
    {
        ARK_ASSERT(lhs != nullptr);
        ARK_ASSERT(rhs != nullptr);

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

        usize lhsIndex = 0;
        usize rhsIndex = 0;

        while (lhsIndex < lhsByteLength && rhsIndex < rhsByteLength)
        {
            auto lhsValue = decodeAt(lhs, lhsByteLength, lhsIndex);
            auto rhsValue = decodeAt(rhs, rhsByteLength, rhsIndex);

            if (!lhsValue.valid || !rhsValue.valid)
            {
                // Invalid UTF-8: lexicographically compare remaining bytes.
                usize const lhsRemaining = lhsByteLength - lhsIndex;
                usize const rhsRemaining = rhsByteLength - rhsIndex;
                usize const n = Ark::min(lhsRemaining, rhsRemaining);
                for (usize i = 0; i < n; ++i)
                {
                    uint8 const lb = static_cast<uint8>(lhs[lhsIndex + i]);
                    uint8 const rb = static_cast<uint8>(rhs[rhsIndex + i]);
                    if (lb != rb)
                    {
                        return (lb < rb) ? Ordering::Less : Ordering::Greater;
                    }
                }
                if (lhsRemaining == rhsRemaining)
                {
                    return Ordering::Equal;
                }
                return (lhsRemaining < rhsRemaining) ? Ordering::Less : Ordering::Greater;
            }

            if (lhsValue.codepoint != rhsValue.codepoint)
            {
                return (lhsValue.codepoint < rhsValue.codepoint) ? Ordering::Less : Ordering::Greater;
            }

            lhsIndex = lhsValue.nextIndex;
            rhsIndex = rhsValue.nextIndex;
        }

        if (lhsIndex >= lhsByteLength && rhsIndex >= rhsByteLength)
        {
            return Ordering::Equal;
        }

        return (lhsIndex >= lhsByteLength) ? Ordering::Less : Ordering::Greater;
    }

    inline Ordering compareCaseInsensitive(char const* lhs, usize lhsByteLength, char const* rhs, usize rhsByteLength)
    {
        ARK_ASSERT(lhs != nullptr);
        ARK_ASSERT(rhs != nullptr);

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

        usize lhsIndex = 0;
        usize rhsIndex = 0;

        while (lhsIndex < lhsByteLength && rhsIndex < rhsByteLength)
        {
            auto lhsValue = decodeAt(lhs, lhsByteLength, lhsIndex);
            auto rhsValue = decodeAt(rhs, rhsByteLength, rhsIndex);

            if (!lhsValue.valid || !rhsValue.valid)
            {
                // Invalid UTF-8: lexicographically compare remaining bytes.
                usize const lhsRemaining = lhsByteLength - lhsIndex;
                usize const rhsRemaining = rhsByteLength - rhsIndex;
                usize const n = Ark::min(lhsRemaining, rhsRemaining);
                for (usize i = 0; i < n; ++i)
                {
                    uint8 const lb = static_cast<uint8>(lhs[lhsIndex + i]);
                    uint8 const rb = static_cast<uint8>(rhs[rhsIndex + i]);
                    if (lb != rb)
                    {
                        return (lb < rb) ? Ordering::Less : Ordering::Greater;
                    }
                }
                if (lhsRemaining == rhsRemaining)
                {
                    return Ordering::Equal;
                }
                return (lhsRemaining < rhsRemaining) ? Ordering::Less : Ordering::Greater;
            }

            UnicodeChar lhsLower = Utf8Characters::toLowercase(lhsValue.codepoint);
            UnicodeChar rhsLower = Utf8Characters::toLowercase(rhsValue.codepoint);

            if (lhsLower != rhsLower)
            {
                return (lhsLower < rhsLower) ? Ordering::Less : Ordering::Greater;
            }

            lhsIndex = lhsValue.nextIndex;
            rhsIndex = rhsValue.nextIndex;
        }

        if (lhsIndex >= lhsByteLength && rhsIndex >= rhsByteLength)
        {
            return Ordering::Equal;
        }

        return (lhsIndex >= lhsByteLength) ? Ordering::Less : Ordering::Greater;
    }

    inline Ordering compare(char const* lhs, char const* rhs, usize count)
    {
        ARK_ASSERT(lhs != nullptr);
        ARK_ASSERT(rhs != nullptr);

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

        usize lhsIndex = 0;
        usize rhsIndex = 0;

        while (lhsIndex < count && rhsIndex < count)
        {
            auto lhsValue = decodeAt(lhs, count, lhsIndex);
            auto rhsValue = decodeAt(rhs, count, rhsIndex);

            if (!lhsValue.valid || !rhsValue.valid)
            {
                // Invalid UTF-8, compare as bytes
                return Ordering::Equal;
            }

            if (lhsValue.codepoint != rhsValue.codepoint)
            {
                return (lhsValue.codepoint < rhsValue.codepoint) ? Ordering::Less : Ordering::Greater;
            }

            lhsIndex = lhsValue.nextIndex;
            rhsIndex = rhsValue.nextIndex;
        }

        return Ordering::Equal;
    }

    inline Ordering compareCaseInsensitive(char const* lhs, char const* rhs, usize count)
    {
        ARK_ASSERT(lhs != nullptr);
        ARK_ASSERT(rhs != nullptr);

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

        usize lhsIndex = 0;
        usize rhsIndex = 0;

        while (lhsIndex < count && rhsIndex < count)
        {
            auto lhsValue = decodeAt(lhs, count, lhsIndex);
            auto rhsValue = decodeAt(rhs, count, rhsIndex);

            if (!lhsValue.valid || !rhsValue.valid)
            {
                // Invalid UTF-8, compare as bytes
                return Ordering::Equal;
            }

            UnicodeChar lhsLower = Utf8Characters::toLowercase(lhsValue.codepoint);
            UnicodeChar rhsLower = Utf8Characters::toLowercase(rhsValue.codepoint);

            if (lhsLower != rhsLower)
            {
                return (lhsLower < rhsLower) ? Ordering::Less : Ordering::Greater;
            }

            lhsIndex = lhsValue.nextIndex;
            rhsIndex = rhsValue.nextIndex;
        }

        return Ordering::Equal;
    }

    inline char const* findFirstCodepoint(char const* str, usize byteLength, UnicodeChar codepoint)
    {
        ARK_ASSERT(str != nullptr);

        if (str == nullptr || byteLength == 0)
        {
            return nullptr;
        }

        usize index = 0;
        while (index < byteLength)
        {
            auto result = decodeAt(str, byteLength, index);
            if (!result.valid)
            {
                break;
            }

            if (result.codepoint == codepoint)
            {
                return str + index;
            }

            index = result.nextIndex;
        }

        return nullptr;
    }

    inline char const* findLastCodepoint(char const* str, usize byteLength, UnicodeChar codepoint)
    {
        ARK_ASSERT(str != nullptr);

        if (str == nullptr || byteLength == 0)
        {
            return nullptr;
        }

        char const* lastFound = nullptr;
        usize index = 0;

        while (index < byteLength)
        {
            auto value = decodeAt(str, byteLength, index);
            if (!value.valid)
            {
                break;
            }

            if (value.codepoint == codepoint)
            {
                lastFound = str + index;
            }

            index = value.nextIndex;
        }

        return lastFound;
    }

    inline char const* findFirst(char const* haystack, usize haystackByteLength, char const* needle, usize needleByteLength)
    {
        ARK_ASSERT(haystack != nullptr);
        ARK_ASSERT(needle != nullptr);

        if (haystack == nullptr || needle == nullptr || needleByteLength == 0)
        {
            return (needleByteLength == 0) ? haystack : nullptr;
        }

        if (needleByteLength > haystackByteLength)
        {
            return nullptr;
        }

        for (usize i = 0; i <= haystackByteLength - needleByteLength; ++i)
        {
            // Check if we're at a codepoint boundary
            if (i > 0)
            {
                uint8 const b = static_cast<uint8>(haystack[i]);
                if (isContinuation(b))
                {
                    // This is a continuation byte, skip it
                    continue;
                }
            }

            // Compare byte sequences first (fast path)
            bool matches = true;
            for (usize j = 0; j < needleByteLength; ++j)
            {
                if (haystack[i + j] != needle[j])
                {
                    matches = false;
                    break;
                }
            }

            if (matches)
            {
                // Verify it's a valid UTF-8 match by comparing codepoints
                if (compare(haystack + i, needleByteLength, needle, needleByteLength) == Ordering::Equal)
                {
                    return haystack + i;
                }
            }
        }

        return nullptr;
    }

    inline char const* findLast(char const* haystack, usize haystackByteLength, char const* needle, usize needleByteLength)
    {
        ARK_ASSERT(haystack != nullptr);
        ARK_ASSERT(needle != nullptr);

        if (haystack == nullptr || needle == nullptr || needleByteLength == 0)
        {
            return (needleByteLength == 0) ? haystack : nullptr;
        }

        if (needleByteLength > haystackByteLength)
        {
            return nullptr;
        }

        char const* lastFound = nullptr;

        for (usize i = 0; i <= haystackByteLength - needleByteLength; ++i)
        {
            // Check if we're at a codepoint boundary
            if (i > 0)
            {
                uint8 const b = static_cast<uint8>(haystack[i]);
                if (isContinuation(b))
                {
                    // This is a continuation byte, skip it
                    continue;
                }
            }

            // Compare byte sequences first (fast path)
            bool matches = true;
            for (usize j = 0; j < needleByteLength; ++j)
            {
                if (haystack[i + j] != needle[j])
                {
                    matches = false;
                    break;
                }
            }

            if (matches)
            {
                // Verify it's a valid UTF-8 match by comparing codepoints
                if (compare(haystack + i, needleByteLength, needle, needleByteLength) == Ordering::Equal)
                {
                    lastFound = haystack + i;
                }
            }
        }

        return lastFound;
    }

    inline bool startsWith(char const* str, usize strByteLength, char const* prefix, usize prefixByteLength)
    {
        ARK_ASSERT(str != nullptr);
        ARK_ASSERT(prefix != nullptr);

        if (prefixByteLength == 0)
        {
            return true;
        }

        if (prefixByteLength > strByteLength)
        {
            return false;
        }

        return compare(str, prefixByteLength, prefix, prefixByteLength) == Ordering::Equal;
    }

    inline bool endsWith(char const* str, usize strByteLength, char const* suffix, usize suffixByteLength)
    {
        ARK_ASSERT(str != nullptr);
        ARK_ASSERT(suffix != nullptr);

        if (suffixByteLength == 0)
        {
            return true;
        }

        if (suffixByteLength > strByteLength)
        {
            return false;
        }

        return compare(str + (strByteLength - suffixByteLength), suffixByteLength, suffix, suffixByteLength) == Ordering::Equal;
    }

    inline bool contains(char const* haystack, usize haystackByteLength, char const* needle, usize needleByteLength)
    {
        return findFirst(haystack, haystackByteLength, needle, needleByteLength) != nullptr;
    }

    inline bool startsWithIgnoreCase(char const* haystack, usize haystackByteLength, char const* needle, usize needleByteLength)
    {
        ARK_ASSERT(haystack != nullptr);
        ARK_ASSERT(needle != nullptr);

        if (needleByteLength == 0)
        {
            return true;
        }

        usize hayIndex = 0;
        usize needleIndex = 0;

        while (needleIndex < needleByteLength)
        {
            if (hayIndex >= haystackByteLength)
            {
                return false;
            }

            auto hayValue = decodeAt(haystack, haystackByteLength, hayIndex);
            auto needleValue = decodeAt(needle, needleByteLength, needleIndex);

            if (!hayValue.valid || !needleValue.valid)
            {
                usize const hayRemaining = haystackByteLength - hayIndex;
                usize const needleRemaining = needleByteLength - needleIndex;
                if (needleRemaining > hayRemaining)
                {
                    return false;
                }
                for (usize i = 0; i < needleRemaining; ++i)
                {
                    if (haystack[hayIndex + i] != needle[needleIndex + i])
                    {
                        return false;
                    }
                }
                return true;
            }

            if (Utf8Characters::toLowercase(hayValue.codepoint) != Utf8Characters::toLowercase(needleValue.codepoint))
            {
                return false;
            }

            hayIndex = hayValue.nextIndex;
            needleIndex = needleValue.nextIndex;
        }

        return true;
    }

    inline bool containsIgnoreCase(char const* haystack, usize haystackByteLength, char const* needle, usize needleByteLength)
    {
        ARK_ASSERT(haystack != nullptr);
        ARK_ASSERT(needle != nullptr);

        if (haystack == nullptr || needle == nullptr || needleByteLength == 0)
        {
            return (needleByteLength == 0);
        }

        for (usize i = 0; i < haystackByteLength; ++i)
        {
            if (i > 0)
            {
                uint8 const b = static_cast<uint8>(haystack[i]);
                if (isContinuation(b))
                {
                    continue;
                }
            }

            if (startsWithIgnoreCase(haystack + i, haystackByteLength - i, needle, needleByteLength))
            {
                return true;
            }
        }

        return false;
    }
}

#undef ARK_CONSTEXPR
