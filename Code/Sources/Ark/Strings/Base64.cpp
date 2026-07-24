#include "Ark/Strings/Base64.hpp"

namespace Ark::Base64
{
    namespace
    {
        constexpr char const* kBase64Table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        constexpr uint8 kInvalid = 0xFF;

        inline uint8 decodeChar(unsigned char c)
        {
            if (c >= 'A' && c <= 'Z')
                return static_cast<uint8>(c - 'A');
            if (c >= 'a' && c <= 'z')
                return static_cast<uint8>(c - 'a' + 26);
            if (c >= '0' && c <= '9')
                return static_cast<uint8>(c - '0' + 52);
            if (c == '+')
                return 62;
            if (c == '/')
                return 63;
            return kInvalid;
        }
    }

    String encode(StringSlice data)
    {
        String out;
        unsigned char a3[3];
        unsigned char a4[4];
        size_t i = 0;

        for (size_t idx = 0; idx < data.getLength(); ++idx)
        {
            a3[i++] = static_cast<unsigned char>(data[idx]);
            if (i == 3)
            {
                a4[0] = (a3[0] & 0xfc) >> 2;
                a4[1] = ((a3[0] & 0x03) << 4) + ((a3[1] & 0xf0) >> 4);
                a4[2] = ((a3[1] & 0x0f) << 2) + ((a3[2] & 0xc0) >> 6);
                a4[3] = (a3[2] & 0x3f);
                out.append(kBase64Table[a4[0]]);
                out.append(kBase64Table[a4[1]]);
                out.append(kBase64Table[a4[2]]);
                out.append(kBase64Table[a4[3]]);
                i = 0;
            }
        }

        if (i)
        {
            for (size_t j = i; j < 3; ++j)
            {
                a3[j] = 0;
            }
            a4[0] = (a3[0] & 0xfc) >> 2;
            a4[1] = ((a3[0] & 0x03) << 4) + ((a3[1] & 0xf0) >> 4);
            a4[2] = ((a3[1] & 0x0f) << 2) + ((a3[2] & 0xc0) >> 6);
            a4[3] = (a3[2] & 0x3f);
            out.append(kBase64Table[a4[0]]);
            out.append(kBase64Table[a4[1]]);
            if (i > 1)
                out.append(kBase64Table[a4[2]]);
            else
                out.append('=');
            if (i > 2)
                out.append(kBase64Table[a4[3]]);
            else
                out.append('=');
        }
        return out;
    }

    String encode(Collections::Array<uint8> const& bytes)
    {
        String out;
        unsigned char a3[3];
        unsigned char a4[4];
        size_t i = 0;

        for (size_t idx = 0; idx < bytes.getCount(); ++idx)
        {
            a3[i++] = bytes[idx];
            if (i == 3)
            {
                a4[0] = (a3[0] & 0xfc) >> 2;
                a4[1] = ((a3[0] & 0x03) << 4) + ((a3[1] & 0xf0) >> 4);
                a4[2] = ((a3[1] & 0x0f) << 2) + ((a3[2] & 0xc0) >> 6);
                a4[3] = (a3[2] & 0x3f);
                out.append(kBase64Table[a4[0]]);
                out.append(kBase64Table[a4[1]]);
                out.append(kBase64Table[a4[2]]);
                out.append(kBase64Table[a4[3]]);
                i = 0;
            }
        }

        if (i)
        {
            for (size_t j = i; j < 3; ++j)
            {
                a3[j] = 0;
            }
            a4[0] = (a3[0] & 0xfc) >> 2;
            a4[1] = ((a3[0] & 0x03) << 4) + ((a3[1] & 0xf0) >> 4);
            a4[2] = ((a3[1] & 0x0f) << 2) + ((a3[2] & 0xc0) >> 6);
            a4[3] = (a3[2] & 0x3f);
            out.append(kBase64Table[a4[0]]);
            out.append(kBase64Table[a4[1]]);
            if (i > 1)
                out.append(kBase64Table[a4[2]]);
            else
                out.append('=');
            if (i > 2)
                out.append(kBase64Table[a4[3]]);
            else
                out.append('=');
        }
        return out;
    }

    bool decode(StringSlice b64, Collections::Array<uint8>& outBytes)
    {
        outBytes.clear();
        uint8 quartet[4];
        int32 q = 0;
        int32 padInBlock = 0;
        bool sawPadding = false;

        auto flush = [&]() -> bool
        {
            if (q != 4)
            {
                return false;
            }
            if (padInBlock > 2)
            {
                return false;
            }

            uint8 a0 = quartet[0];
            uint8 a1 = quartet[1];
            uint8 a2 = quartet[2];
            uint8 a3 = quartet[3];

            uint8 b0 = static_cast<uint8>((a0 << 2) | ((a1 & 0x30) >> 4));
            uint8 b1 = static_cast<uint8>(((a1 & 0x0F) << 4) | ((a2 & 0x3C) >> 2));
            uint8 b2 = static_cast<uint8>(((a2 & 0x03) << 6) | a3);

            outBytes.append(b0);
            if (padInBlock <= 1)
            {
                outBytes.append(b1);
            }

            if (padInBlock == 0)
            {
                outBytes.append(b2);
            }
            q = 0;
            padInBlock = 0;
            return true;
        };

        for (size_t idx = 0; idx < b64.getLength(); ++idx)
        {
            char ch = b64[idx];
            if (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r')
            {
                continue;
            }

            if (sawPadding && ch != '=')
            {
                outBytes.clear();
                return false;
            }

            if (ch == '=')
            {
                if (q < 2)
                {
                    outBytes.clear();
                    return false;
                }
                quartet[q++] = 0;
                padInBlock++;
                sawPadding = true;
            }
            else
            {
                uint8 d = decodeChar(static_cast<unsigned char>(ch));
                if (d == kInvalid)
                {
                    outBytes.clear();
                    return false;
                }
                quartet[q++] = d;
            }

            if (q == 4)
            {
                if (!flush())
                {
                    outBytes.clear();
                    return false;
                }
            }
        }

        if (q != 0)
        {
            outBytes.clear();
            return false;
        }

        return true;
    }
}
