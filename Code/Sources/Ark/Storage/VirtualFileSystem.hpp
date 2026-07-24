#pragma once

#include "Ark/Collections/Array.hpp"
#include "Ark/Core/Result.hpp"
#include "Ark/Memory/UniquePointer.hpp"
#include "Ark/Storage/File.hpp"
#include "Ark/Storage/FileStream.hpp"
#include "Ark/Storage/Path.hpp"
#include "Ark/Storage/Types.hpp"
#include "Ark/Strings/StringSlice.hpp"

namespace Ark::Storage
{
    /// Read-only VFS that resolves relative paths against a list of mounted roots.
    /// Files are searched in mount order (first match wins). Useful for layered resource loading
    /// (e.g., base assets, mod overrides, user configs). All relative paths are resolved this way.
    class VirtualFileSystem final
    {
    public:
        struct Mount final
        {
            /// Absolute filesystem path where this mount points to.
            Path root;
        };

    private:
        Collections::Array<Mount> mounts;

    public:
        VirtualFileSystem() = default;

        /// Adds an absolute path to the mount list. New mounts checked before existing ones.
        /// @param rootPath Absolute path to add to the mount list.
        void mount(StringSlice rootPath);
        void mount(Path const& rootPath)
        {
            mount(rootPath.string());
        }

        /// Removes a mount from the list. No-op if mount doesn't exist.
        /// @param rootPath Absolute path to remove from the mount list.
        void unmount(StringSlice rootPath);
        void unmount(Path const& rootPath)
        {
            unmount(rootPath.string());
        }

        /// Clears all mounts.
        /// @return (void)
        void clear();

        /// Returns the current mount list (in search order).
        /// @return Array of Mount structures in search order.
        Collections::Array<Mount> const& getMounts() const;

        /// Checks if a relative path exists in any mount. Checks in mount order, stops at first match.
        /// @param relativePath Relative path to check.
        /// @return True if the path exists in any mount.
        bool exists(StringSlice relativePath) const;
        bool exists(Path const& relativePath) const
        {
            return exists(relativePath.string());
        }

        /// Resolves a relative path to a concrete absolute system path by checking mounts in order.
        /// Returns the first matching mount path + relative path combination.
        /// @param relativePath Relative path to resolve.
        /// @return Absolute path if found in any mount, or error if not found.
        Result<Path> resolve(StringSlice relativePath) const;
        Result<Path> resolve(Path const& relativePath) const
        {
            return resolve(relativePath.string());
        }

        /// Opens a file from VFS. Resolves path first, then opens the result.
        /// @param relativePath Relative path to open.
        /// @param mode File open mode.
        /// @param descriptor Optional access and sharing descriptor.
        /// @return Unique pointer to opened file, or null if resolution/opening fails.
        UniquePointer<File> openFile(StringSlice relativePath, FileMode mode, FileDescriptor const& descriptor = {});
        UniquePointer<File> openFile(Path const& relativePath, FileMode mode, FileDescriptor const& descriptor = {})
        {
            return openFile(relativePath.string(), mode, descriptor);
        }

        /// Opens a file stream from VFS. Resolves path first, then opens the result.
        /// @param relativePath Relative path to open.
        /// @param mode File open mode.
        /// @param descriptor Optional access and sharing descriptor.
        /// @return File stream (may be closed if resolution/opening fails).
        FileStream openStream(StringSlice relativePath, FileMode mode, FileDescriptor const& descriptor = {});
        FileStream openStream(Path const& relativePath, FileMode mode, FileDescriptor const& descriptor = {})
        {
            return openStream(relativePath.string(), mode, descriptor);
        }
    };
}
