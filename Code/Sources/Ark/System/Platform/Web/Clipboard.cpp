#if defined(ARK_PLATFORM_WEBASSEMBLY)

#include "Ark/System/Clipboard.hpp"

#include <emscripten/emscripten.h>

namespace Ark::System::Clipboard
{
    namespace
    {
        static uint64 gSequence = 0;
        static Ark::String gText;
        static Ark::String gPrimarySelectionText;
    }

    bool setText(Ark::StringSlice text)
    {
        gText = Ark::String(text);
        ++gSequence;

        EM_ASM({
            var value = UTF8ToString($0);
            if (navigator.clipboard && navigator.clipboard.writeText)
            {
                navigator.clipboard.writeText(value);
            }
        }, gText.asPointer());
        return true;
    }

    Ark::String getText()
    {
        // Synchronous clipboard read is restricted in browsers; return last locally set value.
        return gText;
    }

    bool hasText()
    {
        return !gText.isEmpty();
    }

    bool setPrimarySelectionText(Ark::StringSlice text)
    {
        gPrimarySelectionText = Ark::String(text);
        ++gSequence;
        return true;
    }

    Ark::String getPrimarySelectionText()
    {
        return gPrimarySelectionText;
    }

    bool hasPrimarySelectionText()
    {
        return !gPrimarySelectionText.isEmpty();
    }

    uint64 getSequenceNumber()
    {
        return gSequence;
    }
}

#endif
