#pragma once

#include "Ark/Strings/String.hpp"
#include "Ark/Strings/StringSlice.hpp"

namespace Ark::AnsiConversions
{
    /// Helpers for ANSI string conversion entry points.
    /// Creates an owning `String` from a string slice.
    /// @param slice Source text view.
    /// @return A copied string.
    inline String toString(StringSlice slice)
    {
        return String(slice);
    }
}

#include "Ark/Strings/Platform/Generic/AnsiConversions.hpp"
