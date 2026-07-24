#include "Ark/System/MessageBox.hpp"
#include "Ark/Strings/String.hpp"
#include "Ark/Strings/StringSlice.hpp"

namespace Ark::System
{
    Result<MessageBoxButton, String> showMessageBox(MessageBoxOptions const& options)
    {
        StringSlice const titleSlice = (options.title != nullptr) ? StringSlice(options.title)
                                                                  : StringSlice("");
        StringSlice const messageSlice = (options.message != nullptr) ? StringSlice(options.message)
                                                                      : StringSlice("");

        return showMessageBox(titleSlice, messageSlice, options);
    }

    Result<MessageBoxButton, String> showMessageBox(StringSlice title, StringSlice message, MessageBoxType type)
    {
        MessageBoxOptions opts;
        opts.title = title.asPointer();
        opts.message = message.asPointer();
        opts.type = type;
        return showMessageBox(title, message, opts);
    }
}
