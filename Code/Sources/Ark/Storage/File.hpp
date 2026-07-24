#pragma once

#include "Ark/Core/Result.hpp"
#include "Ark/Memory/UniquePointer.hpp"
#include "Ark/Storage/Path.hpp"
#include "Ark/Storage/Types.hpp"
#include "Ark/Strings/String.hpp"
#include "Ark/Strings/StringSlice.hpp"

namespace Ark::Storage
{
    /// Abstract file interface.
    /// Platform-specific implementations handle I/O with proper error reporting.
    /// All paths are normalized to forward slashes internally; native paths are converted.
    class File
    {
    public:
        virtual ~File() = default;

        /// Returns the current file path.
        /// @return Current file path.
        virtual String getPath() const = 0;

        /// Creates a platform-specific file.
        /// @return Unique pointer to a new File instance.
        static UniquePointer<File> create();

        /// Creates and optionally opens a platform-specific file.
        /// @param path File path to open.
        /// @param mode File open mode.
        /// @param descriptor Optional access and sharing descriptor.
        /// @return Unique pointer to file instance (may be closed on failure).
        static UniquePointer<File> create(StringSlice path, FileMode mode, FileDescriptor const& descriptor = {});
        static UniquePointer<File> create(Path const& path, FileMode mode, FileDescriptor const& descriptor = {})
        {
            return create(path.string(), mode, descriptor);
        }

        /// Checks whether a file is currently open.
        /// @return True if file is open and usable.
        virtual bool isOpen() const = 0;

        /// Opens a file with the specified mode and optional creation/opening parameters.
        /// Closes any previously open file automatically.
        /// @param path File path to open.
        /// @param mode File open mode.
        /// @param descriptor Optional access and sharing descriptor.
        /// @return Success or failure outcome.
        virtual Outcome open(StringSlice path, FileMode mode, FileDescriptor const& descriptor = {}) = 0;
        Outcome open(Path const& path, FileMode mode, FileDescriptor const& descriptor = {})
        {
            return open(path.string(), mode, descriptor);
        }

        /// Closes the file and releases system resources. Safe to call on closed files.
        /// @return Success or failure outcome.
        virtual Outcome close() = 0;

        /// Reads bytes from the current position. Does not advance position on failure.
        /// @param buffer Destination buffer.
        /// @param count Number of bytes to read.
        /// @return Number of bytes read, or error on failure.
        virtual Result<usize> read(void* buffer, usize count) = 0;

        /// Writes bytes at the current position. Fails immediately if file opened read-only.
        /// @param buffer Source buffer.
        /// @param count Number of bytes to write.
        /// @return Number of bytes written, or error on failure.
        virtual Result<usize> write(void const* buffer, usize count) = 0;

        /// Flushes buffered writes to disk. Succeeds silently if no buffering used.
        /// @return Success or failure outcome.
        virtual Outcome flush() = 0;

        /// Returns file size in bytes. For special files (pipes, sockets) behavior is undefined.
        /// @return File size, or error if unavailable.
        virtual Result<usize> getSize() const = 0;

        /// Returns current read/write position. Position is unsigned; clamping used for negative seeks.
        /// @return Current position, or error if unavailable.
        virtual Result<usize> getPosition() const = 0;

        /// Seeks to an offset from a specified origin. Supports seeking past EOF for writes.
        /// @param position Position offset from origin.
        /// @param seekOrigin Origin to use (Begin, Current, or End).
        /// @return Success or failure outcome.
        virtual Outcome setPosition(isize position, SeekOrigin seekOrigin = SeekOrigin::Begin) = 0;

        /// Returns file modification time as a 64-bit timestamp. Platform-specific representation.
        /// @return File modification time, or error if unavailable.
        virtual Result<uint64> getModificationTime() const = 0;

        /// Opens a file, reads all bytes into an array, and closes the file atomically.
        /// Returns empty array on open failure (not on read failure).
        /// @param path File path to read from.
        /// @return Array containing all file bytes, or error on failure.
        static Result<Collections::Array<uint8>> readAllBinary(StringSlice path);
        static Result<Collections::Array<uint8>> readAllBinary(Path const& path)
        {
            return readAllBinary(path.string());
        }

        /// Opens a text file, reads all text into a string, and closes the file atomically.
        /// Text is interpreted as UTF-8; invalid sequences are preserved as-is.
        /// @param path File path to read from.
        /// @return String containing all file text, or error on failure.
        static Result<String> readAllText(StringSlice path);
        static Result<String> readAllText(Path const& path)
        {
            return readAllText(path.string());
        }

        /// Creates a new file (or truncates existing), writes all data, and closes atomically.
        /// Fails if directory doesn't exist. File descriptor ignored if file already exists.
        /// @param path File path to write to.
        /// @param data Bytes to write to the file.
        /// @return Success or failure outcome.
        static Outcome writeAllBinary(StringSlice path, Collections::Array<uint8> const& data);
        static Outcome writeAllBinary(Path const& path, Collections::Array<uint8> const& data)
        {
            return writeAllBinary(path.string(), data);
        }

        /// Creates a new file (or truncates existing), writes text, and closes atomically.
        /// Text written as UTF-8. Directory must exist. File descriptor ignored if file already exists.
        /// @param path File path to write to.
        /// @param text Text to write to the file.
        /// @return Success or failure outcome.
        static Outcome writeAllText(StringSlice path, StringSlice text);
        static Outcome writeAllText(Path const& path, StringSlice text)
        {
            return writeAllText(path.string(), text);
        }
    };
}
