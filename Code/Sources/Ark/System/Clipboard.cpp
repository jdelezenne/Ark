#include "Ark/System/Clipboard.hpp"
#include "Ark/Strings/String.hpp"
#include "Ark/Strings/StringSlice.hpp"

#include "Ark/Core/Platform.hpp"

#include "Ark/Core/Platform.hpp"

namespace Ark::System::Clipboard
{
#if !defined(ARK_PLATFORM_WINDOWS) && !defined(ARK_PLATFORM_MACOS) && !defined(ARK_PLATFORM_LINUX)
    bool setText(Ark::StringSlice)
    {
        return false;
    }
    Ark::String getText()
    {
        return Ark::String("");
    }
    bool hasText()
    {
        return false;
    }
    bool setPrimarySelectionText(Ark::StringSlice)
    {
        return false;
    }
    Ark::String getPrimarySelectionText()
    {
        return Ark::String("");
    }
    bool hasPrimarySelectionText()
    {
        return false;
    }
    uint64 getSequenceNumber()
    {
        return 0;
    }
#endif
}
