#pragma once

#include "Ark/Core/Result.hpp"
#include "Ark/Storage/Path.hpp"
#include "Ark/Strings/String.hpp"
#include "Ark/Strings/StringSlice.hpp"

namespace Ark::System
{
    /// File filter entry used by file dialogs.
    struct FileDialogFilter final
    {
        /// Display name shown in the dialog.
        StringSlice name;
        /// Filter pattern (for example: "png", "jpg;jpeg", "*").
        StringSlice pattern; // e.g. "png" or "jpg;jpeg" or "*"
    };

    /// Options shared by open/save/select-folder dialogs.
    struct FileDialogOptions final
    {
        /// Dialog title.
        StringSlice title;
        /// Initial directory.
        Storage::Path defaultDirectory;
        /// Suggested default file name.
        StringSlice defaultName;
        /// Optional native owner window pointer.
        void* owner = nullptr; // HWND on Windows, NSWindow* on macOS
        /// Whether multi-selection is allowed (when supported).
        bool allowMultiple = false;
        /// Whether hidden files are shown.
        bool showHidden = false;
        /// Optional filter array.
        FileDialogFilter const* filters = nullptr;
        /// Number of filter entries.
        usize filterCount = 0;
    };

    /// Shows an "open file" dialog.
    /// @param options Dialog options.
    /// @return Selected path or an error string.
    Result<Storage::Path, String> showOpenFileDialog(FileDialogOptions const& options);

    /// Shows a "save file" dialog.
    /// @param options Dialog options.
    /// @return Selected path or an error string.
    Result<Storage::Path, String> showSaveFileDialog(FileDialogOptions const& options);

    /// Shows a "select folder" dialog.
    /// @param options Dialog options.
    /// @return Selected folder path or an error string.
    Result<Storage::Path, String> showSelectFolderDialog(FileDialogOptions const& options);
}
