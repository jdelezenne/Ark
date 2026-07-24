// Ark::System::Clipboard implementation for Windows
#if defined(ARK_PLATFORM_WINDOWS)

#include "Ark/System/Clipboard.hpp"
#include "Ark/Core/Types.hpp"
#include "Ark/Strings/String.hpp"
#include "Ark/Strings/StringSlice.hpp"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace Ark::System::Clipboard
{
    static uint64 gSequence = 0;
    static Ark::String gPrimarySelectionText;

    bool setText(Ark::StringSlice text)
    {
        if (!OpenClipboard(nullptr))
        {
            return false;
        }
        EmptyClipboard();

        size_t const len = text.getCount();
        HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, (len + 1));
        if (!hMem)
        {
            CloseClipboard();
            return false;
        }
        void* ptr = GlobalLock(hMem);
        if (!ptr)
        {
            GlobalFree(hMem);
            CloseClipboard();
            return false;
        }
        memcpy(ptr, text.asPointer(), len);
        static_cast<char*>(ptr)[len] = '\0';
        GlobalUnlock(hMem);
        SetClipboardData(CF_TEXT, hMem);
        CloseClipboard();
        ++gSequence;
        return true;
    }

    Ark::String getText()
    {
        if (!OpenClipboard(nullptr))
        {
            return Ark::String("");
        }
        HANDLE hData = GetClipboardData(CF_TEXT);
        if (!hData)
        {
            CloseClipboard();
            return Ark::String("");
        }
        char* pszText = static_cast<char*>(GlobalLock(hData));
        if (!pszText)
        {
            CloseClipboard();
            return Ark::String("");
        }
        Ark::String out(pszText);
        GlobalUnlock(hData);
        CloseClipboard();
        return out;
    }

    bool hasText()
    {
        return IsClipboardFormatAvailable(CF_TEXT) != 0;
    }

    bool setPrimarySelectionText(Ark::StringSlice text)
    {
        gPrimarySelectionText = Ark::String(text.asPointer());
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
