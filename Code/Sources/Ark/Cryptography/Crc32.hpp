#pragma once

#include "Ark/Core/Types.hpp"

namespace Ark::Cryptography
{
    /// Computes CRC32 checksum of a memory buffer.
    /// Intended for error detection, not cryptographic security.
    /// @param data Input buffer.
    /// @param size Buffer size in bytes.
    /// @return 32-bit CRC32 value.
    uint32 crc32Compute(uint8 const* data, usize size);
}
