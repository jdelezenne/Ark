#include "Ark/Storage/File.hpp"
#include "Ark/Storage/FileSystem.hpp"
#include "Ark/Strings/String.hpp"
#include "Ark/System/Console.hpp"
#include "Ark/System/EntryPoint.hpp"
#include "Ark/System/KnownFolders.hpp"

using Ark::System::Console;

bool arkMain(Ark::Collections::Array<Ark::String> const&)
{
    Ark::Storage::Path const tempDir = Ark::System::getKnownFolderPath(Ark::System::KnownFolder::Temp);
    Ark::Storage::Path const path = tempDir / "ark_example_storage.txt";

    Ark::Storage::FileSystem::deleteFile(path);

    Ark::String const text = "Hello Storage";
    Ark::Storage::File::writeAllText(path, text);
    auto back = Ark::Storage::File::readAllText(path);
    if (back)
    {
        Console::println(Ark::String::format("Wrote and read back: {}", *back));
    }

    Ark::Storage::FileSystem::deleteFile(path);
    Console::flush();
    return true;
}
