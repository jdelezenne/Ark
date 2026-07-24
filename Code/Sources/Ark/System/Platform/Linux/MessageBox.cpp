#if defined(ARK_PLATFORM_LINUX) && !defined(ARK_PLATFORM_WEBASSEMBLY)

#include "Ark/System/MessageBox.hpp"

#include <cstdlib>
#include <sys/wait.h>

namespace Ark::System
{
    namespace
    {
        static char const* typeFlag(MessageBoxType type)
        {
            switch (type)
            {
                case MessageBoxType::Warning:
                    return "--warning";
                case MessageBoxType::Error:
                    return "--error";
                case MessageBoxType::Information:
                default:
                    return "--info";
            }
        }
    }

    Result<MessageBoxButton, String> showMessageBox(StringSlice title, StringSlice message, MessageBoxOptions const& options)
    {
        String command;
        command.append("zenity ");
        command.append(typeFlag(options.type));
        command.append(" --title=\"");
        command.append(title);
        command.append("\" --text=\"");
        command.append(message);
        command.append("\"");

        if (options.buttons == MessageBoxButtons::OkCancel)
        {
            command.append(" --ok-label=OK --cancel-label=Cancel");
        }
        else if (options.buttons == MessageBoxButtons::YesNo)
        {
            command.append(" --question --ok-label=Yes --cancel-label=No");
        }
        else if (options.buttons == MessageBoxButtons::YesNoCancel)
        {
            command.append(" --question --ok-label=Yes --cancel-label=Cancel --extra-button=No");
        }

        command.append(" 2>/dev/null");

        int status = std::system(command.asPointer());
        if (status == -1)
        {
            return Result<MessageBoxButton, String>(Ark::unexpectedResult, String("Failed to launch message box dialog"));
        }

        int const exitCode = WEXITSTATUS(status);
        switch (options.buttons)
        {
            case MessageBoxButtons::OkCancel:
                return (exitCode == 0) ? MessageBoxButton::Ok : MessageBoxButton::Cancel;
            case MessageBoxButtons::YesNo:
                return (exitCode == 0) ? MessageBoxButton::Yes : MessageBoxButton::No;
            case MessageBoxButtons::YesNoCancel:
                return (exitCode == 0) ? MessageBoxButton::Yes : MessageBoxButton::Cancel;
            case MessageBoxButtons::Ok:
            default:
                return MessageBoxButton::Ok;
        }
    }
}

#endif
