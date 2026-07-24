#include "Ark/Testing/Test.hpp"

#include "Ark/Collections/Array.hpp"
#include "Ark/Storage/File.hpp"
#include "Ark/Storage/FileSystem.hpp"
#include "Ark/System/KnownFolders.hpp"

namespace
{
    Ark::Storage::Path getTestPath()
    {
        Ark::Storage::Path const tempDir = Ark::System::getKnownFolderPath(Ark::System::KnownFolder::Temp);
        return tempDir / "ark_test_file_io.bin";
    }

    void writeSeedFile(Ark::Storage::Path const& path)
    {
        Ark::Storage::FileSystem::deleteFile(path);

        Ark::Collections::Array<Ark::uint8> data;
        data.resize(5);
        data[0] = 1;
        data[1] = 2;
        data[2] = 3;
        data[3] = 4;
        data[4] = 5;
        Ark::Storage::File::writeAllBinary(path, data);
    }

    void requireDeleted(Ark::Storage::Path const& path)
    {
        Ark::Storage::FileSystem::deleteFile(path);
        auto existsAfter = Ark::Storage::FileSystem::fileExists(path);
        REQUIRE(existsAfter);
        REQUIRE(*existsAfter == false);
    }
}

ARK_TEST_CASE("File", "[file]")
{
    SECTION("write read and size")
    {
        Ark::Storage::Path const path = getTestPath();
        writeSeedFile(path);

        auto sizeRes = Ark::Storage::FileSystem::getFileSize(path);
        REQUIRE(sizeRes);
        REQUIRE(*sizeRes == 5);

        auto readRes = Ark::Storage::File::readAllBinary(path);
        REQUIRE(readRes);
        REQUIRE(readRes->getCount() == 5);
        REQUIRE((*readRes)[0] == 1);
        REQUIRE((*readRes)[4] == 5);

        requireDeleted(path);
    }

    SECTION("seek and partial overwrite")
    {
        Ark::Storage::Path const path = getTestPath();
        writeSeedFile(path);

        auto f = Ark::Storage::File::create();
        REQUIRE(f != nullptr);
        f->open(path, Ark::Storage::FileMode::OpenAlways, {Ark::Storage::FileAccess::ReadWrite, Ark::Storage::FileShare::All});
        REQUIRE(f->isOpen());

        auto pos0 = f->getPosition();
        REQUIRE(pos0);
        REQUIRE(*pos0 == 0);

        f->setPosition(2, Ark::Storage::SeekOrigin::Begin);
        auto pos2 = f->getPosition();
        REQUIRE(pos2);
        REQUIRE(*pos2 == 2);

        f->close();
        REQUIRE(!f->isOpen());

        requireDeleted(path);
    }

    SECTION("overwrite write operation updates position")
    {
        Ark::Storage::Path const path = getTestPath();
        writeSeedFile(path);

        auto f = Ark::Storage::File::create();
        REQUIRE(f != nullptr);
        f->open(path, Ark::Storage::FileMode::OpenAlways, {Ark::Storage::FileAccess::ReadWrite, Ark::Storage::FileShare::All});
        REQUIRE(f->isOpen());

        auto setPos = f->setPosition(2, Ark::Storage::SeekOrigin::Begin);
        REQUIRE(setPos);
        Ark::uint8 twoBytes[2] = {9, 9};
        auto wrote = f->write(twoBytes, 2);
        REQUIRE(wrote);
        REQUIRE(*wrote == 2);

        auto position = f->getPosition();
        REQUIRE(position);
        REQUIRE(*position == 4);

        f->close();
        REQUIRE(!f->isOpen());

        requireDeleted(path);
    }

    SECTION("overwrite readback matches expected bytes")
    {
        Ark::Storage::Path const path = getTestPath();
        writeSeedFile(path);

        auto f = Ark::Storage::File::create();
        REQUIRE(f != nullptr);
        f->open(path, Ark::Storage::FileMode::OpenAlways, {Ark::Storage::FileAccess::ReadWrite, Ark::Storage::FileShare::All});
        REQUIRE(f->isOpen());

        auto setPos = f->setPosition(2, Ark::Storage::SeekOrigin::Begin);
        REQUIRE(setPos);
        Ark::uint8 twoBytes[2] = {9, 9};
        auto wrote = f->write(twoBytes, 2);
        REQUIRE(wrote);
        REQUIRE(*wrote == 2);

        f->setPosition(0, Ark::Storage::SeekOrigin::Begin);
        Ark::uint8 buf[5] = {};
        auto read = f->read(buf, 5);
        REQUIRE(read);
        REQUIRE(*read == 5);

        Ark::uint8 const expected[5] = {1, 2, 9, 9, 5};
        bool matches = true;
        for (Ark::usize i = 0; i < 5; ++i)
        {
            if (buf[i] != expected[i])
            {
                matches = false;
                break;
            }
        }
        REQUIRE(matches);

        f->close();
        REQUIRE(!f->isOpen());

        requireDeleted(path);
    }
}
