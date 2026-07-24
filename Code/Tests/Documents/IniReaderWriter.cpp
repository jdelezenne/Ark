#include "Ark/Testing/Test.hpp"

#include "Ark/Documents/IniDocument.hpp"
#include "Ark/Documents/IniReader.hpp"
#include "Ark/Documents/IniWriter.hpp"
#include "Ark/Storage/File.hpp"
#include "Ark/Storage/FileStream.hpp"
#include "Ark/Storage/FileSystem.hpp"
#include "Ark/Storage/MemoryStream.hpp"
#include "Ark/Storage/UnsafeMemoryStream.hpp"
#include "Ark/System/Console.hpp"
#include "Ark/System/KnownFolders.hpp"

using namespace Ark;
using namespace Ark::Documents;

namespace
{
    IniDocument createRoundTripIniDocument()
    {
        IniDocument doc;
        doc.setValue("", "globalKey", "globalValue");
        doc.setValue("section1", "key1", "value1");
        doc.setInt("section1", "number", 42);
        doc.setBool("section1", "flag", true);
        doc.setValue("section2", "key2", "value2");
        doc.setFloat("section2", "pi", 3.14159);
        doc.setComment("section1", "First section");
        return doc;
    }

    void requireRoundTripIniValues(IniDocument const& doc)
    {
        REQUIRE(doc.getValue("", "globalKey") == "globalValue");
        REQUIRE(doc.getValue("section1", "key1") == "value1");
        REQUIRE(doc.getInt("section1", "number") == 42);
        REQUIRE(doc.getBool("section1", "flag") == true);
        REQUIRE(doc.getValue("section2", "key2") == "value2");

        float64 const pi = doc.getFloat("section2", "pi");
        REQUIRE(pi > 3.14);
        REQUIRE(pi < 3.15);
    }

    void writeIniDocumentToPath(IniDocument const& doc, Storage::Path const& path)
    {
        Storage::FileStream writeStream(path, Storage::FileMode::CreateNew);
        REQUIRE(writeStream.isOpen());

        IniWriter::Options options;
        options.writeComments = true;

        IniWriter writer(writeStream, options);
        auto writeResult = writer.write(doc);
        REQUIRE(writeResult.isOk());
    }

    IniDocument readIniDocumentFromPath(Storage::Path const& path)
    {
        Storage::FileStream readStream(path, Storage::FileMode::OpenExisting);
        REQUIRE(readStream.isOpen());

        IniReader reader(readStream);
        auto readResult = reader.parse();
        REQUIRE(readResult.isOk());
        return readResult.getValue();
    }
}

ARK_TEST_CASE("IniReaderWriter", "[ini][reader][writer][roundtrip]")
{
    SECTION("IniWriter basic writing")
    {
        Storage::Path const tempDir = System::getKnownFolderPath(System::KnownFolder::Temp);
        Storage::Path const testPath = tempDir / "_ark_ini_basic_test.ini";

        IniDocument doc;
        doc.setValue("section", "key", "value");
        doc.setInt("section", "number", 42);

        {
            Storage::FileStream stream(testPath, Storage::FileMode::CreateAlways);
            REQUIRE(stream.isOpen());

            IniWriter writer(stream);
            auto result = writer.write(doc);
            REQUIRE(result.isOk());
        }

        auto content = Storage::File::readAllText(testPath);
        REQUIRE(content.isOk());
        REQUIRE(content.getValue().getLength() > 0);
        REQUIRE(content.getValue().contains("[section]"));
        REQUIRE(content.getValue().contains("key=value"));

        Storage::FileSystem::deleteFile(testPath);
    }

    SECTION("IniWriter with comments")
    {
        Storage::Path const tempDir = System::getKnownFolderPath(System::KnownFolder::Temp);
        Storage::Path const testPath = tempDir / "_ark_ini_comment_test.ini";

        IniDocument doc;
        doc.setComment("section", "This is a section comment");
        doc.setValue("section", "key", "value");

        IniWriter::Options options;
        options.writeComments = true;

        {
            Storage::FileStream stream(testPath, Storage::FileMode::CreateAlways);
            REQUIRE(stream.isOpen());

            IniWriter writer(stream, options);
            auto result = writer.write(doc);
            REQUIRE(result.isOk());
        }

        auto content = Storage::File::readAllText(testPath);
        REQUIRE(content.isOk());
        REQUIRE(content.getValue().contains("; This is a section comment"));

        Storage::FileSystem::deleteFile(testPath);
    }

    SECTION("IniWriter without comments")
    {
        Storage::Path const tempDir = System::getKnownFolderPath(System::KnownFolder::Temp);
        Storage::Path const testPath = tempDir / "_ark_ini_nocomment_test.ini";

        IniDocument doc;
        doc.setComment("section", "This should not appear");
        doc.setValue("section", "key", "value");

        IniWriter::Options options;
        options.writeComments = false;

        {
            Storage::FileStream stream(testPath, Storage::FileMode::CreateAlways);
            REQUIRE(stream.isOpen());

            IniWriter writer(stream, options);
            auto result = writer.write(doc);
            REQUIRE(result.isOk());
        }

        auto content = Storage::File::readAllText(testPath);
        REQUIRE(content.isOk());
        REQUIRE(!content.getValue().contains("This should not appear"));

        Storage::FileSystem::deleteFile(testPath);
    }

    SECTION("IniWriter unix line endings")
    {
        Storage::Path const tempDir = System::getKnownFolderPath(System::KnownFolder::Temp);
        Storage::Path const testPath = tempDir / "_ark_ini_unix_test.ini";

        IniDocument doc;
        doc.setValue("section", "key", "value");

        IniWriter::Options options;
        options.lineEnding = IniWriter::Options::LineEnding::Unix;

        {
            Storage::FileStream stream(testPath, Storage::FileMode::CreateAlways);
            IniWriter writer(stream, options);
            writer.write(doc);
        }

        auto content = Storage::File::readAllText(testPath);
        REQUIRE(content.isOk());
        REQUIRE(!content.getValue().contains("\r\n"));

        Storage::FileSystem::deleteFile(testPath);
    }

    SECTION("IniWriter windows line endings")
    {
        Storage::Path const tempDir = System::getKnownFolderPath(System::KnownFolder::Temp);
        Storage::Path const testPath = tempDir / "_ark_ini_windows_test.ini";

        IniDocument doc;
        doc.setValue("section", "key", "value");

        IniWriter::Options options;
        options.lineEnding = IniWriter::Options::LineEnding::Windows;

        {
            Storage::FileStream stream(testPath, Storage::FileMode::CreateAlways);
            IniWriter writer(stream, options);
            writer.write(doc);
        }

        auto content = Storage::File::readAllText(testPath);
        REQUIRE(content.isOk());
        REQUIRE(content.getValue().contains("\r\n"));

        Storage::FileSystem::deleteFile(testPath);
    }

    SECTION("IniReader basic parsing")
    {
        Storage::Path const tempDir = System::getKnownFolderPath(System::KnownFolder::Temp);
        Storage::Path const testPath = tempDir / "_ark_ini_read_test.ini";

        String const iniContent =
            "[section]\n"
            "key=value\n"
            "number=42\n";

        Storage::File::writeAllText(testPath, iniContent);

        {
            Storage::FileStream stream(testPath, Storage::FileMode::OpenExisting);
            REQUIRE(stream.isOpen());

            IniReader reader(stream);
            auto result = reader.parse();

            REQUIRE(result.isOk());

            IniDocument doc = result.getValue();
            REQUIRE(doc.getValue("section", "key") == "value");
            REQUIRE(doc.getValue("section", "number") == "42");
            REQUIRE(doc.getInt("section", "number") == 42);
        }

        Storage::FileSystem::deleteFile(testPath);
    }

    SECTION("IniReader with comments")
    {
        Storage::Path const tempDir = System::getKnownFolderPath(System::KnownFolder::Temp);
        Storage::Path const testPath = tempDir / "_ark_ini_comment_read_test.ini";

        String const iniContent =
            "# This is a comment\n"
            "[section]\n"
            "key=value # inline comment\n";

        Storage::File::writeAllText(testPath, iniContent);

        {
            Storage::FileStream stream(testPath, Storage::FileMode::OpenExisting);
            REQUIRE(stream.isOpen());

            IniReader reader(stream);
            auto result = reader.parse();

            REQUIRE(result.isOk());

            IniDocument doc = result.getValue();
            REQUIRE(doc.getValue("section", "key") == "value");
        }

        Storage::FileSystem::deleteFile(testPath);
    }

    SECTION("IniReader with global section")
    {
        Storage::Path const tempDir = System::getKnownFolderPath(System::KnownFolder::Temp);
        Storage::Path const testPath = tempDir / "_ark_ini_global_test.ini";

        String const iniContent =
            "globalKey=globalValue\n"
            "globalNumber=100\n"
            "[section]\n"
            "key=value\n";

        Storage::File::writeAllText(testPath, iniContent);

        {
            Storage::FileStream stream(testPath, Storage::FileMode::OpenExisting);
            REQUIRE(stream.isOpen());

            IniReader reader(stream);
            auto result = reader.parse();

            REQUIRE(result.isOk());

            IniDocument doc = result.getValue();
            REQUIRE(doc.hasKey("", "globalKey"));
            REQUIRE(doc.hasKey("", "globalNumber"));
            REQUIRE(doc.getValue("", "globalKey") == "globalValue");
            REQUIRE(doc.getValue("", "globalNumber") == "100");
            REQUIRE(doc.getInt("", "globalNumber") == 100);
            REQUIRE(doc.getValue("section", "key") == "value");
        }

        Storage::FileSystem::deleteFile(testPath);
    }

    SECTION("IniReader with whitespace")
    {
        Storage::Path const tempDir = System::getKnownFolderPath(System::KnownFolder::Temp);
        Storage::Path const testPath = tempDir / "_ark_ini_whitespace_test.ini";

        String const iniContent =
            "  [ section ]  \n"
            "  key  =  value  \n"
            "  \n"
            "key2=value2\n";

        Storage::File::writeAllText(testPath, iniContent);

        {
            Storage::FileStream stream(testPath, Storage::FileMode::OpenExisting);
            REQUIRE(stream.isOpen());

            IniReader reader(stream);
            auto result = reader.parse();

            REQUIRE(result.isOk());

            IniDocument doc = result.getValue();
            REQUIRE(doc.getValue("section", "key") == "value");
            REQUIRE(doc.getValue("section", "key2") == "value2");
        }

        Storage::FileSystem::deleteFile(testPath);
    }

    SECTION("IniReader empty lines")
    {
        Storage::Path const tempDir = System::getKnownFolderPath(System::KnownFolder::Temp);
        Storage::Path const testPath = tempDir / "_ark_ini_empty_test.ini";

        String const iniContent =
            "\n"
            "[section]\n"
            "\n"
            "key=value\n"
            "\n";

        Storage::File::writeAllText(testPath, iniContent);

        {
            Storage::FileStream stream(testPath, Storage::FileMode::OpenExisting);
            REQUIRE(stream.isOpen());

            IniReader reader(stream);
            auto result = reader.parse();

            REQUIRE(result.isOk());

            IniDocument doc = result.getValue();
            REQUIRE(doc.getValue("section", "key") == "value");
        }

        Storage::FileSystem::deleteFile(testPath);
    }

    SECTION("IniReader malformed lines")
    {
        Storage::Path const tempDir = System::getKnownFolderPath(System::KnownFolder::Temp);
        Storage::Path const testPath = tempDir / "_ark_ini_malformed_test.ini";

        String const iniContent =
            "[section]\n"
            "validKey=validValue\n"
            "invalidLine\n"
            "anotherKey=anotherValue\n";

        Storage::File::writeAllText(testPath, iniContent);

        {
            Storage::FileStream stream(testPath, Storage::FileMode::OpenExisting);
            REQUIRE(stream.isOpen());

            IniReader reader(stream);
            auto result = reader.parse();

            REQUIRE(result.isOk());

            IniDocument doc = result.getValue();
            REQUIRE(doc.getValue("section", "validKey") == "validValue");
            REQUIRE(doc.getValue("section", "anotherKey") == "anotherValue");
        }

        Storage::FileSystem::deleteFile(testPath);
    }

    SECTION("IniReader and IniWriter round-trip")
    {
        Storage::Path const tempDir = System::getKnownFolderPath(System::KnownFolder::Temp);
        Storage::Path const testPath = tempDir / "_ark_ini_test.ini";

        IniDocument original = createRoundTripIniDocument();

        Storage::FileSystem::deleteFile(testPath);

        writeIniDocumentToPath(original, testPath);
        IniDocument loaded = readIniDocumentFromPath(testPath);
        requireRoundTripIniValues(loaded);

        Storage::FileSystem::deleteFile(testPath);
    }

    SECTION("IniReader multiple sections")
    {
        Storage::Path const tempDir = System::getKnownFolderPath(System::KnownFolder::Temp);
        Storage::Path const testPath = tempDir / "_ark_ini_multi_test.ini";

        String const iniContent =
            "[section1]\n"
            "key1=value1\n"
            "[section2]\n"
            "key2=value2\n"
            "[section3]\n"
            "key3=value3\n";

        Storage::File::writeAllText(testPath, iniContent);

        {
            Storage::FileStream stream(testPath, Storage::FileMode::OpenExisting);
            REQUIRE(stream.isOpen());

            IniReader reader(stream);
            auto result = reader.parse();

            REQUIRE(result.isOk());

            IniDocument doc = result.getValue();

            REQUIRE(doc.hasKey("section1", "key1"));
            REQUIRE(doc.hasKey("section2", "key2"));
            REQUIRE(doc.hasKey("section3", "key3"));

            REQUIRE(doc.getSectionCount() == 3);
            REQUIRE(doc.getValue("section1", "key1") == "value1");
            REQUIRE(doc.getValue("section2", "key2") == "value2");
            REQUIRE(doc.getValue("section3", "key3") == "value3");
        }

        Storage::FileSystem::deleteFile(testPath);
    }

    SECTION("IniReader with MemoryStream")
    {
        String const iniContent =
            "[section]\n"
            "key=value\n"
            "number=42\n";

        Storage::MemoryStream stream(iniContent.asPointer(), iniContent.getLength(), true);
        REQUIRE(stream.isOpen());

        IniReader reader(stream);
        auto result = reader.parse();

        REQUIRE(result.isOk());

        IniDocument doc = result.getValue();
        REQUIRE(doc.getValue("section", "key") == "value");
        REQUIRE(doc.getInt("section", "number") == 42);
    }

    SECTION("IniReader with MemoryStream multiple sections")
    {
        String const iniContent =
            "[section1]\n"
            "key1=value1\n"
            "number1=100\n"
            "[section2]\n"
            "key2=value2\n"
            "flag=true\n";

        Storage::MemoryStream stream(iniContent.asPointer(), iniContent.getLength(), true);
        REQUIRE(stream.isOpen());

        IniReader reader(stream);
        auto result = reader.parse();

        REQUIRE(result.isOk());

        IniDocument doc = result.getValue();
        REQUIRE(doc.getValue("section1", "key1") == "value1");
        REQUIRE(doc.getInt("section1", "number1") == 100);
        REQUIRE(doc.getValue("section2", "key2") == "value2");
        REQUIRE(doc.getBool("section2", "flag") == true);
    }

    SECTION("IniReader with MemoryStream and global section")
    {
        String const iniContent =
            "globalKey=globalValue\n"
            "[section]\n"
            "key=value\n";

        Storage::MemoryStream stream(iniContent.asPointer(), iniContent.getLength(), true);
        REQUIRE(stream.isOpen());

        IniReader reader(stream);
        auto result = reader.parse();

        REQUIRE(result.isOk());

        IniDocument doc = result.getValue();
        REQUIRE(doc.getValue("", "globalKey") == "globalValue");
        REQUIRE(doc.getValue("section", "key") == "value");
    }

    SECTION("IniWriter with UnsafeMemoryStream")
    {
        IniDocument doc;
        doc.setValue("section", "key", "value");
        doc.setInt("section", "number", 42);

        uint8 buffer[1024];
        Storage::UnsafeMemoryStream stream(buffer, sizeof(buffer), false);
        REQUIRE(stream.isOpen());

        IniWriter writer(stream);
        auto result = writer.write(doc);
        REQUIRE(result.isOk());

        auto position = stream.getPosition();
        REQUIRE(position.isOk());
        REQUIRE(position.getValue() > 0);

        String content(reinterpret_cast<char const*>(buffer), position.getValue());
        REQUIRE(content.contains("[section]"));
        REQUIRE(content.contains("key=value"));
        REQUIRE(content.contains("number=42"));
    }

    SECTION("IniWriter with UnsafeMemoryStream and comments")
    {
        IniDocument doc;
        doc.setComment("section", "This is a section comment");
        doc.setValue("section", "key", "value");

        IniWriter::Options options;
        options.writeComments = true;

        uint8 buffer[1024];
        Storage::UnsafeMemoryStream stream(buffer, sizeof(buffer), false);
        REQUIRE(stream.isOpen());

        IniWriter writer(stream, options);
        auto result = writer.write(doc);
        REQUIRE(result.isOk());

        auto position = stream.getPosition();
        REQUIRE(position.isOk());

        String content(reinterpret_cast<char const*>(buffer), position.getValue());
        REQUIRE(content.contains("; This is a section comment"));
        REQUIRE(content.contains("[section]"));
        REQUIRE(content.contains("key=value"));
    }

    SECTION("IniWriter with UnsafeMemoryStream unix line endings")
    {
        IniDocument doc;
        doc.setValue("section", "key", "value");

        IniWriter::Options options;
        options.lineEnding = IniWriter::Options::LineEnding::Unix;

        uint8 buffer[1024];
        Storage::UnsafeMemoryStream stream(buffer, sizeof(buffer), false);

        IniWriter writer(stream, options);
        writer.write(doc);

        auto position = stream.getPosition();
        REQUIRE(position.isOk());

        String content(reinterpret_cast<char const*>(buffer), position.getValue());
        REQUIRE(!content.contains("\r\n"));
    }

    SECTION("IniWriter with UnsafeMemoryStream windows line endings")
    {
        IniDocument doc;
        doc.setValue("section", "key", "value");

        IniWriter::Options options;
        options.lineEnding = IniWriter::Options::LineEnding::Windows;

        uint8 buffer[1024];
        Storage::UnsafeMemoryStream stream(buffer, sizeof(buffer), false);

        IniWriter writer(stream, options);
        writer.write(doc);

        auto position = stream.getPosition();
        REQUIRE(position.isOk());

        String content(reinterpret_cast<char const*>(buffer), position.getValue());
        REQUIRE(content.contains("\r\n"));
    }

    SECTION("IniWriter with memory stream writes content")
    {
        IniDocument doc;
        doc.setValue("", "globalKey", "globalValue");
        doc.setValue("section1", "key1", "value1");
        doc.setInt("section1", "number", 42);
        doc.setBool("section1", "flag", true);
        doc.setValue("section2", "key2", "value2");
        doc.setFloat("section2", "pi", 3.14159);

        uint8 buffer[2048];

        Storage::UnsafeMemoryStream writeStream(buffer, sizeof(buffer), false);
        REQUIRE(writeStream.isOpen());

        IniWriter writer(writeStream);
        auto writeResult = writer.write(doc);
        REQUIRE(writeResult.isOk());

        auto position = writeStream.getPosition();
        REQUIRE(position.isOk());
        REQUIRE(position.getValue() > 0);
    }

    SECTION("IniReader and IniWriter round-trip with memory streams")
    {
        IniDocument original = createRoundTripIniDocument();

        uint8 buffer[2048];

        Storage::UnsafeMemoryStream writeStream(buffer, sizeof(buffer), false);
        REQUIRE(writeStream.isOpen());

        IniWriter writer(writeStream);
        auto writeResult = writer.write(original);
        REQUIRE(writeResult.isOk());

        auto position = writeStream.getPosition();
        REQUIRE(position.isOk());

        usize const writtenBytes = position.getValue();
        REQUIRE(writtenBytes > 0);

        Storage::MemoryStream readStream(buffer, writtenBytes, true);
        REQUIRE(readStream.isOpen());

        auto setPos = readStream.setPosition(0, Storage::SeekOrigin::Begin);
        REQUIRE(setPos.isOk());

        IniReader reader(readStream);
        auto readResult = reader.parse();
        REQUIRE(readResult.isOk());

        IniDocument loaded = readResult.getValue();
        requireRoundTripIniValues(loaded);
    }

    SECTION("IniReader with MemoryStream empty document")
    {
        String const iniContent = "";

        Storage::MemoryStream stream(iniContent.asPointer(), iniContent.getLength(), true);
        REQUIRE(stream.isOpen());

        IniReader reader(stream);
        auto result = reader.parse();

        REQUIRE(result.isOk());

        IniDocument doc = result.getValue();
        REQUIRE(doc.getSectionCount() == 0);
    }

    SECTION("IniReader with MemoryStream only comments")
    {
        String const iniContent =
            "# This is a comment\n"
            "; Another comment\n"
            "\n";

        Storage::MemoryStream stream(iniContent.asPointer(), iniContent.getLength(), true);
        REQUIRE(stream.isOpen());

        IniReader reader(stream);
        auto result = reader.parse();

        REQUIRE(result.isOk());

        IniDocument doc = result.getValue();
        REQUIRE(doc.getSectionCount() == 0);
    }
}
