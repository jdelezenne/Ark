#include "Ark/Storage/VirtualFileSystem.hpp"
#include "Ark/Storage/File.hpp"
#include "Ark/Storage/FileSystem.hpp"
#include "Ark/Strings/String.hpp"
#include "Ark/System/KnownFolders.hpp"

#include "Ark/Testing/Test.hpp"

ARK_TEST_CASE("VirtualFileSystem resolves existing mounted files", "[vfs]")
{
    using Ark::Storage::File;
    using Ark::Storage::FileMode;
    using Ark::Storage::FileSystem;
    using Ark::Storage::Path;
    using Ark::Storage::VirtualFileSystem;

    Path const tempRoot = Ark::System::getKnownFolderPath(Ark::System::KnownFolder::Temp) / "ark_vfs_test";
    Path const dirA = tempRoot / "dirA";
    Path const fileA = dirA / "hello.txt";

    FileSystem::deleteDirectoryRecursively(tempRoot);
    FileSystem::createDirectory(tempRoot);
    FileSystem::createDirectory(dirA);
    File::writeAllText(fileA, "hello");

    VirtualFileSystem vfs;
    vfs.mount(tempRoot);

    bool const e = vfs.exists("dirA/hello.txt");
    REQUIRE(e);

    auto rp = vfs.resolve("dirA/hello.txt");
    REQUIRE(rp);
    REQUIRE(rp.getValue() == fileA);

    auto f = vfs.openFile("dirA/hello.txt", FileMode::OpenExisting);
    REQUIRE(f != nullptr);
    auto size = f->getSize();
    REQUIRE(size);
    REQUIRE(*size == 5);

    FileSystem::deleteDirectoryRecursively(tempRoot);
}

ARK_TEST_CASE("VirtualFileSystem missing entries are not resolved", "[vfs]")
{
    using Ark::Storage::File;
    using Ark::Storage::FileMode;
    using Ark::Storage::FileSystem;
    using Ark::Storage::Path;
    using Ark::Storage::VirtualFileSystem;

    Path const tempRoot = Ark::System::getKnownFolderPath(Ark::System::KnownFolder::Temp) / "ark_vfs_test";
    Path const dirA = tempRoot / "dirA";
    Path const fileA = dirA / "hello.txt";

    FileSystem::deleteDirectoryRecursively(tempRoot);
    FileSystem::createDirectory(tempRoot);
    FileSystem::createDirectory(dirA);
    File::writeAllText(fileA, "hello");

    VirtualFileSystem vfs;
    vfs.mount(tempRoot);

    bool const e = vfs.exists("missing.txt");
    REQUIRE_FALSE(e);

    auto rp = vfs.resolve("missing.txt");
    REQUIRE_FALSE(rp);

    auto f = vfs.openFile("missing.txt", FileMode::OpenExisting);
    REQUIRE(f == nullptr);

    FileSystem::deleteDirectoryRecursively(tempRoot);
}
