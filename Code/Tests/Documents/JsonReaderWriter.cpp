#include "Ark/Testing/Test.hpp"

#include "Ark/Documents/JsonReader.hpp"
#include "Ark/Documents/JsonWriter.hpp"
#include "Ark/Storage/File.hpp"
#include "Ark/Storage/FileStream.hpp"
#include "Ark/Storage/FileSystem.hpp"
#include "Ark/Storage/MemoryStream.hpp"
#include "Ark/System/KnownFolders.hpp"

using namespace Ark;
using namespace Ark::Documents;

ARK_TEST_CASE("JsonReaderWriter", "[json][reader][writer]")
{
    SECTION("JsonWriter and JsonReader round-trip object")
    {
        Storage::Path const tempDir = System::getKnownFolderPath(System::KnownFolder::Temp);
        Storage::Path const testPath = tempDir / "_ark_json_roundtrip_test.json";

        {
            Storage::FileStream stream(testPath, Storage::FileMode::CreateAlways);
            REQUIRE(stream.isOpen());

            JsonWriter writer(stream);
            writer.startObject();

            writer.writeKey("name");
            writer.writeString("Alice");

            writer.writeKey("age");
            writer.writeUint32(30);

            writer.writeKey("active");
            writer.writeBool(true);

            writer.writeKey("scores");
            writer.startArray();
            writer.writeInt32(10);
            writer.writeInt32(20);
            writer.writeInt32(30);
            writer.endArray(3);

            writer.endObject();
            REQUIRE(writer.close().isOk());
        }

        auto content = Storage::File::readAllText(testPath);
        REQUIRE(content.isOk());
        REQUIRE(content.getValue().contains("\"name\""));
        REQUIRE(content.getValue().contains("Alice"));

        {
            Storage::FileStream stream(testPath, Storage::FileMode::OpenExisting);
            REQUIRE(stream.isOpen());

            JsonReader reader(stream);
            auto root = reader.getRoot();

            REQUIRE(root.isObject());
            REQUIRE(root.hasMember("name"));
            REQUIRE(root.getString("name") == "Alice");
            REQUIRE(root.getUint("age") == 30u);
            REQUIRE(root.getBool("active") == true);

            auto scores = root.getMember("scores");
            REQUIRE(scores.isArray());
            REQUIRE(scores.getCount() == 3u);
            REQUIRE(scores.getMember(0).asInt64() == 10);
            REQUIRE(scores.getMember(1).asInt64() == 20);
            REQUIRE(scores.getMember(2).asInt64() == 30);

            auto names = root.getMemberNames();
            REQUIRE(names.getCount() == 4);
        }

        Storage::FileSystem::deleteFile(testPath);
    }

    SECTION("JsonReader handles invalid JSON input")
    {
        String const invalidJson = "{ invalid_json: [ }";
        Storage::MemoryStream stream(invalidJson.asPointer(), invalidJson.getLength(), true);
        REQUIRE(stream.isOpen());

        JsonReader reader(stream);
        auto root = reader.getRoot();

        REQUIRE(!root.hasMember("name"));
        REQUIRE(root.getString("name", "fallback") == "fallback");
        REQUIRE(root.getInt("age", 99) == 99);
    }

    SECTION("JsonReader parses top-level array")
    {
        String const json = "[1, 2, 3]";
        Storage::MemoryStream stream(json.asPointer(), json.getLength(), true);
        REQUIRE(stream.isOpen());

        JsonReader reader(stream);
        auto root = reader.getRoot();

        REQUIRE(root.isArray());
        REQUIRE(root.getCount() == 3u);
        REQUIRE(root.getMember(0).asInt64() == 1);
        REQUIRE(root.getMember(1).asInt64() == 2);
        REQUIRE(root.getMember(2).asInt64() == 3);
    }
}
