#include "Ark/Storage/VirtualFileSystem.hpp"
#include "Ark/Storage/FileSystem.hpp"

namespace Ark::Storage
{
    void VirtualFileSystem::mount(StringSlice rootPath)
    {
        Path const root(rootPath);

        Mount m;
        m.root = root;
        mounts.append(m);
    }

    void VirtualFileSystem::unmount(StringSlice rootPath)
    {
        Path const target = Path(rootPath).normalized();

        for (usize i = 0; i < mounts.getCount(); ++i)
        {
            if (mounts[i].root == target)
            {
                mounts.removeAt(i);
                return;
            }
        }
    }

    void VirtualFileSystem::clear()
    {
        mounts.clear();
    }

    Collections::Array<VirtualFileSystem::Mount> const& VirtualFileSystem::getMounts() const
    {
        return mounts;
    }

    bool VirtualFileSystem::exists(StringSlice relativePath) const
    {
        Path const rel(relativePath);
        Path const normalizedRel = rel.normalized();

        for (usize i = 0; i < mounts.getCount(); ++i)
        {
            Path const joined = mounts[i].root.join(normalizedRel);
            auto const existsResult = FileSystem::exists(joined);
            if (existsResult && *existsResult)
            {
                return true;
            }
        }

        return false;
    }

    Result<Path> VirtualFileSystem::resolve(StringSlice relativePath) const
    {
        Path const rel(relativePath);
        Path const normalizedRel = rel.normalized();

        for (usize i = 0; i < mounts.getCount(); ++i)
        {
            Path const joined = mounts[i].root.join(normalizedRel);
            auto const existsResult = FileSystem::exists(joined);
            if (existsResult && *existsResult)
            {
                return joined;
            }
        }

        return Result<Path>(unexpectedResult);
    }

    UniquePointer<File> VirtualFileSystem::openFile(StringSlice relativePath, FileMode mode, FileDescriptor const& descriptor)
    {
        if (mode != FileMode::OpenExisting)
        {
            return nullptr;
        }

        auto const resolved = resolve(relativePath);
        if (!resolved)
        {
            return nullptr;
        }

        return File::create(resolved.getValue().string(), mode, descriptor);
    }

    FileStream VirtualFileSystem::openStream(StringSlice relativePath, FileMode mode, FileDescriptor const& descriptor)
    {
        if (mode != FileMode::OpenExisting)
        {
            return FileStream();
        }

        auto const resolved = resolve(relativePath);
        if (!resolved)
        {
            return FileStream();
        }

        return FileStream(resolved.getValue().string(), mode, descriptor);
    }
}
