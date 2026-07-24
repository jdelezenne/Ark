#include "Ark/Cryptography/Md5.hpp"

#include <algorithm>
#include <cstring>
#include <iomanip>

namespace Ark::Cryptography
{
    namespace
    {
        inline uint32 leftRotate(uint32 value, uint32 bits)
        {
            return (value << bits) | (value >> (32 - bits));
        }

        constexpr uint32 S[64] = {
            7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21};

        constexpr uint32 K[64] = {
            0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee, 0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501, 0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be, 0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821, 0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa, 0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8, 0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed, 0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a, 0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c, 0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70, 0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05, 0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665, 0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039, 0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1, 0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1, 0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391};
    }

    Md5::Md5()
    {
        reset();
    }

    void Md5::reset()
    {
        a = 0x67452301u;
        b = 0xefcdab89u;
        c = 0x98badcfeu;
        d = 0x10325476u;
        totalBits = 0;
        bufferSize = 0;
        finalized = false;
        result.fill(0);
    }

    void Md5::update(void const* data, usize length)
    {
        auto const* bytes = static_cast<uint8 const*>(data);
        totalBits += static_cast<uint64>(length) * 8ULL;
        while (length > 0)
        {
            usize const toCopy = std::min<usize>(length, 64 - bufferSize);
            std::memcpy(buffer + bufferSize, bytes, toCopy);
            bufferSize += toCopy;
            bytes += toCopy;
            length -= toCopy;
            if (bufferSize == 64)
            {
                processBlock(buffer);
                bufferSize = 0;
            }
        }
    }

    void Md5::update(String const& data)
    {
        update(data.asPointer(), data.getCount());
    }

    void Md5::finalize()
    {
        if (finalized)
            return;
        uint8 const bit = 0x80u;
        update(&bit, 1);
        uint8 zero = 0;
        while (bufferSize != 56)
        {
            update(&zero, 1);
        }
        uint8 lengthBytes[8];
        uint64 bits = totalBits;
        for (int i = 0; i < 8; ++i)
        {
            lengthBytes[i] = static_cast<uint8>(bits & 0xFF);
            bits >>= 8;
        }
        update(lengthBytes, 8);
        finalized = true;
        auto writeWordLE = [&](uint32 value, int offset)
        {
            result[offset + 0] = static_cast<uint8>(value & 0xFF);
            result[offset + 1] = static_cast<uint8>((value >> 8) & 0xFF);
            result[offset + 2] = static_cast<uint8>((value >> 16) & 0xFF);
            result[offset + 3] = static_cast<uint8>((value >> 24) & 0xFF);
        };
        writeWordLE(a, 0);
        writeWordLE(b, 4);
        writeWordLE(c, 8);
        writeWordLE(d, 12);
    }

    Md5::Digest Md5::digest() const
    {
        return result;
    }

    String Md5::hexDigest() const
    {
        String hex;
        for (uint8 b8 : result)
        {
            hex.appendFormat("{:02x}", static_cast<int>(b8));
        }
        return hex;
    }

    void Md5::processBlock(uint8 const block[64])
    {
        uint32 M[16];
        for (int i = 0; i < 16; ++i)
        {
            M[i] = static_cast<uint32>(block[i * 4]) |
                   (static_cast<uint32>(block[i * 4 + 1]) << 8) |
                   (static_cast<uint32>(block[i * 4 + 2]) << 16) |
                   (static_cast<uint32>(block[i * 4 + 3]) << 24);
        }
        uint32 A = a, B = b, C = c, D = d;
        for (int i = 0; i < 64; ++i)
        {
            uint32 F;
            int g;
            if (i < 16)
            {
                F = (B & C) | ((~B) & D);
                g = i;
            }
            else if (i < 32)
            {
                F = (D & B) | ((~D) & C);
                g = (5 * i + 1) & 15;
            }
            else if (i < 48)
            {
                F = B ^ C ^ D;
                g = (3 * i + 5) & 15;
            }
            else
            {
                F = C ^ (B | (~D));
                g = (7 * i) & 15;
            }
            uint32 const temp = D;
            D = C;
            C = B;
            B = B + leftRotate(A + F + K[i] + M[g], S[i]);
            A = temp;
        }
        a += A;
        b += B;
        c += C;
        d += D;
    }

    Md5::Digest Md5::compute(void const* data, usize length)
    {
        Md5 m;
        m.update(data, length);
        m.finalize();
        return m.digest();
    }

    Md5::Digest Md5::compute(String const& data)
    {
        return compute(data.asPointer(), data.getCount());
    }

    String Md5::hex(String const& data)
    {
        Md5 m;
        m.update(data);
        m.finalize();
        return m.hexDigest();
    }
}
