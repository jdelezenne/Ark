#pragma once

#include "Ark/Collections/Array.hpp"
#include "Ark/Core/Types.hpp"
#include "Ark/Strings/String.hpp"

namespace Ark::Strings
{
    /// Converts CP437 (DOS code page 437) encoded bytes to UTF-8.
    /// @param inputData The CP437 encoded input data.
    /// @return A String containing the UTF-8 encoded result.
    String convertCp437ToUtf8(Collections::Array<uint8> const& inputData);
}
