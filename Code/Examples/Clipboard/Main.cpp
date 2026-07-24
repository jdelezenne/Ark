#include "Ark/Strings/String.hpp"
#include "Ark/System/Clipboard.hpp"
#include "Ark/System/Console.hpp"
#include "Ark/System/EntryPoint.hpp"

bool arkMain(Ark::Collections::Array<Ark::String> const&)
{
    using Ark::System::Clipboard::getText;
    using Ark::System::Clipboard::hasText;
    using Ark::System::Clipboard::setText;

    Ark::String text = "Example clipboard text";
    if (setText(text))
    {
        Ark::System::Console::printlnf("Copied to clipboard: %s", text.asPointer());
    }
    else
    {
        Ark::System::Console::printlnf("Failed to set clipboard text");
        return false;
    }

    if (hasText())
    {
        Ark::String got = getText();
        Ark::System::Console::printlnf("Clipboard now: %s", got.asPointer());
    }
    else
    {
        Ark::System::Console::printlnf("Clipboard has no text");
    }
    return true;
}
