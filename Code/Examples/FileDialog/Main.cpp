#include "Ark/Storage/Path.hpp"
#include "Ark/System/Console.hpp"
#include "Ark/System/EntryPoint.hpp"
#include "Ark/System/FileDialog.hpp"

bool arkMain(Ark::Collections::Array<Ark::String> const&)
{
    using Ark::System::Console;

    Ark::System::FileDialogOptions fopts;
    Ark::System::FileDialogFilter filters[] = {
        {"PNG images", "png"},
        {"JPEG images", "jpg;jpeg"},
        {"All images", "png;jpg;jpeg"},
        {"All files", "*"},
    };
    fopts.title = "Open Image";
    fopts.filters = filters;
    fopts.filterCount = sizeof(filters) / sizeof(filters[0]);

    auto openRes = Ark::System::showOpenFileDialog(fopts);
    if (openRes)
    {
        Console::println(Ark::String::format("[FileDialog] Selected: {}", openRes.getValue().string()));
    }
    else
    {
        Console::println("[FileDialog] Open canceled or failed");
    }

    Ark::System::FileDialogOptions sopts;
    sopts.title = "Save As";
    sopts.defaultName = "untitled.txt";
    auto saveRes = Ark::System::showSaveFileDialog(sopts);
    if (saveRes)
    {
        Console::println(Ark::String::format("[FileDialog] Save path: {}", saveRes.getValue().string()));
    }
    else
    {
        Console::println("[FileDialog] Save canceled or failed");
    }

    Ark::System::FileDialogOptions dopts;
    dopts.title = "Select Folder";
    auto dirRes = Ark::System::showSelectFolderDialog(dopts);
    if (dirRes)
    {
        Console::println(Ark::String::format("[FileDialog] Folder: {}", dirRes.getValue().string()));
    }
    else
    {
        Console::println("[FileDialog] Folder selection canceled or failed");
    }

    return true;
}
