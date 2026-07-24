#pragma once

#include "Ark/Core/ByteOrder.hpp"
#include "Ark/Core/Types.hpp"
#include "Ark/Storage/Stream.hpp"

namespace Ark::Storage
{
    /// Binary output helper that writes typed values to a Stream.
    /// Supports configurable byte order and transparent endianness conversion.
    class BinaryWriter final
    {
    private:
        Stream& stream;
        ByteOrder byteOrder;
        bool needSwap;

    public:
        /// Creates a binary writer over a stream.
        /// @param stream Output stream.
        /// @param byteOrder Optional byte order override.
        BinaryWriter(Stream& stream, Option<ByteOrder> byteOrder = none);

        ~BinaryWriter() = default;

        /// Closes the underlying stream.
        /// @return Success or failure outcome.
        Outcome close();

        /// Returns the underlying stream.
        /// @return Stream reference.
        Stream& getStream() const;

        /// Returns the byte order used for typed writes.
        /// @return Active byte order.
        ByteOrder getByteOrder() const;

        Result<usize> getSize() const;

        Result<usize> getPosition() const;

        Outcome setPosition(isize position, SeekOrigin seekOrigin = SeekOrigin::Begin);

        /// Writes raw bytes.
        /// @param buffer Source buffer.
        /// @param count Number of bytes to write.
        /// @return Number of bytes written.
        Result<usize> write(void const* buffer, usize count);

        /// Writes an unsigned 8-bit integer. Throws on write failure.
        void writeUInt8(uint8 value);

        /// Writes a signed 8-bit integer. Throws on write failure.
        void writeInt8(int8 value);

        /// Writes an unsigned 16-bit integer with byte-order conversion. Throws on write failure.
        void writeUInt16(uint16 value);

        /// Writes a signed 16-bit integer with byte-order conversion. Throws on write failure.
        void writeInt16(int16 value);

        /// Writes an unsigned 32-bit integer with byte-order conversion. Throws on write failure.
        void writeUInt32(uint32 value);

        /// Writes a signed 32-bit integer with byte-order conversion. Throws on write failure.
        void writeInt32(int32 value);

        /// Writes an unsigned 64-bit integer with byte-order conversion. Throws on write failure.
        void writeUInt64(uint64 value);

        /// Writes a signed 64-bit integer with byte-order conversion. Throws on write failure.
        void writeInt64(int64 value);

        /// Writes a 32-bit floating-point value with byte-order conversion. Throws on write failure.
        void writeFloat32(float32 value);

        /// Writes a 64-bit floating-point value with byte-order conversion. Throws on write failure.
        void writeFloat64(float64 value);
    };
}
