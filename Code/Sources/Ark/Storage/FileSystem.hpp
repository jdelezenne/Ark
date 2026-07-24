#pragma once

#include "Ark/Collections/Array.hpp"
#include "Ark/Core/Result.hpp"
#include "Ark/Core/Types.hpp"
#include "Ark/Storage/Path.hpp"
#include "Ark/Storage/Types.hpp"
#include "Ark/Strings/String.hpp"
#include "Ark/Strings/StringSlice.hpp"
#include <functional>

namespace Ark::Storage
{
    class FileSystem
    {
        FileSystem() = delete;

    public:
        /// Normalizes the specified path to a system path.
        /// @param path The path to normalize.
        /// @return The normalized path.
        static String normalizePath(StringSlice path);
        static String normalizePath(Path const& path)
        {
            return normalizePath(path.string());
        }

        /// Determines whether the specified path refers to a file or a directory.
        /// @param path The path to check.
        /// @return true if the path refers to an existing file or directory; otherwise, false.
        static Result<bool> exists(StringSlice path);
        static Result<bool> exists(Path const& path)
        {
            return exists(path.string());
        }

        /// Determines whether the specified directory exists.
        /// @param path The directory to check.
        /// @return true if the path refers to an existing directory; otherwise, false.
        static Result<bool> directoryExists(StringSlice path);
        static Result<bool> directoryExists(Path const& path)
        {
            return directoryExists(path.string());
        }

        /// Creates all directories in the specified path.
        /// Succeeds silently if directories already exist. Parent paths are created as needed.
        /// @param path The directory path to create.
        /// @return Success or failure outcome.
        static Outcome createDirectory(StringSlice path);
        static Outcome createDirectory(Path const& path)
        {
            return createDirectory(path.string());
        }

        /// Deletes an empty directory. Fails if directory is not empty or doesn't exist.
        /// @param path The directory path to delete.
        /// @return Success or failure outcome.
        static Outcome deleteDirectory(StringSlice path);
        static Outcome deleteDirectory(Path const& path)
        {
            return deleteDirectory(path.string());
        }

        /// Recursively deletes a directory and all its contents (subdirectories and files).
        /// Partial failures may leave the directory partially deleted on error.
        /// @param path The directory path to delete recursively.
        /// @return Success or failure outcome.
        static Outcome deleteDirectoryRecursively(StringSlice path);
        static Outcome deleteDirectoryRecursively(Path const& path)
        {
            return deleteDirectoryRecursively(path.string());
        }

        using EnumerateDirectoryCallback = std::function<bool(Path const& path, bool isFile)>;

        /// Enumerates immediate children (non-recursive) of a directory.
        /// Callback returns true to continue, false to stop enumeration.
        /// Callback arguments: path relative to start directory, isFile flag.
        /// @param path The directory path to enumerate.
        /// @param callback Function to invoke for each entry.
        /// @return Success or failure outcome.
        static Outcome enumerateDirectory(StringSlice path, EnumerateDirectoryCallback callback);
        static Outcome enumerateDirectory(Path const& path, EnumerateDirectoryCallback callback)
        {
            return enumerateDirectory(path.string(), Ark::move(callback));
        }

        /// Enumerates a directory recursively or non-recursively.
        /// Recursion order: depth-first. Callback can control traversal with return value.
        /// @param path The directory path to enumerate.
        /// @param recursive Whether to traverse subdirectories recursively.
        /// @param callback Function to invoke for each entry.
        /// @return Success or failure outcome.
        static Outcome enumerateDirectory(StringSlice path, bool recursive, EnumerateDirectoryCallback callback);
        static Outcome enumerateDirectory(Path const& path, bool recursive, EnumerateDirectoryCallback callback)
        {
            return enumerateDirectory(path.string(), recursive, Ark::move(callback));
        }

        /// Returns all files in a directory (non-recursive). Names only, no paths included.
        /// @param path The directory path to list files from.
        /// @return Array of file names, or error on failure.
        static Result<Collections::Array<String>> getFiles(StringSlice path);
        static Result<Collections::Array<String>> getFiles(Path const& path)
        {
            return getFiles(path.string());
        }

        /// Determines whether the specified file exists.
        /// @param path The file to check.
        /// @return true if the path refers to an existing file; otherwise, false.
        static Result<bool> fileExists(StringSlice path);
        static Result<bool> fileExists(Path const& path)
        {
            return fileExists(path.string());
        }

        /// Deletes the specified file.
        /// @param path The name of the file to be deleted.
        static Outcome deleteFile(StringSlice path);
        static Outcome deleteFile(Path const& path)
        {
            return deleteFile(path.string());
        }

        /// Gets complete metadata for a file or directory.
        /// @param path The file or directory to query.
        /// @return FileInformation structure with all metadata, or error on failure.
        static Result<FileInformation> getFileInformation(StringSlice path);
        static Result<FileInformation> getFileInformation(Path const& path)
        {
            return getFileInformation(path.string());
        }

        /// Gets the size, in bytes, of the specified file.
        /// @param path The file to check.
        /// @return The size of the current file in bytes.
        static Result<uint64> getFileSize(StringSlice path);
        static Result<uint64> getFileSize(Path const& path)
        {
            return getFileSize(path.string());
        }

        /// Returns the creation time of a file or directory. Platform-specific; may be birth time or modification time.
        /// @param path The file or directory to query.
        /// @return Creation timestamp, or error on failure.
        static Result<Storage::FileTime> getCreationTime(StringSlice path);
        static Result<Storage::FileTime> getCreationTime(Path const& path)
        {
            return getCreationTime(path.string());
        }

        /// Returns the last access time (read or write). Not reliable on all filesystems.
        /// @param path The file or directory to query.
        /// @return Last access timestamp, or error on failure.
        static Result<Storage::FileTime> getLastAccessTime(StringSlice path);
        static Result<Storage::FileTime> getLastAccessTime(Path const& path)
        {
            return getLastAccessTime(path.string());
        }

        /// Returns the last write time. Most reliable timestamp across platforms.
        /// @param path The file or directory to query.
        /// @return Last modification timestamp, or error on failure.
        static Result<Storage::FileTime> getLastModificationTime(StringSlice path);
        static Result<Storage::FileTime> getLastModificationTime(Path const& path)
        {
            return getLastModificationTime(path.string());
        }

        /// Sets the last modification time. Platform-specific precision; typically seconds.
        /// @param path The file or directory to modify.
        /// @param time The new modification time.
        /// @return Success or failure outcome.
        static Outcome setLastModificationTime(StringSlice path, Storage::FileTime const& time);
        static Outcome setLastModificationTime(Path const& path, Storage::FileTime const& time)
        {
            return setLastModificationTime(path.string(), time);
        }

        /// Checks the read-only flag. Behavior varies by platform (Unix ignores this flag).
        /// @param path The file or directory to check.
        /// @return Read-only status, or error on failure.
        static Result<bool> isReadOnly(StringSlice path);
        static Result<bool> isReadOnly(Path const& path)
        {
            return isReadOnly(path.string());
        }

        /// Sets the read-only flag. On Unix, removes write permissions. Partial failures may occur.
        /// @param path The file or directory to modify.
        /// @param value Whether to set read-only (true) or writable (false).
        /// @return Success or failure outcome.
        static Outcome setReadOnly(StringSlice path, bool value);
        static Outcome setReadOnly(Path const& path, bool value)
        {
            return setReadOnly(path.string(), value);
        }

        /// Copies a directory and optionally its contents recursively.
        /// Fails if destination exists (unless overwrite true). Preserves attributes when possible.
        /// @param sourcePath The source directory path.
        /// @param destinationPath The destination path for the copy.
        /// @param isRecursive Whether to copy subdirectories and files.
        /// @param overwriteExisting Whether to overwrite if destination exists.
        /// @return Success or failure outcome.
        static Outcome copyDirectory(StringSlice sourcePath, StringSlice destinationPath, bool isRecursive = false, bool overwriteExisting = false);
        static Outcome copyDirectory(Path const& sourcePath, Path const& destinationPath, bool isRecursive = false, bool overwriteExisting = false)
        {
            return copyDirectory(sourcePath.string(), destinationPath.string(), isRecursive, overwriteExisting);
        }

        /// Moves a directory. On same filesystem, typically just renames. Cross-filesystem moves may copy+delete.
        /// @param sourcePath The source directory path.
        /// @param destinationPath The destination path for the move.
        /// @param overwriteExisting Whether to overwrite if destination exists.
        /// @return Success or failure outcome.
        static Outcome moveDirectory(StringSlice sourcePath, StringSlice destinationPath, bool overwriteExisting = false);
        static Outcome moveDirectory(Path const& sourcePath, Path const& destinationPath, bool overwriteExisting = false)
        {
            return moveDirectory(sourcePath.string(), destinationPath.string(), overwriteExisting);
        }

        /// Renames a directory. Source must exist and be a directory. Destination path must not exist.
        /// @param oldPath The current directory path.
        /// @param newPath The new directory path.
        /// @return Success or failure outcome.
        static Outcome renameDirectory(StringSlice oldPath, StringSlice newPath);
        static Outcome renameDirectory(Path const& oldPath, Path const& newPath)
        {
            return renameDirectory(oldPath.string(), newPath.string());
        }

        /// Copies a file. Fails if destination exists (unless overwrite true). Preserves timestamps when possible.
        /// @param sourcePath The source file path.
        /// @param destinationPath The destination path for the copy.
        /// @param overwriteExisting Whether to overwrite if destination exists.
        /// @return Success or failure outcome.
        static Outcome copyFile(StringSlice sourcePath, StringSlice destinationPath, bool overwriteExisting = false);
        static Outcome copyFile(Path const& sourcePath, Path const& destinationPath, bool overwriteExisting = false)
        {
            return copyFile(sourcePath.string(), destinationPath.string(), overwriteExisting);
        }

        /// Moves a file. On same filesystem, typically just renames. Cross-filesystem moves may copy+delete.
        /// @param sourcePath The source file path.
        /// @param destinationPath The destination path for the move.
        /// @param overwriteExisting Whether to overwrite if destination exists.
        /// @return Success or failure outcome.
        static Outcome moveFile(StringSlice sourcePath, StringSlice destinationPath, bool overwriteExisting = false);
        static Outcome moveFile(Path const& sourcePath, Path const& destinationPath, bool overwriteExisting = false)
        {
            return moveFile(sourcePath.string(), destinationPath.string(), overwriteExisting);
        }

        /// Renames a file. Source must exist and be a file. Destination path must not exist.
        /// @param oldPath The current file path.
        /// @param newPath The new file path.
        /// @return Success or failure outcome.
        static Outcome renameFile(StringSlice oldPath, StringSlice newPath);
        static Outcome renameFile(Path const& oldPath, Path const& newPath)
        {
            return renameFile(oldPath.string(), newPath.string());
        }
    };
}
