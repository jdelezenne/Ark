#pragma once

#include "Ark/Storage/Stream.hpp"

namespace Ark::Storage
{
    /// Stream implementation over caller-owned memory buffer.
    /// Does NOT allocate or manage the buffer lifetime. Caller is responsible for ensuring
    /// the buffer remains valid for the duration of the stream's use. Useful for stack buffers
    /// or read-only data segments. For safe ownership, use MemoryStream instead.
    class UnsafeMemoryStream final : public Stream
    {
    private:
        bool const readOnly;

        uint8* buffer;
        usize capacity;

        usize position{0};

    public:
        /// Creates an unsafe stream over an external buffer.
        /// The buffer is not copied and must remain valid until the stream is destroyed.
        /// @param buffer Pointer to external backing buffer. MUST NOT be freed while stream is in use.
        /// @param capacity Total usable size of the buffer in bytes.
        /// @param isReadOnly True to disable writes (converts to read-only view of buffer).
        UnsafeMemoryStream(void* buffer, usize capacity, bool isReadOnly = false);

        ~UnsafeMemoryStream();

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
