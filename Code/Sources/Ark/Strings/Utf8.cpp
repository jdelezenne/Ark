#include "Ark/Strings/Utf8.hpp"
#include "Ark/Strings/StringSlice.hpp"

namespace Ark::Utf8
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
    }

    bool isValid(char const* bytes, usize length)
    {
        if (bytes == nullptr && length > 0)
        {
            return false;
        }

        usize index = 0;

        while (index < length)
        {
            uint8 const b0 = static_cast<uint8>(bytes[index]);
            usize expected = 0;
            uint32 codepoint = decodeLeading(b0, expected);

            if (expected == static_cast<usize>(-1))
            {
                return false;
            }

            if (expected == 0)
            {
                ++index;
                continue;
            }

            if (index + expected >= length)
            {
                return false;
            }

            for (usize k = 0; k < expected; ++k)
            {
                uint8 const bx = static_cast<uint8>(bytes[index + 1 + k]);

                if (!isContinuation(bx))
                {
                    return false;
                }

                codepoint = (codepoint << 6) | (bx & 0x3Fu);
            }

            usize const totalBytes = expected + 1;

            if (isOverlong(codepoint, totalBytes))
            {
                return false;
            }

            if (codepoint > 0x10FFFFu || (codepoint >= 0xD800u && codepoint <= 0xDFFFu))
            {
                return false;
            }

            index += totalBytes;
        }

        return true;
    }

    bool isValid(StringSlice bytes)
    {
        return isValid(bytes.asPointer(), bytes.getCount());
    }

    Option<Pair<UnicodeChar, usize>> decodeAt(StringSlice bytes, usize index)
    {
        if (index >= bytes.getCount())
        {
            return none;
        }

        uint8 const firstByte = static_cast<uint8>(bytes[index]);
        usize expected = 0;
        uint32 codepoint = decodeLeading(firstByte, expected);
        if (expected == static_cast<usize>(-1))
        {
            return none;
        }

        if (expected == 0)
        {
            return makeOption<Pair<UnicodeChar, usize>>(static_cast<UnicodeChar>(codepoint), index + 1);
        }

        if (index + expected >= bytes.getCount())
        {
            return none;
        }

        for (usize k = 0; k < expected; ++k)
        {
            uint8 const currentByte = static_cast<uint8>(bytes[index + 1 + k]);
            if (!isContinuation(currentByte))
            {
                return none;
            }

            codepoint = (codepoint << 6) | (currentByte & 0x3Fu);
        }

        usize const totalBytes = expected + 1;

        if (isOverlong(codepoint, totalBytes))
        {
            return none;
        }

        if (codepoint > 0x10FFFFu || (codepoint >= 0xD800u && codepoint <= 0xDFFFu))
        {
            return none;
        }

        return makeOption<Pair<UnicodeChar, usize>>(static_cast<UnicodeChar>(codepoint), index + totalBytes);
    }

    Option<usize> nextCharBoundary(StringSlice bytes, usize index)
    {
        if (index > bytes.getCount())
        {
            return none;
        }

        if (index == bytes.getCount())
        {
            return index;
        }

        // If at lead byte or ASCII, ok; otherwise advance until we hit a non-continuation.
        for (usize i = index; i < bytes.getCount(); ++i)
        {
            uint8 const b = static_cast<uint8>(bytes[i]);
            if ((b & 0xC0u) != 0x80u)
            {
                return i;
            }
        }

        return none;
    }

    Option<usize> prevCharBoundary(StringSlice bytes, usize index)
    {
        if (index > bytes.getCount())
        {
            return none;
        }

        if (index == 0)
        {
            return 0u;
        }

        for (usize i = index; i-- > 0;)
        {
            uint8 const b = static_cast<uint8>(bytes[i]);
            if ((b & 0xC0u) != 0x80u)
            {
                return i;
            }
        }

        return 0u;
    }

    Option<usize> findFirstCodepoint(StringSlice haystack, UnicodeChar codepoint)
    {
        usize index = 0;
        while (index < haystack.getCount())
        {
            auto value = decodeAt(haystack, index);

            if (!value)
            {
                return none;
            }

            if (value->first == codepoint)
            {
                return index;
            }

            index = value->second;
        }

        return none;
    }
}
