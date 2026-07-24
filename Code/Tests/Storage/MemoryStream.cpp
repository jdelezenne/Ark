#include "Ark/Testing/Test.hpp"

#include "Ark/Storage/MemoryStream.hpp"
#include "Ark/Storage/UnsafeMemoryStream.hpp"

ARK_TEST_CASE("MemoryStream", "[memorystream]")
{
    SECTION("capabilities and initial state")
    {
        Ark::uint8 const data[] = {1, 2, 3, 4, 5};
        Ark::Storage::MemoryStream stream(data, sizeof(data), true);

        REQUIRE(stream.isOpen());
        REQUIRE(stream.canRead());
        REQUIRE(!stream.canWrite());
        REQUIRE(stream.canSeek());
        REQUIRE(stream.isReadOnly());

        auto size = stream.getSize();
        REQUIRE(size.isOk());
        REQUIRE(size.getValue() == 5);

        auto position = stream.getPosition();
        REQUIRE(position.isOk());
        REQUIRE(position.getValue() == 0);
    }

    SECTION("sequential reads update position")
    {
        Ark::uint8 const data[] = {1, 2, 3, 4, 5};
        Ark::Storage::MemoryStream stream(data, sizeof(data), true);

        Ark::uint8 buffer[3];
        auto readResult = stream.read(buffer, 3);
        REQUIRE(readResult.isOk());
        REQUIRE(readResult.getValue() == 3);
        REQUIRE(buffer[0] == 1);
        REQUIRE(buffer[1] == 2);
        REQUIRE(buffer[2] == 3);

        auto position = stream.getPosition();
        REQUIRE(position.isOk());
        REQUIRE(position.getValue() == 3);

        readResult = stream.read(buffer, 3);
        REQUIRE(readResult.isOk());
        REQUIRE(readResult.getValue() == 2);
        REQUIRE(buffer[0] == 4);
        REQUIRE(buffer[1] == 5);
    }

    SECTION("seek from begin")
    {
        Ark::uint8 const data[] = {10, 20, 30, 40, 50};
        Ark::Storage::MemoryStream stream(data, sizeof(data), true);

        auto result = stream.setPosition(2, Ark::Storage::SeekOrigin::Begin);
        REQUIRE(result.isOk());

        auto position = stream.getPosition();
        REQUIRE(position.isOk());
        REQUIRE(position.getValue() == 2);

        Ark::uint8 value;
        auto readResult = stream.read(&value, 1);
        REQUIRE(readResult.isOk());
        REQUIRE(value == 30);
    }

    SECTION("seek from current and end")
    {
        Ark::uint8 const data[] = {10, 20, 30, 40, 50};
        Ark::Storage::MemoryStream stream(data, sizeof(data), true);

        REQUIRE(stream.setPosition(1, Ark::Storage::SeekOrigin::Current).isOk());
        auto position = stream.getPosition();
        REQUIRE(position.isOk());
        REQUIRE(position.getValue() == 1);

        REQUIRE(stream.setPosition(-1, Ark::Storage::SeekOrigin::End).isOk());
        position = stream.getPosition();
        REQUIRE(position.isOk());
        REQUIRE(position.getValue() == 4);

        Ark::uint8 value;
        auto readResult = stream.read(&value, 1);
        REQUIRE(readResult.isOk());
        REQUIRE(value == 50);
    }

    SECTION("read beyond end")
    {
        Ark::uint8 const data[] = {1, 2, 3};
        Ark::Storage::MemoryStream stream(data, sizeof(data), true);
        stream.setPosition(0, Ark::Storage::SeekOrigin::End);

        Ark::uint8 buffer[10];
        auto readResult = stream.read(buffer, 10);
        REQUIRE(!readResult.isOk());
    }
}

ARK_TEST_CASE("UnsafeMemoryStream", "[unsafememorystream]")
{
    SECTION("capabilities and size")
    {
        Ark::uint8 buffer[10] = {1, 2, 3, 4, 5, 0, 0, 0, 0, 0};
        Ark::Storage::UnsafeMemoryStream stream(buffer, sizeof(buffer), false);

        REQUIRE(stream.isOpen());
        REQUIRE(stream.canRead());
        REQUIRE(stream.canWrite());
        REQUIRE(stream.canSeek());

        auto size = stream.getSize();
        REQUIRE(size.isOk());
        REQUIRE(size.getValue() == 10);
    }

    SECTION("write updates position")
    {
        Ark::uint8 buffer[10] = {1, 2, 3, 4, 5, 0, 0, 0, 0, 0};
        Ark::Storage::UnsafeMemoryStream stream(buffer, sizeof(buffer), false);

        stream.setPosition(5, Ark::Storage::SeekOrigin::Begin);
        Ark::uint8 writeData[] = {6, 7, 8};
        auto writeResult = stream.write(writeData, sizeof(writeData));
        REQUIRE(writeResult.isOk());
        REQUIRE(writeResult.getValue() == 3);

        auto position = stream.getPosition();
        REQUIRE(position.isOk());
        REQUIRE(position.getValue() == 8);
    }

    SECTION("read returns written bytes")
    {
        Ark::uint8 buffer[10] = {1, 2, 3, 4, 5, 0, 0, 0, 0, 0};
        Ark::Storage::UnsafeMemoryStream stream(buffer, sizeof(buffer), false);

        stream.setPosition(5, Ark::Storage::SeekOrigin::Begin);
        Ark::uint8 writeData[] = {6, 7, 8};
        auto writeResult = stream.write(writeData, sizeof(writeData));
        REQUIRE(writeResult.isOk());

        stream.setPosition(0, Ark::Storage::SeekOrigin::Begin);
        Ark::uint8 readBuffer[8];
        auto readResult = stream.read(readBuffer, 8);
        REQUIRE(readResult.isOk());
        REQUIRE(readResult.getValue() == 8);
        REQUIRE(readBuffer[0] == 1);
        REQUIRE(readBuffer[4] == 5);
        REQUIRE(readBuffer[5] == 6);
        REQUIRE(readBuffer[6] == 7);
        REQUIRE(readBuffer[7] == 8);
    }

    SECTION("seek operations")
    {
        Ark::uint8 buffer[5] = {10, 20, 30, 40, 50};
        Ark::Storage::UnsafeMemoryStream stream(buffer, sizeof(buffer), false);

        auto result = stream.setPosition(2, Ark::Storage::SeekOrigin::Begin);
        REQUIRE(result.isOk());

        auto position = stream.getPosition();
        REQUIRE(position.isOk());
        REQUIRE(position.getValue() == 2);

        REQUIRE(stream.setPosition(1, Ark::Storage::SeekOrigin::Current).isOk());
        position = stream.getPosition();
        REQUIRE(position.isOk());
        REQUIRE(position.getValue() == 3);

        REQUIRE(stream.setPosition(-1, Ark::Storage::SeekOrigin::End).isOk());
        position = stream.getPosition();
        REQUIRE(position.isOk());
        REQUIRE(position.getValue() == 4);

        REQUIRE(stream.setPosition(-2, Ark::Storage::SeekOrigin::Current).isOk());
        position = stream.getPosition();
        REQUIRE(position.isOk());
        REQUIRE(position.getValue() == 2);
    }

    SECTION("read-only mode")
    {
        Ark::uint8 buffer[5] = {1, 2, 3, 4, 5};
        Ark::Storage::UnsafeMemoryStream stream(buffer, sizeof(buffer), true);

        REQUIRE(stream.canRead());
        REQUIRE(!stream.canWrite());

        Ark::uint8 data[] = {99};
        auto writeResult = stream.write(data, 1);
        REQUIRE(!writeResult.isOk());

        Ark::uint8 readBuffer[5];
        auto readResult = stream.read(readBuffer, 5);
        REQUIRE(readResult.isOk());
        REQUIRE(readResult.getValue() == 5);
        REQUIRE(readBuffer[0] == 1);
    }

    SECTION("flush")
    {
        Ark::uint8 buffer[5];
        Ark::Storage::UnsafeMemoryStream stream(buffer, sizeof(buffer), false);
        auto result = stream.flush();
        REQUIRE(result.isOk());
    }

    SECTION("partial reads and writes")
    {
        Ark::uint8 buffer[5] = {0, 0, 0, 0, 0};
        Ark::Storage::UnsafeMemoryStream stream(buffer, sizeof(buffer), false);

        Ark::uint8 writeData1[] = {1, 2};
        auto writeResult = stream.write(writeData1, sizeof(writeData1));
        REQUIRE(writeResult.isOk());
        REQUIRE(writeResult.getValue() == 2);

        auto position = stream.getPosition();
        REQUIRE(position.getValue() == 2);

        Ark::uint8 writeData2[] = {3, 4, 5};
        writeResult = stream.write(writeData2, sizeof(writeData2));
        REQUIRE(writeResult.isOk());
        REQUIRE(writeResult.getValue() == 3);

        position = stream.getPosition();
        REQUIRE(position.getValue() == 5);

        stream.setPosition(0, Ark::Storage::SeekOrigin::Begin);
        Ark::uint8 readBuffer[5];
        auto readResult = stream.read(readBuffer, 5);
        REQUIRE(readResult.isOk());
        REQUIRE(readBuffer[0] == 1);
        REQUIRE(readBuffer[1] == 2);
        REQUIRE(readBuffer[2] == 3);
        REQUIRE(readBuffer[3] == 4);
        REQUIRE(readBuffer[4] == 5);
    }
}
