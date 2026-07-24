#include "Ark/Strings/UUID.hpp"
#include "Ark/Core/Hasher.hpp"

namespace Ark
{
    namespace
    {
        inline int hexValue(char c)
        {
            if (c >= '0' && c <= '9')
            {
                return c - '0';
            }
            if (c >= 'a' && c <= 'f')
            {
                return 10 + (c - 'a');
            }
            if (c >= 'A' && c <= 'F')
            {
                return 10 + (c - 'A');
            }
            return -1;
        }
    }

    UUID UUID::fromString(StringSlice text)
    {
        if (text.isEmpty())
        {
            return UUID();
        }

        char hex[32]{};
        usize count = 0;

        for (usize i = 0; i < text.getCount(); ++i)
        {
            char c = text[i];
            if (c == '-' || c == '{' || c == '}' || c == '(' || c == ')')
            {
                continue;
            }
            if (count < 32)
            {
                hex[count++] = c;
            }
            else
            {
                return UUID();
            }
        }

        if (count != 32)
        {
            return UUID();
        }

        Bytes out{};
        for (usize i = 0; i < 16; ++i)
        {
            int hi = hexValue(hex[i * 2]);
            int lo = hexValue(hex[i * 2 + 1]);
            if (hi < 0 || lo < 0)
            {
                return UUID();
            }
            out.asPointer()[i] = static_cast<uint8>((hi << 4) | lo);
        }

        return UUID(out);
    }

    String UUID::toString() const
    {
        auto const* b = data.asPointer();
        static char const* digits = "0123456789abcdef";

        String out;
        out.reserve(36);
        for (usize i = 0; i < 16; ++i)
        {
            out.append(digits[(b[i] >> 4) & 0xF]);
            out.append(digits[b[i] & 0xF]);
            if (i == 3 || i == 5 || i == 7 || i == 9)
            {
                out.append('-');
            }
        }
        return out;
    }

    UUID UUID::fromHash160(Collections::InlineArray<uint8, 20> const& digest)
    {
        Bytes out{};
        auto* dst = out.asPointer();
        auto const* src = digest.asPointer();
        for (usize i = 0; i < 16; ++i)
        {
            dst[i] = src[i];
        }

        // Set version (0101b = 5) and variant (10b)
        dst[6] = static_cast<uint8>((dst[6] & 0x0F) | 0x50);
        dst[8] = static_cast<uint8>((dst[8] & 0x3F) | 0x80);

        return UUID(out);
    }
}
