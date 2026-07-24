#pragma once

#include "Ark/Collections/Array.hpp"
#include "Ark/Strings/String.hpp"
#include "Ark/Strings/StringSlice.hpp"

namespace Ark::Unicode
{
    /// Convert a wide-character null-terminated string to UTF-8 `String`.
    String fromWide(wchar_t const* wideString);

    /// Convert a UTF-8 string slice to a wide-character array with a null terminator.
    Collections::Array<wchar_t> toWide(StringSlice utf8String);
}
