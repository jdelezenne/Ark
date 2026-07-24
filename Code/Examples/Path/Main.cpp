#include "Ark/Storage/Path.hpp"
#include "Ark/Strings/String.hpp"
#include "Ark/System/Console.hpp"
#include "Ark/System/EntryPoint.hpp"

using Ark::Storage::Path;
using Ark::System::Console;

bool arkMain(Ark::Collections::Array<Ark::String> const&)
{
    Path p("/tmp/foo/../bar/file.txt");
    Path n = p.normalized();

    Console::printlnFormat("Path: {}", p.string());
    Console::printlnFormat("Normalized: {}", n.string());
    Console::printlnFormat("Parent: {}", n.parent().string());
    Console::printlnFormat("Filename: {}", n.filename());
    Console::printlnFormat("Stem: {}", n.stem());
    Console::printlnFormat("Extension: {}", n.extension());
    Console::flush();
    return true;
}
