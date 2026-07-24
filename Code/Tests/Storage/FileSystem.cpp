#include "Ark/Testing/Test.hpp"

#include "Ark/Storage/File.hpp"
#include "Ark/Storage/FileSystem.hpp"
#include "Ark/System/KnownFolders.hpp"

ARK_TEST_CASE("FileSystem directory creation and existence", "[filesystem]")
{
    Ark::Storage::Path const root = Ark::System::getKnownFolderPath(Ark::System::KnownFolder::Temp) / "_ark_fs_test";
    Ark::Storage::Path const sub = root / "sub";

    Ark::Storage::FileSystem::deleteDirectoryRecursively(root);

    Ark::Storage::FileSystem::createDirectory(root);
    Ark::Storage::FileSystem::createDirectory(sub);
    auto dirRoot = Ark::Storage::FileSystem::directoryExists(root);
    auto dirSub = Ark::Storage::FileSystem::directoryExists(sub);
    REQUIRE(dirRoot);
    REQUIRE(dirSub);
    REQUIRE(*dirRoot);
    REQUIRE(*dirSub);

    Ark::Storage::FileSystem::deleteDirectoryRecursively(root);
    auto existsAfter = Ark::Storage::FileSystem::exists(root);
    REQUIRE(existsAfter);
    REQUIRE(*existsAfter == false);
}

ARK_TEST_CASE("FileSystem file copy rename and move", "[filesystem]")
{
    Ark::Storage::Path const root = Ark::System::getKnownFolderPath(Ark::System::KnownFolder::Temp) / "_ark_fs_test";
    Ark::Storage::Path const sub = root / "sub";
    Ark::Storage::Path const fileA = root / "a.txt";
    Ark::Storage::Path const fileB = root / "b.txt";

    Ark::Storage::FileSystem::deleteDirectoryRecursively(root);
    Ark::Storage::FileSystem::createDirectory(root);
    Ark::Storage::FileSystem::createDirectory(sub);

    Ark::Storage::File::writeAllText(fileA, "abc");
    auto existsA = Ark::Storage::FileSystem::fileExists(fileA);
    REQUIRE(existsA);
    REQUIRE(*existsA);

    Ark::Storage::FileSystem::copyFile(fileA, fileB, true);
    auto existsB = Ark::Storage::FileSystem::fileExists(fileB);
    REQUIRE(existsB);
    REQUIRE(*existsB);

    Ark::Storage::FileSystem::renameFile(fileB, root / "c.txt");
    auto existsC = Ark::Storage::FileSystem::fileExists(root / "c.txt");
    REQUIRE(existsC);
    REQUIRE(*existsC);

    Ark::Storage::FileSystem::moveFile(root / "c.txt", sub / "c.txt", true);
    auto existsMoved = Ark::Storage::FileSystem::fileExists(sub / "c.txt");
    REQUIRE(existsMoved);
    REQUIRE(*existsMoved);

    Ark::Storage::FileSystem::deleteDirectoryRecursively(root);
    auto existsAfter = Ark::Storage::FileSystem::exists(root);
    REQUIRE(existsAfter);
    REQUIRE(*existsAfter == false);
}

ARK_TEST_CASE("FileSystem directory enumeration", "[filesystem]")
{
    Ark::Storage::Path const root = Ark::System::getKnownFolderPath(Ark::System::KnownFolder::Temp) / "_ark_fs_test";
    Ark::Storage::Path const fileA = root / "a.txt";

    Ark::Storage::FileSystem::deleteDirectoryRecursively(root);
    Ark::Storage::FileSystem::createDirectory(root);
    Ark::Storage::File::writeAllText(fileA, "abc");

    int count = 0;
    auto res = Ark::Storage::FileSystem::enumerateDirectory(
        root, [&]([[maybe_unused]] Ark::Storage::Path const& p, bool isFile)
        {
            if (isFile)
            {
                ++count;
            }
            return true;
        });
    REQUIRE(res);
    REQUIRE(count >= 1);

    Ark::Storage::FileSystem::deleteDirectoryRecursively(root);
    auto existsAfter = Ark::Storage::FileSystem::exists(root);
    REQUIRE(existsAfter);
    REQUIRE(*existsAfter == false);
}
