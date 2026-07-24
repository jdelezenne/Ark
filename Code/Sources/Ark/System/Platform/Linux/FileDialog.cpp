#if defined(ARK_PLATFORM_LINUX) && !defined(ARK_PLATFORM_WEBASSEMBLY)

#include "Ark/System/FileDialog.hpp"

#include "Ark/Storage/Path.hpp"
#include "Ark/Strings/String.hpp"

#include <cstdio>
#include <cstring>

namespace Ark::System
{
    namespace
    {
        static Result<Storage::Path, String> runZenity(char const* mode, FileDialogOptions const& options)
        {
            String command;
            command.append("zenity --file-selection ");
            command.append(mode);
            if (!options.title.isEmpty())
            {
                command.append(" --title=\"");
                command.append(options.title);
                command.append("\"");
            }
            if (!options.defaultDirectory.isEmpty())
            {
                command.append(" --filename=\"");
                command.append(options.defaultDirectory.string());
                if (!options.defaultName.isEmpty())
                {
                    command.append("/");
                    command.append(options.defaultName);
                }
                command.append("\"");
            }
            else if (!options.defaultName.isEmpty())
            {
                command.append(" --filename=\"");
                command.append(options.defaultName);
                command.append("\"");
            }
            command.append(" 2>/dev/null");

            FILE* pipe = popen(command.asPointer(), "r");
            if (pipe == nullptr)
            {
                return Result<Storage::Path, String>(Ark::unexpectedResult, String("Failed to launch file dialog (zenity)"));
            }

            char buffer[4096] = {};
            if (std::fgets(buffer, sizeof(buffer), pipe) == nullptr)
            {
                pclose(pipe);
                return Result<Storage::Path, String>(Ark::unexpectedResult, String("File dialog cancelled"));
            }
            int const status = pclose(pipe);
            if (status != 0)
            {
                return Result<Storage::Path, String>(Ark::unexpectedResult, String("File dialog cancelled"));
            }

            usize length = std::strlen(buffer);
            while (length > 0 && (buffer[length - 1] == '\n' || buffer[length - 1] == '\r'))
            {
                buffer[--length] = '\0';
            }
            if (length == 0)
            {
                return Result<Storage::Path, String>(Ark::unexpectedResult, String("File dialog cancelled"));
            }
            return Storage::Path(String(buffer));
        }
    }

    Result<Storage::Path, String> showOpenFileDialog(FileDialogOptions const& options)
    {
        return runZenity("", options);
    }

    Result<Storage::Path, String> showSaveFileDialog(FileDialogOptions const& options)
    {
        return runZenity("--save --confirm-overwrite", options);
    }

    Result<Storage::Path, String> showSelectFolderDialog(FileDialogOptions const& options)
    {
        return runZenity("--directory", options);
    }
}

#endif
