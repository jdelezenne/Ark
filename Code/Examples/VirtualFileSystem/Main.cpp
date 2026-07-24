#include "Ark/Storage/File.hpp"
#include "Ark/Storage/FileSystem.hpp"
#include "Ark/Storage/VirtualFileSystem.hpp"
#include "Ark/Strings/String.hpp"
#include "Ark/System/EntryPoint.hpp"
#include "Ark/System/KnownFolders.hpp"

bool arkMain(Ark::Collections::Array<Ark::String> const&)
{
    using Ark::String;
    using Ark::Storage::File;
    using Ark::Storage::FileMode;
    using Ark::Storage::FileSystem;
    using Ark::Storage::Path;
    using Ark::Storage::VirtualFileSystem;

    Path const temp = Ark::System::getKnownFolderPath(Ark::System::KnownFolder::Temp) / "ark_vfs_example";
    FileSystem::deleteDirectoryRecursively(temp);
    FileSystem::createDirectory(temp);
    File::writeAllText(temp / "example.txt", "Virtual FS example");

    VirtualFileSystem vfs;
    vfs.mount(temp);

    if (vfs.exists("example.txt"))
    {
        auto s = vfs.openStream("example.txt", FileMode::OpenExisting);
        char buffer[64] = {};
        auto read = s.read(buffer, sizeof(buffer));
    }

    FileSystem::deleteDirectoryRecursively(temp);
    return true;
}
