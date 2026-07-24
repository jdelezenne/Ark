#pragma once

#include "Ark/Core/Result.hpp"
#include "Ark/Strings/String.hpp"
#include "Ark/Strings/StringSlice.hpp"

namespace Ark::System
{
    /// Message box type/icon displayed to the user.
    enum class MessageBoxType
    {
        Information, ///< Informational icon
        Warning,     ///< Warning icon
        Error,       ///< Error icon
    };

    /// Individual button selected by the user.
    enum class MessageBoxButton
    {
        Ok,     ///< OK button
        Cancel, ///< Cancel button
        Yes,    ///< Yes button
        No,     ///< No button
    };

    /// Button set available in the message box.
    enum class MessageBoxButtons
    {
        Ok,          ///< Single OK button
        OkCancel,    ///< OK and Cancel buttons
        YesNo,       ///< Yes and No buttons
        YesNoCancel, ///< Yes, No, and Cancel buttons
    };

    /// Options controlling message box appearance and behavior.
    struct MessageBoxOptions final
    {
        /// Dialog title text.
        char const* title = nullptr;
        /// Dialog message body text.
        char const* message = nullptr;
        /// Icon type displayed in the dialog.
        MessageBoxType type = MessageBoxType::Information;
        /// Button set shown in the dialog.
        MessageBoxButtons buttons = MessageBoxButtons::Ok;
        /// Optional native owner window pointer (HWND on Windows, NSWindow* on macOS).
        void* owner = nullptr;
    };

    /// Show a native system message box/alert and return the selected button.
    /// Buttons order is platform-appropriate; if none provided, defaults to { Ok }.
    Result<MessageBoxButton, String> showMessageBox(StringSlice title, StringSlice message, MessageBoxOptions const& options = {});

    Result<MessageBoxButton, String> showMessageBox(MessageBoxOptions const& options);

    Result<MessageBoxButton, String> showMessageBox(StringSlice title, StringSlice message, MessageBoxType type);
}
