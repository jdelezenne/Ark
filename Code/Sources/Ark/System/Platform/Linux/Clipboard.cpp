#if defined(ARK_PLATFORM_LINUX) && !defined(ARK_PLATFORM_WEBASSEMBLY)

#include "Ark/System/Clipboard.hpp"
#include "Ark/Core/Types.hpp"
#include "Ark/Strings/String.hpp"
#include "Ark/Strings/StringSlice.hpp"

#include <X11/Xatom.h>
#include <X11/Xlib.h>

#include <cstring>

namespace Ark::System::Clipboard
{
    namespace
    {
        static uint64 gSequence = 0;
        static Ark::String gText;
        static Ark::String gPrimarySelectionText;

        static ::Display* getDisplay()
        {
            static ::Display* display = XOpenDisplay(nullptr);
            return display;
        }

        static void storeCutBuffer(Ark::StringSlice text)
        {
            ::Display* display = getDisplay();
            if (display == nullptr || text.isEmpty())
            {
                return;
            }
            XStoreBytes(display, text.asPointer(), static_cast<int>(text.getLength()));
            XFlush(display);
        }
    }

    bool setText(Ark::StringSlice text)
    {
        gText = Ark::String(text);
        storeCutBuffer(text);
        ++gSequence;
        return true;
    }

    Ark::String getText()
    {
        if (!gText.isEmpty())
        {
            return gText;
        }

        ::Display* display = getDisplay();
        if (display == nullptr)
        {
            return {};
        }

        int length = 0;
        char* bytes = XFetchBytes(display, &length);
        if (bytes == nullptr || length <= 0)
        {
            return {};
        }

        gText = Ark::String(bytes, static_cast<Ark::String::SizeType>(length));
        XFree(bytes);
        return gText;
    }

    bool hasText()
    {
        return !getText().isEmpty();
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
