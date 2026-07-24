#pragma once

#include "Ark/Core/Types.hpp"
#include "Ark/Strings/String.hpp"
#include "Ark/Strings/StringSlice.hpp"

namespace Ark::System::Clipboard
{
    /// Put UTF-8 text into the clipboard.
    bool setText(Ark::StringSlice text);

    /// Get UTF-8 text from the clipboard. Returns empty string on failure or if no text.
    Ark::String getText();

    /// Whether the clipboard currently contains non-empty text.
    bool hasText();

    /// Put UTF-8 text into the primary selection (if unsupported by the OS, stored process-local).
    bool setPrimarySelectionText(Ark::StringSlice text);

    /// Get UTF-8 text from the primary selection.
    Ark::String getPrimarySelectionText();

    /// Whether the primary selection contains non-empty text.
    bool hasPrimarySelectionText();

    /// Monotonically increasing sequence that changes when the clipboard changes (0 if unsupported).
    uint64 getSequenceNumber();
}
