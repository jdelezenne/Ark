#pragma once

#include "Ark/Core/ByteOrder.hpp"
#include "Ark/Core/Types.hpp"
#include "Ark/Storage/Stream.hpp"

namespace Ark::Storage
{
    /// Binary input helper that reads typed values from a Stream.
    /// Supports configurable byte order and transparent endianness conversion.
    class BinaryReader final
    {
    private:
        Stream& stream;
        ByteOrder byteOrder;
        bool needSwap;

    public:
        /// Creates a binary reader over a stream.
        /// @param stream Input stream.
        /// @param byteOrder Optional byte order override.
        BinaryReader(Stream& stream, Option<ByteOrder> byteOrder = none);

        ~BinaryReader() = default;

        /// Closes the underlying stream.
        /// @return Success or failure outcome.
        Outcome close();

        /// Returns the underlying stream.
        /// @return Stream reference.
        Stream& getStream() const;

        /// Returns the byte order used for typed reads.
        /// @return Active byte order.
        ByteOrder getByteOrder() const;

        Result<usize> getSize() const;

        Result<usize> getPosition() const;

        Outcome setPosition(isize position, SeekOrigin seekOrigin = SeekOrigin::Begin);

        /// Reads raw bytes.
        /// @param buffer Destination buffer.
        /// @param count Number of bytes to read.
        /// @return Number of bytes read.
        Result<usize> read(void* buffer, usize count);

        /// Reads an unsigned 8-bit integer. Throws on read failure.
        uint8 readUInt8();

        /// Reads a signed 8-bit integer. Throws on read failure.
        int8 readInt8();

        /// Reads an unsigned 16-bit integer with byte-order conversion. Throws on read failure.
        uint16 readUInt16();

        /// Reads a signed 16-bit integer with byte-order conversion. Throws on read failure.
        int16 readInt16();

        /// Reads an unsigned 32-bit integer with byte-order conversion. Throws on read failure.
        uint32 readUInt32();

        /// Reads a signed 32-bit integer with byte-order conversion. Throws on read failure.
        int32 readInt32();

        /// Reads an unsigned 64-bit integer with byte-order conversion. Throws on read failure.
        uint64 readUInt64();

        /// Reads a signed 64-bit integer with byte-order conversion. Throws on read failure.
        int64 readInt64();

        /// Reads a 32-bit floating-point value with byte-order conversion. Throws on read failure.
        float32 readFloat32();

        /// Reads a 64-bit floating-point value with byte-order conversion. Throws on read failure.
        float64 readFloat64();
    };
}
