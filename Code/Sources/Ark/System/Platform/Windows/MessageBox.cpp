// Only compile on Windows
#if defined(ARK_PLATFORM_WINDOWS)

#include "Ark/System/MessageBox.hpp"
#include "Ark/Core/Result.hpp"
#include "Ark/Strings/StringSlice.hpp"
#include "Ark/Strings/Unicode.hpp"

#include <windows.h>

namespace Ark::System
{
    static UINT getIconFlag(MessageBoxType type)
    {
        switch (type)
        {
            case MessageBoxType::Information:
                return MB_ICONINFORMATION;
            case MessageBoxType::Warning:
                return MB_ICONWARNING;
            case MessageBoxType::Error:
                return MB_ICONERROR;
        }

        return MB_ICONINFORMATION;
    }

    static UINT getButtonsFlag(MessageBoxButtons buttons)
    {
        switch (buttons)
        {
            case MessageBoxButtons::Ok:
                return MB_OK;
            case MessageBoxButtons::OkCancel:
                return MB_OKCANCEL;
            case MessageBoxButtons::YesNo:
                return MB_YESNO;
            case MessageBoxButtons::YesNoCancel:
                return MB_YESNOCANCEL;
        }

        return MB_OK;
    }

    Ark::Result<MessageBoxButton, Ark::String> showMessageBox(Ark::StringSlice title, Ark::StringSlice message, MessageBoxOptions const& options)
    {
        auto const wTitle = Unicode::toWide(title);
        auto const wMessage = Unicode::toWide(message);

        UINT const style = getIconFlag(options.type) | getButtonsFlag(options.buttons);
        HWND owner = reinterpret_cast<HWND>(options.owner);
        int const result = MessageBoxW(owner, wMessage.asPointer(), wTitle.asPointer(), style | MB_TASKMODAL);

        switch (result)
        {
            case IDOK:
                return Ark::Result<MessageBoxButton, Ark::String>(MessageBoxButton::Ok);
            case IDCANCEL:
                return Ark::Result<MessageBoxButton, Ark::String>(MessageBoxButton::Cancel);
            case IDYES:
                return Ark::Result<MessageBoxButton, Ark::String>(MessageBoxButton::Yes);
            case IDNO:
                return Ark::Result<MessageBoxButton, Ark::String>(MessageBoxButton::No);
            default:
                return Ark::Result<MessageBoxButton, Ark::String>(Ark::unexpectedResult, Ark::String("MessageBox failed"));
        }
    }
}

#endif
