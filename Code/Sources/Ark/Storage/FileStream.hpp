#pragma once

#include "Ark/Storage/File.hpp"
#include "Ark/Storage/Path.hpp"
#include "Ark/Storage/Stream.hpp"

namespace Ark::Storage
{
    /// Stream implementation backed by a file handle.
    /// Provides standard stream interface over file I/O. Handles platform differences transparently.
    /// File is opened immediately on construction; failures are silently ignored (use isOpen to check).
    class FileStream final : public Stream
    {
    private:
        UniquePointer<File> file;

    public:
        FileStream() = default;

        /// Opens a file stream from a path string.
        /// @param path File path (automatically converted to platform-native format).
        /// @param mode High-level file opening semantics (create/truncate/open behavior).
        /// @param descriptor Optional access and sharing permissions.
        FileStream(StringSlice path, FileMode mode, FileDescriptor const& descriptor = {});

        /// Opens a file stream from a Path object.
        /// @param path File path as Path object.
        /// @param mode High-level file opening semantics.
        /// @param descriptor Optional access and sharing permissions.
        FileStream(Path const& path, FileMode mode, FileDescriptor const& descriptor = {})
            : FileStream(path.string(), mode, descriptor)
        {
        }

        ~FileStream();

        /// Returns the path of the currently open file (generic format).
        /// @return File path as string.
        String getPath() const;

        bool isOpen() const override;

        bool canSeek() const override;

        bool canRead() const override;

        bool canWrite() const override;

        /// Opens a file stream from a path string.
        /// Closes any previously open file automatically.
        /// @param path File path (converted to platform-native format).
        /// @param mode High-level file opening semantics (create/truncate/open behavior).
        /// @param descriptor Optional access and sharing permissions.
        /// @return Success or failure outcome.
        Outcome open(StringSlice path, FileMode mode, FileDescriptor const& descriptor = {});

        /// Opens a file stream from a Path object.
        /// Closes any previously open file automatically.
        /// @param path File path as Path object.
        /// @param mode High-level file opening semantics.
        /// @param descriptor Optional access and sharing permissions.
        /// @return Success or failure outcome.
        Outcome open(Path const& path, FileMode mode, FileDescriptor const& descriptor = {})
        {
            return open(path.string(), mode, descriptor);
        }

        Outcome close() override;

        Outcome flush() override;

        Result<usize> getSize() const override;

        Result<usize> getPosition() const override;

        Outcome setPosition(isize position, SeekOrigin seekOrigin = SeekOrigin::Begin) override;

        Result<usize> read(void* buffer, usize count) override;

        Result<usize> write(void const* buffer, usize count) override;
    };
}
