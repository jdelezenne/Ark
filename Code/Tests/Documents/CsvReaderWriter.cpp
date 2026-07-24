#include "Ark/Testing/Test.hpp"

#include "Ark/Documents/CsvDocument.hpp"
#include "Ark/Documents/CsvReader.hpp"
#include "Ark/Documents/CsvWriter.hpp"
#include "Ark/Storage/File.hpp"
#include "Ark/Storage/FileStream.hpp"
#include "Ark/Storage/FileSystem.hpp"
#include "Ark/Storage/MemoryStream.hpp"
#include "Ark/System/KnownFolders.hpp"

using namespace Ark;
using namespace Ark::Documents;

namespace
{
    CsvDocument createRoundTripCsvDocument()
    {
        CsvDocument doc{true};

        CsvDocument::Row headers;
        headers.append("Name");
        headers.append("Age");
        headers.append("Score");
        doc.setHeaders(headers);

        CsvDocument::Row row1;
        row1.append("Alice");
        row1.append("25");
        row1.append("95.5");
        doc.appendRow(row1);

        CsvDocument::Row row2;
        row2.append("Bob");
        row2.append("30");
        row2.append("87.3");
        doc.appendRow(row2);

        return doc;
    }

    void writeCsvDocumentToPath(CsvDocument const& doc, Storage::Path const& path)
    {
        Storage::FileStream stream(path, Storage::FileMode::CreateAlways);
        REQUIRE(stream.isOpen());

        CsvWriter writer(stream);
        auto writeResult = writer.write(doc);
        REQUIRE(writeResult.isOk());
    }

    CsvDocument readCsvDocumentFromPath(Storage::Path const& path)
    {
        Storage::FileStream stream(path, Storage::FileMode::OpenExisting);
        REQUIRE(stream.isOpen());

        CsvReader::Options options;
        options.hasHeaders = true;

        CsvReader reader(stream, options);
        auto readResult = reader.parse();
        REQUIRE(readResult.isOk());
        return readResult.getValue();
    }
}

ARK_TEST_CASE("CsvReaderWriter", "[csv][reader][writer][roundtrip]")
{
    SECTION("CsvWriter basic writing")
    {
        Storage::Path const tempDir = System::getKnownFolderPath(System::KnownFolder::Temp);
        Storage::Path const testPath = tempDir / "_ark_csv_basic_test.csv";

        CsvDocument doc{true};

        CsvDocument::Row headers;
        headers.append("Name");
        headers.append("Age");
        doc.setHeaders(headers);

        CsvDocument::Row row1;
        row1.append("Alice");
        row1.append("25");
        doc.appendRow(row1);

        {
            Storage::FileStream stream(testPath, Storage::FileMode::CreateAlways);
            REQUIRE(stream.isOpen());

            CsvWriter writer(stream);
            auto result = writer.write(doc);
            REQUIRE(result.isOk());
        }

        auto content = Storage::File::readAllText(testPath);
        REQUIRE(content.isOk());
        REQUIRE(content.getValue().getLength() > 0);
        REQUIRE(content.getValue().contains("Name,Age"));
        REQUIRE(content.getValue().contains("Alice,25"));

        Storage::FileSystem::deleteFile(testPath);
    }

    SECTION("CsvWriter with quoted fields")
    {
        Storage::Path const tempDir = System::getKnownFolderPath(System::KnownFolder::Temp);
        Storage::Path const testPath = tempDir / "_ark_csv_quoted_test.csv";

        CsvDocument doc;

        CsvDocument::Row row;
        row.append("Smith, John");
        row.append("Hello \"World\"");
        row.append("Line1\nLine2");
        doc.appendRow(row);

        {
            Storage::FileStream stream(testPath, Storage::FileMode::CreateAlways);
            REQUIRE(stream.isOpen());

            CsvWriter writer(stream);
            auto result = writer.write(doc);
            REQUIRE(result.isOk());
        }

        auto content = Storage::File::readAllText(testPath);
        REQUIRE(content.isOk());
        REQUIRE(content.getValue().contains("\"Smith, John\""));
        REQUIRE(content.getValue().contains("\"Hello \"\"World\"\"\""));

        Storage::FileSystem::deleteFile(testPath);
    }

    SECTION("CsvWriter always quote mode")
    {
        Storage::Path const tempDir = System::getKnownFolderPath(System::KnownFolder::Temp);
        Storage::Path const testPath = tempDir / "_ark_csv_alwaysquote_test.csv";

        CsvDocument doc;

        CsvDocument::Row row;
        row.append("Simple");
        row.append("Text");
        doc.appendRow(row);

        CsvWriter::Options options;
        options.alwaysQuote = true;

        {
            Storage::FileStream stream(testPath, Storage::FileMode::CreateAlways);
            REQUIRE(stream.isOpen());

            CsvWriter writer(stream, options);
            auto result = writer.write(doc);
            REQUIRE(result.isOk());
        }

        auto content = Storage::File::readAllText(testPath);
        REQUIRE(content.isOk());
        REQUIRE(content.getValue().contains("\"Simple\",\"Text\""));

        Storage::FileSystem::deleteFile(testPath);
    }

    SECTION("CsvWriter unix line endings")
    {
        Storage::Path const tempDir = System::getKnownFolderPath(System::KnownFolder::Temp);
        Storage::Path const testPath = tempDir / "_ark_csv_unix_test.csv";

        CsvDocument doc;
        CsvDocument::Row row1;
        row1.append("A");
        doc.appendRow(row1);

        CsvDocument::Row row2;
        row2.append("B");
        doc.appendRow(row2);

        CsvWriter::Options options;
        options.lineEnding = CsvWriter::Options::LineEnding::Unix;

        {
            Storage::FileStream stream(testPath, Storage::FileMode::CreateAlways);
            REQUIRE(stream.isOpen());

            CsvWriter writer(stream, options);
            writer.write(doc);
        }

        auto content = Storage::File::readAllText(testPath);
        REQUIRE(content.isOk());
        REQUIRE(content.getValue().contains("A\nB"));
        REQUIRE(!content.getValue().contains("\r\n"));

        Storage::FileSystem::deleteFile(testPath);
    }

    SECTION("CsvWriter windows line endings")
    {
        Storage::Path const tempDir = System::getKnownFolderPath(System::KnownFolder::Temp);
        Storage::Path const testPath = tempDir / "_ark_csv_windows_test.csv";

        CsvDocument doc;
        CsvDocument::Row row1;
        row1.append("A");
        doc.appendRow(row1);

        CsvDocument::Row row2;
        row2.append("B");
        doc.appendRow(row2);

        CsvWriter::Options options;
        options.lineEnding = CsvWriter::Options::LineEnding::Windows;

        {
            Storage::FileStream stream(testPath, Storage::FileMode::CreateAlways);
            REQUIRE(stream.isOpen());

            CsvWriter writer(stream, options);
            writer.write(doc);
        }

        auto content = Storage::File::readAllText(testPath);
        REQUIRE(content.isOk());
        REQUIRE(content.getValue().contains("\r\n"));

        Storage::FileSystem::deleteFile(testPath);
    }

    SECTION("CsvWriter custom delimiter")
    {
        Storage::Path const tempDir = System::getKnownFolderPath(System::KnownFolder::Temp);
        Storage::Path const testPath = tempDir / "_ark_csv_delimiter_test.csv";

        CsvDocument doc;

        CsvDocument::Row row;
        row.append("A");
        row.append("B");
        row.append("C");
        doc.appendRow(row);

        CsvWriter::Options options;
        options.delimiter = ';';

        {
            Storage::FileStream stream(testPath, Storage::FileMode::CreateAlways);
            REQUIRE(stream.isOpen());

            CsvWriter writer(stream, options);
            auto result = writer.write(doc);
            REQUIRE(result.isOk());
        }

        auto content = Storage::File::readAllText(testPath);
        REQUIRE(content.isOk());
        REQUIRE(content.getValue().contains("A;B;C"));

        Storage::FileSystem::deleteFile(testPath);
    }

    SECTION("CsvReader basic reading")
    {
        String const csvContent = "Name,Age,Score\nAlice,25,95.5\nBob,30,87.3";

        Storage::MemoryStream stream(csvContent.asPointer(), csvContent.getLength(), true);
        REQUIRE(stream.isOpen());

        CsvReader::Options options;
        options.hasHeaders = true;

        CsvReader reader(stream, options);
        auto result = reader.parse();

        REQUIRE(result.isOk());

        CsvDocument doc = result.getValue();
        REQUIRE(doc.getHasHeaders());
        REQUIRE(doc.getRowCount() == 2);
        REQUIRE(doc.getColumnCount() == 3);

        auto value = doc.getValue(0, "Name");
        REQUIRE(value.hasValue());
        REQUIRE(value.getValue() == "Alice");

        REQUIRE(doc.getInt(1, "Age") == 30);
        REQUIRE(doc.getFloat(0, "Score") == 95.5);
    }

    SECTION("CsvReader without headers")
    {
        String const csvContent = "Alice,25,95.5\nBob,30,87.3";

        Storage::MemoryStream stream(csvContent.asPointer(), csvContent.getLength(), true);
        REQUIRE(stream.isOpen());

        CsvReader::Options options;
        options.hasHeaders = false;

        CsvReader reader(stream, options);
        auto result = reader.parse();

        REQUIRE(result.isOk());

        CsvDocument doc = result.getValue();
        REQUIRE(!doc.getHasHeaders());
        REQUIRE(doc.getRowCount() == 2);

        auto value = doc.getValue(0, 0);
        REQUIRE(value.hasValue());
        REQUIRE(value.getValue() == "Alice");
    }

    SECTION("CsvReader quoted fields")
    {
        String const csvContent = "\"Smith, John\",\"Hello \"\"World\"\"\",\"Line1\nLine2\"";

        Storage::MemoryStream stream(csvContent.asPointer(), csvContent.getLength(), true);
        REQUIRE(stream.isOpen());

        CsvReader reader(stream);
        auto result = reader.parse();

        REQUIRE(result.isOk());

        CsvDocument doc = result.getValue();
        REQUIRE(doc.getRowCount() == 1);

        auto value1 = doc.getValue(0, 0);
        REQUIRE(value1.hasValue());
        REQUIRE(value1.getValue() == "Smith, John");

        auto value2 = doc.getValue(0, 1);
        REQUIRE(value2.hasValue());
        REQUIRE(value2.getValue() == "Hello \"World\"");

        auto value3 = doc.getValue(0, 2);
        REQUIRE(value3.hasValue());
        REQUIRE(value3.getValue() == "Line1\nLine2");
    }

    SECTION("CsvReader empty fields")
    {
        String const csvContent = "A,,C\n,B,\n,,";

        Storage::MemoryStream stream(csvContent.asPointer(), csvContent.getLength(), true);
        REQUIRE(stream.isOpen());

        CsvReader::Options options;
        options.allowEmptyRows = true;

        CsvReader reader(stream, options);
        auto result = reader.parse();

        REQUIRE(result.isOk());

        CsvDocument doc = result.getValue();
        REQUIRE(doc.getRowCount() == 3);

        auto value1 = doc.getValue(0, 1);
        REQUIRE(value1.hasValue());
        REQUIRE(value1.getValue().isEmpty());

        auto value2 = doc.getValue(1, 0);
        REQUIRE(value2.hasValue());
        REQUIRE(value2.getValue().isEmpty());

        auto value3 = doc.getValue(2, 0);
        REQUIRE(value3.hasValue());
        REQUIRE(value3.getValue().isEmpty());
    }

    SECTION("CsvReader custom delimiter")
    {
        String const csvContent = "A;B;C\n1;2;3";

        Storage::MemoryStream stream(csvContent.asPointer(), csvContent.getLength(), true);
        REQUIRE(stream.isOpen());

        CsvReader::Options options;
        options.delimiter = ';';

        CsvReader reader(stream, options);
        auto result = reader.parse();

        REQUIRE(result.isOk());

        CsvDocument doc = result.getValue();
        REQUIRE(doc.getRowCount() == 2);
        REQUIRE(doc.getColumnCount() == 3);

        auto value = doc.getValue(1, 2);
        REQUIRE(value.hasValue());
        REQUIRE(value.getValue() == "3");
    }

    SECTION("CsvReader whitespace trimming")
    {
        String const csvContent = " A , B , C \n 1 , 2 , 3 ";

        Storage::MemoryStream stream(csvContent.asPointer(), csvContent.getLength(), true);
        REQUIRE(stream.isOpen());

        CsvReader::Options options;
        options.trimWhitespace = true;

        CsvReader reader(stream, options);
        auto result = reader.parse();

        REQUIRE(result.isOk());

        CsvDocument doc = result.getValue();
        auto value = doc.getValue(0, 0);
        REQUIRE(value.hasValue());
        REQUIRE(value.getValue() == "A");

        value = doc.getValue(1, 2);
        REQUIRE(value.hasValue());
        REQUIRE(value.getValue() == "3");
    }

    SECTION("CsvReader CRLF line endings")
    {
        String const csvContent = "A,B\r\nC,D\r\nE,F";

        Storage::MemoryStream stream(csvContent.asPointer(), csvContent.getLength(), true);
        REQUIRE(stream.isOpen());

        CsvReader reader(stream);
        auto result = reader.parse();

        REQUIRE(result.isOk());

        CsvDocument doc = result.getValue();
        REQUIRE(doc.getRowCount() == 3);

        auto value = doc.getValue(2, 0);
        REQUIRE(value.hasValue());
        REQUIRE(value.getValue() == "E");
    }

    SECTION("CsvReader empty document")
    {
        String const csvContent = "";

        Storage::MemoryStream stream(csvContent.asPointer(), csvContent.getLength(), true);
        REQUIRE(stream.isOpen());

        CsvReader reader(stream);
        auto result = reader.parse();

        REQUIRE(result.isOk());

        CsvDocument doc = result.getValue();
        REQUIRE(doc.isEmpty());
        REQUIRE(doc.getRowCount() == 0);
    }

    SECTION("CsvReader unclosed quote error")
    {
        String const csvContent = "\"Unclosed quote,B,C";

        Storage::MemoryStream stream(csvContent.asPointer(), csvContent.getLength(), true);
        REQUIRE(stream.isOpen());

        CsvReader reader(stream);
        auto result = reader.parse();

        REQUIRE(result.isError());
    }

    SECTION("CsvReader round-trip preserves shape")
    {
        Storage::Path const tempDir = System::getKnownFolderPath(System::KnownFolder::Temp);
        Storage::Path const testPath = tempDir / "_ark_csv_roundtrip_shape_test.csv";

        CsvDocument originalDoc = createRoundTripCsvDocument();
        writeCsvDocumentToPath(originalDoc, testPath);
        CsvDocument loadedDoc = readCsvDocumentFromPath(testPath);

        REQUIRE(loadedDoc.getRowCount() == originalDoc.getRowCount());
        REQUIRE(loadedDoc.getColumnCount() == originalDoc.getColumnCount());

        Storage::FileSystem::deleteFile(testPath);
    }

    SECTION("CsvReader round-trip preserves values")
    {
        Storage::Path const tempDir = System::getKnownFolderPath(System::KnownFolder::Temp);
        Storage::Path const testPath = tempDir / "_ark_csv_roundtrip_values_test.csv";

        CsvDocument originalDoc = createRoundTripCsvDocument();
        writeCsvDocumentToPath(originalDoc, testPath);
        CsvDocument loadedDoc = readCsvDocumentFromPath(testPath);

        REQUIRE(loadedDoc.getValue(0, "Name").getValue() == "Alice");
        REQUIRE(loadedDoc.getInt(0, "Age") == 25);
        REQUIRE(loadedDoc.getFloat(0, "Score") == 95.5);
        REQUIRE(loadedDoc.getValue(1, "Name").getValue() == "Bob");
        REQUIRE(loadedDoc.getInt(1, "Age") == 30);
        REQUIRE(loadedDoc.getFloat(1, "Score") == 87.3);

        Storage::FileSystem::deleteFile(testPath);
    }

    SECTION("CsvReader complex quoted fields round-trip")
    {
        Storage::Path const tempDir = System::getKnownFolderPath(System::KnownFolder::Temp);
        Storage::Path const testPath = tempDir / "_ark_csv_complex_roundtrip_test.csv";

        CsvDocument originalDoc;

        CsvDocument::Row row;
        row.append("Smith, John");
        row.append("Hello \"World\"");
        row.append("Simple");
        originalDoc.appendRow(row);

        {
            Storage::FileStream stream(testPath, Storage::FileMode::CreateAlways);
            REQUIRE(stream.isOpen());

            CsvWriter writer(stream);
            writer.write(originalDoc);
        }

        {
            Storage::FileStream stream(testPath, Storage::FileMode::OpenExisting);
            REQUIRE(stream.isOpen());

            CsvReader reader(stream);
            auto result = reader.parse();
            REQUIRE(result.isOk());

            CsvDocument loadedDoc = result.getValue();
            REQUIRE(loadedDoc.getValue(0, 0).getValue() == "Smith, John");
            REQUIRE(loadedDoc.getValue(0, 1).getValue() == "Hello \"World\"");
            REQUIRE(loadedDoc.getValue(0, 2).getValue() == "Simple");
        }

        Storage::FileSystem::deleteFile(testPath);
    }

    SECTION("CsvReader allow empty rows")
    {
        String const csvContent = "A,B\n\nC,D\n\nE,F";

        Storage::MemoryStream stream(csvContent.asPointer(), csvContent.getLength(), true);
        REQUIRE(stream.isOpen());

        CsvReader::Options options;
        options.allowEmptyRows = false;

        CsvReader reader(stream, options);
        auto result = reader.parse();
        REQUIRE(result.isOk());

        CsvDocument doc = result.getValue();
        REQUIRE(doc.getRowCount() == 3);
    }

    SECTION("CsvReader allow empty rows enabled")
    {
        String const csvContent = "A,B\n\nC,D\n\nE,F";
        Storage::MemoryStream stream(csvContent.asPointer(), csvContent.getLength(), true);
        REQUIRE(stream.isOpen());

        CsvReader::Options options;
        options.allowEmptyRows = true;

        CsvReader reader(stream, options);
        auto result = reader.parse();
        REQUIRE(result.isOk());

        CsvDocument doc = result.getValue();
        REQUIRE(doc.getRowCount() == 5);
    }
}
