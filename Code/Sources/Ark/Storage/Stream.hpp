#pragma once

#include "Ark/Core/Result.hpp"
#include "Ark/Core/Types.hpp"
#include "Ark/Storage/Types.hpp"

namespace Ark::Storage
{
    /// Abstract byte stream interface.
    /// Defines virtual methods for capability detection (seek, read, write) and I/O operations.
    /// All methods return Outcome or Result<T> for consistent error handling across implementations.
    class Stream
    {
    public:
        virtual ~Stream() = default;

        /// Checks whether the stream is open and usable.
        /// @return True if the stream is open and available for I/O.
        virtual bool isOpen() const = 0;

        /// Checks whether the stream supports position changes (seeking).
        /// @return True if seek operations are supported.
        virtual bool canSeek() const = 0;

        /// Checks whether the stream supports reading.
        /// @return True if read operations are supported.
        virtual bool canRead() const = 0;

        /// Checks whether the stream supports writing.
        /// @return True if write operations are supported.
        virtual bool canWrite() const = 0;

        /// Closes the stream and releases any associated resources.
        /// Safe to call multiple times. Subsequent I/O operations will fail.
        /// @return Success or failure outcome.
        virtual Outcome close() = 0;

        /// Flushes any buffered output to the underlying storage.
        /// No-op for unbuffered streams. Should be called before closing.
        /// @return Success or failure outcome.
        virtual Outcome flush() = 0;

        /// Gets the total size of the stream in bytes.
        /// For streams with undefined size (pipes, sockets), behavior is implementation-defined.
        /// @return Size result (bytes), or error if size cannot be determined.
        virtual Result<usize> getSize() const = 0;

        /// Gets the current read/write position in the stream.
        /// Position is zero-based. Position beyond size is valid for write operations.
        /// @return Position result (0-based index from start), or error if position unavailable.
        virtual Result<usize> getPosition() const = 0;

        /// Sets stream position relative to a seek origin.
        /// Negative offsets from Begin clamp to 0. Seeking past EOF is allowed for writes.
        /// @param position Offset in bytes (can be negative for relative seeks).
        /// @param seekOrigin Origin used to interpret the offset.
        /// @return Success or failure outcome.
        virtual Outcome setPosition(isize position, SeekOrigin seekOrigin = SeekOrigin::Begin) = 0;

        /// Reads bytes from the stream into a buffer starting at current position.
        /// Advances position by actual bytes read. Partial reads are allowed (return < count).
        /// @param buffer Destination buffer.
        /// @param count Maximum number of bytes to read.
        /// @return Number of bytes actually read, or error on I/O failure.
        virtual Result<usize> read(void* buffer, usize count) = 0;

        /// Writes bytes from a buffer to the stream starting at current position.
        /// Advances position by actual bytes written. Partial writes are allowed (return < count).
        /// @param buffer Source buffer.
        /// @param count Number of bytes to write.
        /// @return Number of bytes actually written, or error on I/O failure.
        virtual Result<usize> write(void const* buffer, usize count) = 0;
    };
}
