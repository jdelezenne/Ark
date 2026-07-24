#include "Ark/System/Console.hpp"
#include "Ark/System/EntryPoint.hpp"
#include "Ark/System/MessageBox.hpp"

bool arkMain(Ark::Collections::Array<Ark::String> const&)
{
    using Ark::System::Console;
    using Ark::System::MessageBoxButton;
    using Ark::System::MessageBoxButtons;
    using Ark::System::MessageBoxOptions;
    using Ark::System::MessageBoxType;

    // Simple convenience overload
    auto r1 = Ark::System::showMessageBox("Hello", "Operation completed.", MessageBoxType::Information);
    if (r1)
    {
        Console::println("[MessageBox] OK pressed (simple)");
    }
    else
    {
        Console::println("[MessageBox] failed (simple)");
    }

    // Full options
    MessageBoxOptions opts;
    opts.title = "Confirm";
    opts.message = "Quit without saving?";
    opts.type = MessageBoxType::Warning;
    opts.buttons = MessageBoxButtons::YesNoCancel;

    auto r2 = Ark::System::showMessageBox(opts.title, opts.message, opts);
    if (r2)
    {
        switch (*r2)
        {
            case MessageBoxButton::Yes:
                Console::println("[MessageBox] Yes");
                break;
            case MessageBoxButton::No:
                Console::println("[MessageBox] No");
                break;
            case MessageBoxButton::Cancel:
                Console::println("[MessageBox] Cancel");
                break;
            case MessageBoxButton::Ok:
                Console::println("[MessageBox] Ok");
                break;
        }
    }
    else
    {
        Console::println("[MessageBox] failed (full)");
    }
    return true;
}
