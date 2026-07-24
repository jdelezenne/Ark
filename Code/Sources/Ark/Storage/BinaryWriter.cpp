#include "Ark/Storage/BinaryWriter.hpp"
#include "Ark/Core/ByteOrder.hpp"

namespace Ark::Storage
{
    BinaryWriter::BinaryWriter(Stream& stream, Option<ByteOrder> byteOrder)
        : stream{stream}
        , byteOrder{ByteOrder::LittleEndian}
    {
        if (byteOrder.hasValue())
        {
            this->byteOrder = *byteOrder;
        }

        needSwap = (this->byteOrder != ByteOrder::LittleEndian);
    }

    Outcome BinaryWriter::close()
    {
        return stream.close();
    }

    Stream& BinaryWriter::getStream() const
    {
        return stream;
    }

    ByteOrder BinaryWriter::getByteOrder() const
    {
        return byteOrder;
    }

    Result<usize> BinaryWriter::getSize() const
    {
        return stream.getSize();
    }

    Result<usize> BinaryWriter::getPosition() const
    {
        return stream.getPosition();
    }

    Outcome BinaryWriter::setPosition(isize position, SeekOrigin seekOrigin)
    {
        return stream.setPosition(position, seekOrigin);
    }

    Result<usize> BinaryWriter::write(void const* buffer, usize count)
    {
        ARK_ASSERT(buffer != nullptr);

        return stream.write(buffer, count);
    }

    void BinaryWriter::writeUInt8(uint8 value)
    {
        write(&value, sizeof(uint8));
    }

    void BinaryWriter::writeInt8(int8 value)
    {
        write(&value, sizeof(int8));
    }

    void BinaryWriter::writeUInt16(uint16 value)
    {
        if (needSwap)
        {
            value = ByteSwap::toLittleEndianUInt16(value);
        }

        write(&value, sizeof(uint16));
    }

    void BinaryWriter::writeInt16(int16 value)
    {
        if (needSwap)
        {
            value = ByteSwap::toLittleEndianInt16(value);
        }

        write(&value, sizeof(int16));
    }

    void BinaryWriter::writeUInt32(uint32 value)
    {
        if (needSwap)
        {
            value = ByteSwap::toLittleEndianUInt32(value);
        }

        write(&value, sizeof(uint32));
    }

    void BinaryWriter::writeInt32(int32 value)
    {
        if (needSwap)
        {
            value = ByteSwap::toLittleEndianInt32(value);
        }

        write(&value, sizeof(int32));
    }

    void BinaryWriter::writeUInt64(uint64 value)
    {
        if (needSwap)
        {
            value = ByteSwap::toLittleEndianUInt64(value);
        }

        write(&value, sizeof(uint64));
    }

    void BinaryWriter::writeInt64(int64 value)
    {
        if (needSwap)
        {
            value = ByteSwap::toLittleEndianInt64(value);
        }

        write(&value, sizeof(int64));
    }

    void BinaryWriter::writeFloat32(float32 value)
    {
        if (needSwap)
        {
            float32 le = ByteSwap::toLittleEndianFloat(value);
            write(&le, sizeof(float32));
            return;
        }

        write(&value, sizeof(float32));
    }

    void BinaryWriter::writeFloat64(float64 value)
    {
        if (needSwap)
        {
            float64 le = ByteSwap::toLittleEndianDouble(value);
            write(&le, sizeof(float64));
            return;
        }

        write(&value, sizeof(float64));
    }
}
