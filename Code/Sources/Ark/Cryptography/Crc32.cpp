#include "Ark/Cryptography/Crc32.hpp"

#if __has_include(<zlib.h>)
#include <zlib.h>
#define ARK_HAVE_ZLIB 1
#endif

namespace Ark::Cryptography
{
    uint32 crc32Compute(uint8 const* data, usize size)
    {
#if ARK_HAVE_ZLIB
        unsigned long crc = ::crc32(0L, Z_NULL, 0);
        crc = ::crc32(crc, reinterpret_cast<const Bytef*>(data), static_cast<uInt>(size));
        return static_cast<uint32>(crc);
#else
        // Fallback simple CRC32 (polynomial 0xEDB88320)
        uint32 crc = 0xFFFFFFFFu;
        for (usize i = 0; i < size; ++i)
        {
            crc ^= data[i];
            for (int k = 0; k < 8; ++k)
            {
                uint32 mask = -(crc & 1u);
                crc = (crc >> 1) ^ (0xEDB88320u & mask);
            }
        }
        return ~crc;
#endif
    }
}
