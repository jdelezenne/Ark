#pragma once

#include "Ark/Core/Option.hpp"
#include "Ark/Core/Pair.hpp"
#include "Ark/Core/Types.hpp"

namespace Ark
{
    struct StringSlice;
}

namespace Ark::Utf8
{
    /// Validate that the byte sequence is well-formed UTF-8 (no overlongs, valid ranges).
    /// @param bytes Pointer to the byte sequence.
    /// @param length Number of bytes to validate.
    bool isValid(char const* bytes, usize length);

    /// Validate that the slice is well-formed UTF-8 (no overlongs, valid ranges).
    bool isValid(StringSlice bytes);

    /// Try decode one codepoint starting at byte index. Returns (codepoint, nextIndex) on success.
    /// On error, returns none.
    Option<Pair<UnicodeChar, usize>> decodeAt(StringSlice bytes, usize index);

    /// Advance to next char boundary at or after index. Returns none if index > size.
    Option<usize> nextCharBoundary(StringSlice bytes, usize index);

    /// Retreat to previous char boundary at or before index. Returns none if index > size.
    Option<usize> prevCharBoundary(StringSlice bytes, usize index);

    /// Find first occurrence of a Unicode codepoint. Returns byte index.
    Option<usize> findFirstCodepoint(StringSlice haystack, UnicodeChar codepoint);
}
