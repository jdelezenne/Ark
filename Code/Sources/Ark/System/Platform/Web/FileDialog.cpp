#if defined(ARK_PLATFORM_WEBASSEMBLY)

#include "Ark/System/FileDialog.hpp"

namespace Ark::System
{
    Result<Storage::Path, String> showOpenFileDialog(FileDialogOptions const&)
    {
        return Result<Storage::Path, String>(Ark::unexpectedResult, String("File dialogs are not available on WebAssembly"));
    }

    Result<Storage::Path, String> showSaveFileDialog(FileDialogOptions const&)
    {
        return Result<Storage::Path, String>(Ark::unexpectedResult, String("File dialogs are not available on WebAssembly"));
    }

    Result<Storage::Path, String> showSelectFolderDialog(FileDialogOptions const&)
    {
        return Result<Storage::Path, String>(Ark::unexpectedResult, String("File dialogs are not available on WebAssembly"));
    }
}

#endif
