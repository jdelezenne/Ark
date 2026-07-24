#if defined(ARK_PLATFORM_MACOS)
#include "Ark/Storage/FileSystem.hpp"
#include "Ark/Core/DateTime.hpp"

#include <cerrno>
#include <cstring>

#include <copyfile.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>

namespace Ark::Storage
{
    namespace
    {
        Result<FileTime> fromUnixSeconds(long long seconds)
        {
            if (seconds < 0)
            {
                return Result<FileTime>(unexpectedResult);
            }

            unsigned long long const ms = static_cast<unsigned long long>(seconds) * 1000ULL;
            auto dt = DateTimeConvert::fromUnixMilliseconds(ms, false);
            return dt ? Result<FileTime>(dt.getValue()) : Result<FileTime>(unexpectedResult);
        }

        long long toUnixSeconds(FileTime const& time)
        {
            auto msRes = DateTimeConvert::toUnixMilliseconds(time);
            if (!msRes)
            {
                return -1;
            }
            return static_cast<long long>(msRes.getValue() / 1000ULL);
        }

        Outcome setMtimeSeconds(StringSlice path, long long seconds)
        {
            int const fd = ::open(path.asPointer(), O_RDONLY);
            if (fd >= 0)
            {
                struct timespec ts[2];
                ts[0].tv_sec = 0;
                ts[0].tv_nsec = UTIME_OMIT;
                ts[1].tv_sec = static_cast<time_t>(seconds);
                ts[1].tv_nsec = 0;
                int const rr = ::futimens(fd, ts);
                ::close(fd);
                if (rr == 0)
                {
                    return makeOutcome();
                }
            }

            struct timeval tv[2];
            tv[0].tv_sec = 0;
            tv[0].tv_usec = -1;
            tv[1].tv_sec = static_cast<time_t>(seconds);
            tv[1].tv_usec = 0;
            return (::utimes(path.asPointer(), tv) == 0) ? makeOutcome() : makeError();
        }
    }

    String FileSystem::normalizePath(StringSlice path)
    {
        char buffer[PATH_MAX];
        char* resolved = ::realpath(path.asPointer(), buffer);
        if (resolved == nullptr)
        {
            return String(path.asPointer());
        }

        for (char* ptr = buffer; *ptr != '\0'; ++ptr)
        {
            if (*ptr == '\\')
            {
                *ptr = '/';
            }
        }

        return buffer;
    }

    Result<bool> FileSystem::exists(StringSlice path)
    {
        struct stat st{};
        if (::lstat(path.asPointer(), &st) != 0)
        {
            if (errno == ENOENT || errno == ENOTDIR)
            {
                return Result<bool>(false);
            }

            return Result<bool>(unexpectedResult);
        }

        return true;
    }

    Result<bool> FileSystem::directoryExists(StringSlice path)
    {
        struct stat st{};
        if (::lstat(path.asPointer(), &st) != 0)
        {
            if (errno == ENOENT || errno == ENOTDIR)
            {
                return Result<bool>(false);
            }

            return Result<bool>(unexpectedResult);
        }

        return S_ISDIR(st.st_mode);
    }

    bool ensureDirectoryCreated(char const* path)
    {
        if (::mkdir(path, 0755) == 0)
        {
            return true;
        }

        if (errno == EEXIST)
        {
            struct stat st{};
            if (::lstat(path, &st) == 0 && S_ISDIR(st.st_mode))
            {
                return true;
            }
        }

        return false;
    }

    Outcome FileSystem::createDirectory(StringSlice path)
    {
        char const* p = path.asPointer();
        if (p == nullptr || *p == '\0')
        {
            return makeError();
        }

        if (ensureDirectoryCreated(p))
        {
            return makeOutcome();
        }

        if (errno != ENOENT)
        {
            return makeError();
        }

        usize const pathLen = Utf8Strings::getByteLengthUnsafe(p);
        String current;
        current.reserve(pathLen);

        for (usize i = 0;; ++i)
        {
            char const c = p[i];
            if (c == '\0' || c == '/')
            {
                if (!current.isEmpty())
                {
                    while (current.getLength() > 1 && current[current.getLength() - 1] == '/')
                    {
                        current = current.prefix(current.getLength() - 1).toString();
                    }

                    if (!(current.getLength() == 1 && current[0] == '/'))
                    {
                        if (!ensureDirectoryCreated(current.asPointer()))
                        {
                            return makeError();
                        }
                    }
                }

                if (c == '\0')
                {
                    break;
                }

                if (current.isEmpty() || current[current.getLength() - 1] != '/')
                {
                    current.append('/');
                }

                while (p[i + 1] == '/')
                {
                    ++i;
                }
            }
            else
            {
                current.append(c);
            }
        }

        return makeOutcome();
    }

    Outcome FileSystem::deleteDirectory(StringSlice path)
    {
        return (::rmdir(path.asPointer()) == 0) ? makeOutcome() : makeError();
    }

    bool deleteDirectoryRecursiveInternal(char const* basePath)
    {
        DIR* dir = ::opendir(basePath);
        if (dir == nullptr)
        {
            return false;
        }

        struct dirent* ent;
        while ((ent = ::readdir(dir)) != nullptr)
        {
            if (std::strcmp(ent->d_name, ".") == 0 || std::strcmp(ent->d_name, "..") == 0)
            {
                continue;
            }
            char child[PATH_MAX];
            std::snprintf(child, sizeof(child), "%s/%s", basePath, ent->d_name);

            struct stat st{};
            if (::lstat(child, &st) == 0)
            {
                if (S_ISDIR(st.st_mode))
                {
                    bool const ok = deleteDirectoryRecursiveInternal(child);
                    if (!ok)
                    {
                        ::closedir(dir);
                        return false;
                    }
                }
                else
                {
                    ::unlink(child);
                }
            }
        }
        ::closedir(dir);
        return (::rmdir(basePath) == 0);
    }

    Outcome FileSystem::deleteDirectoryRecursively(StringSlice path)
    {
        return deleteDirectoryRecursiveInternal(path.asPointer()) ? makeOutcome() : makeError();
    }

    Outcome FileSystem::enumerateDirectory(StringSlice path, EnumerateDirectoryCallback callback)
    {
        DIR* dir = ::opendir(path.asPointer());
        if (dir == nullptr)
        {
            return makeError();
        }

        struct dirent* ent;
        while ((ent = ::readdir(dir)) != nullptr)
        {
            if (std::strcmp(ent->d_name, ".") == 0 || std::strcmp(ent->d_name, "..") == 0)
            {
                continue;
            }

            char full[PATH_MAX];
            std::snprintf(full, sizeof(full), "%s/%s", path.asPointer(), ent->d_name);

            struct stat st{};
            bool isFile = false;
            if (::lstat(full, &st) == 0)
            {
                isFile = S_ISREG(st.st_mode);
            }

            Path p{StringSlice(full)};
            if (!callback(p, isFile))
            {
                break;
            }
        }

        ::closedir(dir);
        return makeOutcome();
    }

    Outcome FileSystem::enumerateDirectory(StringSlice path, bool recursive, EnumerateDirectoryCallback callback)
    {
        if (!recursive)
        {
            return enumerateDirectory(path, Ark::move(callback));
        }

        // DFS using a simple stack of directories
        Collections::Array<String> stack;
        stack.append(String(path.asPointer()));
        while (!stack.isEmpty())
        {
            String current = stack.getLast();
            stack.removeLast();

            DIR* dir = ::opendir(current.asPointer());
            if (dir == nullptr)
            {
                continue;
            }
            struct dirent* ent;
            while ((ent = ::readdir(dir)) != nullptr)
            {
                if (std::strcmp(ent->d_name, ".") == 0 || std::strcmp(ent->d_name, "..") == 0)
                {
                    continue;
                }
                char full[PATH_MAX];
                std::snprintf(full, sizeof(full), "%s/%s", current.asPointer(), ent->d_name);
                struct stat st{};
                bool isFile = false;
                bool isDir = false;
                if (::lstat(full, &st) == 0)
                {
                    isFile = S_ISREG(st.st_mode);
                    isDir = S_ISDIR(st.st_mode);
                }
                Path p{StringSlice(full)};
                if (!callback(p, isFile))
                {
                    ::closedir(dir);
                    return makeOutcome();
                }
                if (isDir)
                {
                    stack.append(String(full));
                }
            }
            ::closedir(dir);
        }
        return makeOutcome();
    }

    Result<Collections::Array<String>> FileSystem::getFiles(StringSlice path)
    {
        Collections::Array<String> files;
        DIR* dir = ::opendir(path.asPointer());
        if (dir == nullptr)
        {
            return Result<Collections::Array<String>>(unexpectedResult);
        }

        struct dirent* ent;
        while ((ent = ::readdir(dir)) != nullptr)
        {
            if (std::strcmp(ent->d_name, ".") == 0 || std::strcmp(ent->d_name, "..") == 0)
            {
                continue;
            }

            char full[PATH_MAX];
            std::snprintf(full, sizeof(full), "%s/%s", path.asPointer(), ent->d_name);

            struct stat st{};
            if (::lstat(full, &st) == 0 && S_ISREG(st.st_mode))
            {
                files.append(String(full));
            }
        }

        ::closedir(dir);
        return files;
    }

    Result<bool> FileSystem::fileExists(StringSlice path)
    {
        struct stat st{};
        if (::lstat(path.asPointer(), &st) != 0)
        {
            if (errno == ENOENT || errno == ENOTDIR)
            {
                return false;
            }

            return Result<bool>(unexpectedResult);
        }

        return S_ISREG(st.st_mode);
    }

    Outcome FileSystem::deleteFile(StringSlice path)
    {
        return (::unlink(path.asPointer()) == 0) ? makeOutcome() : makeError();
    }

    Result<FileInformation> FileSystem::getFileInformation(StringSlice path)
    {
        FileInformation info;
        info.path = String(path.asPointer());

        struct stat st{};
        if (::lstat(path.asPointer(), &st) != 0)
        {
            return info;
        }

        info.exists = true;
        if (S_ISREG(st.st_mode))
        {
            info.type = FileType::File;
            info.isFile = true;
            info.isDirectory = false;
            info.size = static_cast<usize>(st.st_size);
            info.isReadOnly = (st.st_mode & S_IWUSR) == 0;
        }
        else if (S_ISDIR(st.st_mode))
        {
            info.type = FileType::Directory;
            info.isFile = false;
            info.isDirectory = true;
        }
        else if (S_ISLNK(st.st_mode))
        {
            info.type = FileType::SymLink;
        }
        else
        {
            info.type = FileType::Unknown;
        }

        return info;
    }

    Result<uint64> FileSystem::getFileSize(StringSlice path)
    {
        struct stat st{};
        if (::stat(path.asPointer(), &st) != 0 || S_ISDIR(st.st_mode))
        {
            return Result<uint64>(unexpectedResult);
        }

        return static_cast<uint64>(st.st_size);
    }

    Result<Storage::FileTime> FileSystem::getCreationTime(StringSlice path)
    {
        struct stat st{};
        if (::stat(path.asPointer(), &st) != 0)
        {
            return Result<Storage::FileTime>(unexpectedResult);
        }

        long long const seconds = static_cast<long long>(st.st_birthtimespec.tv_sec);
        return fromUnixSeconds(seconds);
    }

    Result<Storage::FileTime> FileSystem::getLastAccessTime(StringSlice path)
    {
        struct stat st{};
        if (::stat(path.asPointer(), &st) != 0)
        {
            return Result<Storage::FileTime>(unexpectedResult);
        }

        long long const seconds = static_cast<long long>(st.st_atimespec.tv_sec);
        return fromUnixSeconds(seconds);
    }

    Result<Storage::FileTime> FileSystem::getLastModificationTime(StringSlice path)
    {
        struct stat st{};
        if (::stat(path.asPointer(), &st) != 0)
        {
            return Result<Storage::FileTime>(unexpectedResult);
        }

        long long const seconds = static_cast<long long>(st.st_mtimespec.tv_sec);
        return fromUnixSeconds(seconds);
    }

    Outcome FileSystem::setLastModificationTime(StringSlice path, Storage::FileTime const& time)
    {
        long long const seconds = toUnixSeconds(time);
        if (seconds < 0)
        {
            return makeError();
        }

        return setMtimeSeconds(path, seconds);
    }

    Result<bool> FileSystem::isReadOnly(StringSlice path)
    {
        struct stat st{};
        if (::stat(path.asPointer(), &st) != 0)
        {
            return Result<bool>(unexpectedResult);
        }

        bool const ro = (st.st_mode & S_IWUSR) == 0;
        return ro;
    }

    Outcome FileSystem::setReadOnly(StringSlice path, bool value)
    {
        struct stat st{};
        if (::stat(path.asPointer(), &st) != 0)
        {
            return makeError();
        }

        mode_t mode = st.st_mode;
        if (value)
        {
            mode &= ~S_IWUSR;
        }
        else
        {
            mode |= S_IWUSR;
        }

        return (::chmod(path.asPointer(), mode) == 0) ? makeOutcome() : makeError();
    }

    bool copyDirectoryRecursiveInternal(char const* src, char const* dst, bool isRecursive, bool overwriteExisting)
    {
        DIR* dir = ::opendir(src);
        if (dir == nullptr)
        {
            return false;
        }

        struct dirent* ent;
        while ((ent = ::readdir(dir)) != nullptr)
        {
            if (std::strcmp(ent->d_name, ".") == 0 || std::strcmp(ent->d_name, "..") == 0)
            {
                continue;
            }
            char srcChild[PATH_MAX];
            char dstChild[PATH_MAX];
            std::snprintf(srcChild, sizeof(srcChild), "%s/%s", src, ent->d_name);
            std::snprintf(dstChild, sizeof(dstChild), "%s/%s", dst, ent->d_name);

            struct stat st{};
            if (::lstat(srcChild, &st) != 0)
            {
                ::closedir(dir);
                return false;
            }

            if (S_ISDIR(st.st_mode))
            {
                if (isRecursive)
                {
                    ::mkdir(dstChild, 0755);
                    if (!copyDirectoryRecursiveInternal(srcChild, dstChild, true, overwriteExisting))
                    {
                        ::closedir(dir);
                        return false;
                    }
                }
            }
            else if (S_ISREG(st.st_mode))
            {
                copyfile_flags_t flags = COPYFILE_DATA;
                if (!overwriteExisting)
                {
                    flags |= COPYFILE_EXCL;
                }

                if (::copyfile(srcChild, dstChild, nullptr, flags) != 0)
                {
                    ::closedir(dir);
                    return false;
                }
            }
        }

        ::closedir(dir);
        return true;
    }

    Outcome FileSystem::copyDirectory(StringSlice sourcePath, StringSlice destinationPath, bool isRecursive, bool overwriteExisting)
    {
        createDirectory(destinationPath);

        bool const ok = copyDirectoryRecursiveInternal(sourcePath.asPointer(), destinationPath.asPointer(), isRecursive, overwriteExisting);
        return ok ? makeOutcome() : makeError();
    }

    Outcome FileSystem::moveDirectory(StringSlice sourcePath, StringSlice destinationPath, bool overwriteExisting)
    {
        if (overwriteExisting)
        {
            deleteDirectoryRecursively(destinationPath);
        }

        return (::rename(sourcePath.asPointer(), destinationPath.asPointer()) == 0) ? makeOutcome() : makeError();
    }

    Outcome FileSystem::renameDirectory(StringSlice oldPath, StringSlice newPath)
    {
        return (::rename(oldPath.asPointer(), newPath.asPointer()) == 0) ? makeOutcome() : makeError();
    }

    Outcome FileSystem::copyFile(StringSlice sourcePath, StringSlice destinationPath, bool overwriteExisting)
    {
        copyfile_flags_t flags = COPYFILE_DATA;
        if (!overwriteExisting)
        {
            flags |= COPYFILE_EXCL;
        }

        return (::copyfile(sourcePath.asPointer(), destinationPath.asPointer(), nullptr, flags) == 0) ? makeOutcome() : makeError();
    }

    Outcome FileSystem::moveFile(StringSlice sourcePath, StringSlice destinationPath, bool overwriteExisting)
    {
        if (overwriteExisting)
        {
            ::unlink(destinationPath.asPointer());
        }

        return (::rename(sourcePath.asPointer(), destinationPath.asPointer()) == 0) ? makeOutcome() : makeError();
    }

    Outcome FileSystem::renameFile(StringSlice oldPath, StringSlice newPath)
    {
        return (::rename(oldPath.asPointer(), newPath.asPointer()) == 0) ? makeOutcome() : makeError();
    }
}

#endif
