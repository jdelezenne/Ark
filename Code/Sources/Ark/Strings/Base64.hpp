#pragma once

#include "Ark/Collections/Array.hpp"
#include "Ark/Core/Types.hpp"
#include "Ark/Strings/String.hpp"
#include "Ark/Strings/StringSlice.hpp"

namespace Ark::Base64
{
    /// Encodes text bytes as Base64.
    /// @param data Input bytes represented as a string slice.
    /// @return Base64-encoded string.
    String encode(StringSlice data);

    /// Encodes a byte array as Base64.
    /// @param bytes Input bytes.
    /// @return Base64-encoded string.
    String encode(Collections::Array<uint8> const& bytes);

    /// Decodes a Base64 string into bytes.
    /// @param b64 Base64 input text.
    /// @param outBytes Destination byte array.
    /// @return `true` on successful decode.
    bool decode(StringSlice b64, Collections::Array<uint8>& outBytes);
}
