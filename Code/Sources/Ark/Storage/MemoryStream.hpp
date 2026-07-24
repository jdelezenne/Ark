#pragma once

#include "Ark/Storage/Stream.hpp"

namespace Ark::Storage
{
    /// Stream implementation backed by an owned memory buffer.
    /// Allocates its own buffer and manages lifetime. Safe for use after construction.
    /// Supports both read and write access with full seek capabilities.
    class MemoryStream final : public Stream
    {
    private:
        bool const readOnly;

        uint8 const* buffer;
        usize size;

        usize position{0};

    public:
        /// Creates an empty in-memory stream. Buffer grows on write.
        /// @param isReadOnly True to prevent write operations (read-only buffer).
        MemoryStream(bool isReadOnly = false);

        /// Creates a stream initialized with data from an external buffer.
        /// The buffer is copied; external buffer can be freed immediately.
        /// @param buffer Source data to copy into stream.
        /// @param count Number of bytes to copy.
        /// @param isReadOnly True to disable writes after initialization.
        MemoryStream(void const* buffer, usize count, bool isReadOnly = false);

        ~MemoryStream();

        /// Checks if this stream is read-only. Read-only streams reject write operations.
        /// @return True if write operations are disabled.
        bool isReadOnly() const;

        bool isOpen() const override;

        bool canSeek() const override;

        bool canRead() const override;

        bool canWrite() const override;

        Outcome close() override;

        Outcome flush() override;

        Result<usize> getSize() const override;

        Result<usize> getPosition() const override;

        Outcome setPosition(isize position, SeekOrigin seekOrigin = SeekOrigin::Begin) override;

        Result<usize> read(void* buffer, usize count) override;

        Result<usize> write(void const* buffer, usize count) override;
    };
}
