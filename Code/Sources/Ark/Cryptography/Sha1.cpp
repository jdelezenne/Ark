#include "Ark/Cryptography/Sha1.hpp"

#include <iomanip>

namespace Ark::Cryptography
{
    namespace
    {
        inline uint32 leftRotate(uint32 value, uint32 bits)
        {
            return (value << bits) | (value >> (32 - bits));
        }

        inline void writeBigEndian64(uint8 out[8], uint64 value)
        {
            for (int i = 7; i >= 0; --i)
            {
                out[i] = static_cast<uint8>(value & 0xFF);
                value >>= 8;
            }
        }
    }

    Sha1::Sha1()
    {
        reset();
    }

    void Sha1::reset()
    {
        h0 = 0x67452301u;
        h1 = 0xEFCDAB89u;
        h2 = 0x98BADCFEu;
        h3 = 0x10325476u;
        h4 = 0xC3D2E1F0u;
        totalBits = 0;
        bufferSize = 0;
        finalized = false;
        result.fill(0);
    }

    void Sha1::update(void const* data, usize length)
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

    void Sha1::update(String const& data)
    {
        update(data.asPointer(), data.getCount());
    }

    void Sha1::finalize()
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
        writeBigEndian64(lengthBytes, totalBits);
        update(lengthBytes, 8);
        finalized = true;
        result[0] = static_cast<uint8>((h0 >> 24) & 0xFF);
        result[1] = static_cast<uint8>((h0 >> 16) & 0xFF);
        result[2] = static_cast<uint8>((h0 >> 8) & 0xFF);
        result[3] = static_cast<uint8>(h0 & 0xFF);
        result[4] = static_cast<uint8>((h1 >> 24) & 0xFF);
        result[5] = static_cast<uint8>((h1 >> 16) & 0xFF);
        result[6] = static_cast<uint8>((h1 >> 8) & 0xFF);
        result[7] = static_cast<uint8>(h1 & 0xFF);
        result[8] = static_cast<uint8>((h2 >> 24) & 0xFF);
        result[9] = static_cast<uint8>((h2 >> 16) & 0xFF);
        result[10] = static_cast<uint8>((h2 >> 8) & 0xFF);
        result[11] = static_cast<uint8>(h2 & 0xFF);
        result[12] = static_cast<uint8>((h3 >> 24) & 0xFF);
        result[13] = static_cast<uint8>((h3 >> 16) & 0xFF);
        result[14] = static_cast<uint8>((h3 >> 8) & 0xFF);
        result[15] = static_cast<uint8>(h3 & 0xFF);
        result[16] = static_cast<uint8>((h4 >> 24) & 0xFF);
        result[17] = static_cast<uint8>((h4 >> 16) & 0xFF);
        result[18] = static_cast<uint8>((h4 >> 8) & 0xFF);
        result[19] = static_cast<uint8>(h4 & 0xFF);
    }

    Sha1::Digest Sha1::digest() const
    {
        return result;
    }

    String Sha1::hexDigest() const
    {
        String hex;
        for (uint8 b : result)
        {
            hex.appendFormat("{:02x}", static_cast<int>(b));
        }
        return hex;
    }

    void Sha1::processBlock(uint8 const block[64])
    {
        uint32 w[80];
        for (int i = 0; i < 16; ++i)
        {
            w[i] = (static_cast<uint32>(block[i * 4]) << 24) |
                   (static_cast<uint32>(block[i * 4 + 1]) << 16) |
                   (static_cast<uint32>(block[i * 4 + 2]) << 8) |
                   (static_cast<uint32>(block[i * 4 + 3]));
        }
        for (int i = 16; i < 80; ++i)
        {
            w[i] = leftRotate(w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16], 1);
        }
        uint32 a = h0, b = h1, c = h2, d = h3, e = h4;
        for (int i = 0; i < 80; ++i)
        {
            uint32 f, k;
            if (i < 20)
            {
                f = (b & c) | ((~b) & d);
                k = 0x5A827999u;
            }
            else if (i < 40)
            {
                f = b ^ c ^ d;
                k = 0x6ED9EBA1u;
            }
            else if (i < 60)
            {
                f = (b & c) | (b & d) | (c & d);
                k = 0x8F1BBCDCu;
            }
            else
            {
                f = b ^ c ^ d;
                k = 0xCA62C1D6u;
            }
            uint32 const temp = leftRotate(a, 5) + f + e + k + w[i];
            e = d;
            d = c;
            c = leftRotate(b, 30);
            b = a;
            a = temp;
        }
        h0 += a;
        h1 += b;
        h2 += c;
        h3 += d;
        h4 += e;
    }

    Sha1::Digest Sha1::compute(void const* data, usize length)
    {
        Sha1 result;
        result.update(data, length);
        result.finalize();
        return result.digest();
    }

    Sha1::Digest Sha1::compute(String const& data)
    {
        return compute(data.asPointer(), data.getCount());
    }

    String Sha1::hex(String const& data)
    {
        Sha1 result;
        result.update(data);
        result.finalize();
        return result.hexDigest();
    }
}
