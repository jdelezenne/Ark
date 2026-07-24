#include "Ark/Storage/BinaryReader.hpp"
#include "Ark/Core/ByteOrder.hpp"

namespace Ark::Storage
{
    BinaryReader::BinaryReader(Stream& stream, Option<ByteOrder> byteOrderParam)
        : stream{stream}
        , byteOrder{byteOrderParam.hasValue() ? *byteOrderParam : getNativeByteOrder()}
        , needSwap{this->byteOrder != getNativeByteOrder()}
    {
    }

    Outcome BinaryReader::close()
    {
        return stream.close();
    }

    Stream& BinaryReader::getStream() const
    {
        return stream;
    }

    ByteOrder BinaryReader::getByteOrder() const
    {
        return byteOrder;
    }

    Result<usize> BinaryReader::getSize() const
    {
        return stream.getSize();
    }

    Result<usize> BinaryReader::getPosition() const
    {
        return stream.getPosition();
    }

    Outcome BinaryReader::setPosition(isize position, SeekOrigin seekOrigin)
    {
        return stream.setPosition(position, seekOrigin);
    }

    Result<usize> BinaryReader::read(void* buffer, usize count)
    {
        ARK_ASSERT(buffer != nullptr);

        return stream.read(buffer, count);
    }

    uint8 BinaryReader::readUInt8()
    {
        uint8 value;
        read(&value, sizeof(uint8));

        return value;
    }

    int8 BinaryReader::readInt8()
    {
        int8 value;
        read(&value, sizeof(int8));

        return value;
    }

    uint16 BinaryReader::readUInt16()
    {
        uint16 value;
        read(&value, sizeof(uint16));

        if (needSwap)
        {
            value = ByteSwap::swapUInt16(value);
        }

        return value;
    }

    int16 BinaryReader::readInt16()
    {
        int16 value;
        read(&value, sizeof(int16));

        if (needSwap)
        {
            value = ByteSwap::swapInt16(value);
        }

        return value;
    }

    uint32 BinaryReader::readUInt32()
    {
        uint32 value;
        read(&value, sizeof(uint32));

        if (needSwap)
        {
            value = ByteSwap::swapUInt32(value);
        }

        return value;
    }

    int32 BinaryReader::readInt32()
    {
        int32 value;
        read(&value, sizeof(int32));

        if (needSwap)
        {
            value = ByteSwap::swapInt32(value);
        }

        return value;
    }

    uint64 BinaryReader::readUInt64()
    {
        uint64 value;
        read(&value, sizeof(uint64));

        if (needSwap)
        {
            value = ByteSwap::swapUInt64(value);
        }

        return value;
    }

    int64 BinaryReader::readInt64()
    {
        int64 value;
        read(&value, sizeof(int64));

        if (needSwap)
        {
            value = ByteSwap::swapInt64(value);
        }

        return value;
    }

    float32 BinaryReader::readFloat32()
    {
        union
        {
            float32 value1;
            uint32 value2;
        } x;
        read(&x.value2, sizeof(float32));

        if (needSwap)
        {
            x.value2 = ByteSwap::swapUInt32(x.value2);
        }

        return x.value1;
    }

    float64 BinaryReader::readFloat64()
    {
        union
        {
            float64 value1;
            uint64 value2;
        } x;
        read(&x.value2, sizeof(float64));

        if (needSwap)
        {
            x.value2 = ByteSwap::swapUInt64(x.value2);
        }

        return x.value1;
    }
}
