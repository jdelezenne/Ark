#include "Ark/Documents/Documents.hpp"
#include "Ark/Storage/FileStream.hpp"
#include "Ark/Strings/String.hpp"
#include "Ark/System/Console.hpp"
#include "Ark/System/EntryPoint.hpp"
#include "Ark/System/KnownFolders.hpp"

using namespace Ark;
using namespace Ark::Documents;

bool arkMain(Ark::Collections::Array<Ark::String> const&)
{
    System::Console::println("Ark INI Document Example");

    Storage::Path const tempDir = System::getKnownFolderPath(System::KnownFolder::Temp);
    Storage::Path const configPath = tempDir / "ark_example_config.ini";

    IniDocument config;

    config.setValue("", "version", "1.0");
    config.setValue("", "author", "John Doe");

    config.setInt("window", "width", 1920);
    config.setInt("window", "height", 1080);
    config.setBool("window", "fullscreen", false);

    config.setValue("graphics", "renderer", "OpenGL");
    config.setInt("graphics", "vsync", 1);
    config.setFloat("graphics", "scale", 1.5);

    config.setComment("window", "Window settings");
    config.setComment("graphics", "Graphics configuration");

    int width = config.getInt("window", "width");
    bool fullscreen = config.getBool("window", "fullscreen");
    String renderer = config.getValue("graphics", "renderer");

    System::Console::printlnFormat("Writing to {}...", configPath.string());
    Storage::FileStream outputFile(configPath, Storage::FileMode::CreateAlways);
    if (!outputFile.isOpen())
    {
        System::Console::println("Error: Failed to open file for writing");
        return false;
    }

    IniWriter writer(outputFile);
    writer.write(config);
    outputFile.close();
    System::Console::println("File written successfully");

    System::Console::printlnFormat("Reading from {}...", configPath.string());
    Storage::FileStream inputFile(configPath, Storage::FileMode::OpenExisting);
    if (!inputFile.isOpen())
    {
        System::Console::println("Error: Failed to open file for reading");
        return false;
    }

    IniReader reader(inputFile);
    auto parseResult = reader.parse();
    if (parseResult.isOk())
    {
        IniDocument loadedConfig = parseResult.getValue();

        int loadedWidth = loadedConfig.getInt("window", "width");
        String loadedRenderer = loadedConfig.getValue("graphics", "renderer");

        System::Console::println("Loaded configuration:");
        System::Console::printlnFormat("  Width: {}", loadedWidth);
        System::Console::printlnFormat("  Renderer: {}", loadedRenderer);
        System::Console::printlnFormat("  Fullscreen: {}", loadedConfig.getBool("window", "fullscreen") ? "true" : "false");
        System::Console::printlnFormat("  VSync: {}", loadedConfig.getInt("graphics", "vsync"));
        System::Console::printlnFormat("  Scale: {}", loadedConfig.getFloat("graphics", "scale"));
    }
    else
    {
        System::Console::println("Error: Failed to parse INI document");
        return false;
    }
    inputFile.close();

    System::Console::println("\n=== CSV Example ===");

    Storage::Path const csvPath = tempDir / "ark_example_data.csv";

    CsvDocument csvDoc{true};

    CsvDocument::Row headers;
    headers.append("Name");
    headers.append("Age");
    headers.append("Score");
    csvDoc.setHeaders(headers);

    CsvDocument::Row row1;
    row1.append("Alice");
    row1.append("25");
    row1.append("95.5");
    csvDoc.appendRow(row1);

    CsvDocument::Row row2;
    row2.append("Bob");
    row2.append("30");
    row2.append("87.3");
    csvDoc.appendRow(row2);

    CsvDocument::Row row3;
    row3.append("Charlie");
    row3.append("22");
    row3.append("92.1");
    csvDoc.appendRow(row3);

    System::Console::printlnFormat("Writing to {}...", csvPath.string());
    Storage::FileStream csvOutputFile(csvPath, Storage::FileMode::CreateAlways);
    if (!csvOutputFile.isOpen())
    {
        System::Console::println("Error: Failed to open CSV file for writing");
        return false;
    }

    CsvWriter csvWriter(csvOutputFile);
    csvWriter.write(csvDoc);
    csvOutputFile.close();
    System::Console::println("CSV file written successfully");

    System::Console::printlnFormat("Reading from {}...", csvPath.string());
    Storage::FileStream csvInputFile(csvPath, Storage::FileMode::OpenExisting);
    if (!csvInputFile.isOpen())
    {
        System::Console::println("Error: Failed to open CSV file for reading");
        return false;
    }

    CsvReader::Options csvOptions;
    csvOptions.hasHeaders = true;
    CsvReader csvReader(csvInputFile, csvOptions);
    auto csvParseResult = csvReader.parse();
    if (csvParseResult.isOk())
    {
        CsvDocument loadedCsv = csvParseResult.getValue();

        System::Console::printlnFormat("Loaded {} rows from CSV", loadedCsv.getRowCount());

        for (usize i = 0; i < loadedCsv.getRowCount(); ++i)
        {
            auto nameOpt = loadedCsv.getValue(i, "Name");
            int64 age = loadedCsv.getInt(i, "Age");
            float64 score = loadedCsv.getFloat(i, "Score");

            if (nameOpt.hasValue())
            {
                System::Console::printlnFormat("  {} (age {}, score {})", nameOpt.getValue(), age, score);
            }
        }
    }
    else
    {
        System::Console::println("Error: Failed to parse CSV document");
        return false;
    }
    csvInputFile.close();

    System::Console::println("\nExample completed successfully");

    return true;
}
