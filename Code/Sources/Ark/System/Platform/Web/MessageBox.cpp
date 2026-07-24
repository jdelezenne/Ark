#if defined(ARK_PLATFORM_WEBASSEMBLY)

#include "Ark/System/MessageBox.hpp"

#include <emscripten/emscripten.h>

namespace Ark::System
{
    Result<MessageBoxButton, String> showMessageBox(StringSlice title, StringSlice message, MessageBoxOptions const& options)
    {
        String combined;
        if (!title.isEmpty())
        {
            combined.append(title);
            combined.append("\n\n");
        }
        combined.append(message);

        int result = 0;
        if (options.buttons == MessageBoxButtons::Ok)
        {
            EM_ASM({
                alert(UTF8ToString($0));
            }, combined.asPointer());
            return MessageBoxButton::Ok;
        }

        result = EM_ASM_INT({
            return confirm(UTF8ToString($0)) ? 1 : 0;
        }, combined.asPointer());

        if (options.buttons == MessageBoxButtons::OkCancel)
        {
            return (result != 0) ? MessageBoxButton::Ok : MessageBoxButton::Cancel;
        }
        if (options.buttons == MessageBoxButtons::YesNo || options.buttons == MessageBoxButtons::YesNoCancel)
        {
            return (result != 0) ? MessageBoxButton::Yes : MessageBoxButton::No;
        }
        return MessageBoxButton::Ok;
    }
}

#endif
