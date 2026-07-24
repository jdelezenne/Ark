#pragma once

#include "Ark/Core/Flags.hpp"
#include "Ark/Core/Macros.hpp"
#include "Ark/Core/Types.hpp"
#include "Ark/Strings/String.hpp"
#include "Ark/Core/DateTime.hpp"

namespace Ark::Storage
{
    /// Specifies the position in a stream to use for seeking.
    enum class SeekOrigin
    {
        Begin,   /// Seek from the start of the stream.
        Current, /// Seek from the current position within a stream.
        End,     /// Seek from the end of the stream (supports negative offsets).
    };

    /// Bitflags for file open modes. Controls how files are accessed and interpreted.
    enum class OpenMode : uint
    {
        Invalid,
        Read = ARK_BIT(0),       /// Open for reading.
        Write = ARK_BIT(1),      /// Open for writing.
        Append = ARK_BIT(2),     /// Append writes to end of file.
        Binary = ARK_BIT(3),     /// Binary mode (no newline translation).
        Text = ARK_BIT(4),       /// Text mode (platform-specific newline handling).
        Update = ARK_BIT(5),     /// Allow read+write on same file handle.
        CreatePath = ARK_BIT(6), /// Create parent directories if they don't exist.
    };

    ARK_FLAG_ENUM(OpenMode)

    /// High-level file opening semantics. Specifies behavior if file exists or doesn't.
    enum class FileMode
    {
        CreateNew,        /// Create a new file. Fails if file already exists.
        CreateAlways,     /// Create a new file or truncate if exists. Preferred for write-only operations.
        OpenExisting,     /// Open existing file. Fails if doesn't exist.
        OpenAlways,       /// Open file or create if doesn't exist.
        TruncateExisting, /// Open and truncate existing file. Fails if doesn't exist.
    };

    /// Bitflags for file access permissions. Determines what operations are allowed.
    enum class FileAccess : uint
    {
        Read = ARK_BIT(0),
        Write = ARK_BIT(1),

        ReadWrite = Read | Write,
    };

    ARK_FLAG_ENUM(FileAccess)

    /// Bitflags controlling whether other processes can access the file simultaneously.
    /// Enforcement is platform-dependent; on Unix most flags are advisory only.
    enum class FileShare : uint
    {
        None = 0,

        Read = ARK_BIT(0),   /// Allow other processes to read this file.
        Write = ARK_BIT(1),  /// Allow other processes to write this file.
        Delete = ARK_BIT(2), /// Allow other processes to delete this file.

        ReadWrite = Read | Write,
        All = ReadWrite | Delete,
    };

    ARK_FLAG_ENUM(FileShare)

    /// Specifies the access permissions and sharing mode for file operations.
    struct FileDescriptor final
    {
        FileAccess access{FileAccess::ReadWrite};
        FileShare share{FileShare::None};
    };

    /// Indicates the filesystem entry type.
    enum class FileType
    {
        Unknown,   /// Unknown or unable to determine type.
        File,      /// A regular file.
        Directory, /// A directory.
        SymLink,   /// A symbolic link (platform-dependent support).
    };

    /// Alias for filesystem timestamps. Typically seconds since Unix epoch.
    using FileTime = DateTime;

    /// Describes properties of a file or directory retrieved from the filesystem.
    struct FileInformation final
    {
        // Type and existence
        FileType type{FileType::Unknown};
        bool isFile{false};
        bool isDirectory{false};

        // Common attributes
        String path;
        bool exists{false};
        FileTime creationTime;
        FileTime lastAccessTime;
        FileTime lastModificationTime;

        // File-specific attributes
        bool isReadOnly{false};
        usize size{0};
    };
}
