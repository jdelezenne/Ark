#include "Ark/Storage/FileSystem.hpp"

#include <filesystem>
#include <functional>
#if !defined(ARK_PLATFORM_WINDOWS)
#include "Ark/Core/DateTime.hpp"
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>

namespace
{
    static Ark::Result<Ark::Storage::FileTime> fromUnixSeconds(long long seconds)
    {
        if (seconds < 0)
        {
            return Ark::Result<Ark::Storage::FileTime>(Ark::unexpectedResult);
        }

        unsigned long long const ms = static_cast<unsigned long long>(seconds) * 1000ULL;
        auto dateTime = Ark::DateTimeConvert::fromUnixMilliseconds(ms, false);
        return dateTime ? Ark::Result<Ark::Storage::FileTime>(dateTime.getValue()) : Ark::Result<Ark::Storage::FileTime>(Ark::unexpectedResult);
    }

    static long long toUnixSeconds(Ark::Storage::FileTime const& time)
    {
        auto timeMs = Ark::DateTimeConvert::toUnixMilliseconds(time);
        if (!timeMs)
        {
            return -1;
        }

        return static_cast<long long>(timeMs.getValue() / 1000ULL);
    }

    static Ark::Outcome setMtimeSeconds(Ark::StringSlice path, long long seconds)
    {
#if defined(UTIME_OMIT)
        struct timespec ts[2];
        ts[0].tv_sec = 0;
        ts[0].tv_nsec = UTIME_OMIT;
        ts[1].tv_sec = static_cast<time_t>(seconds);
        ts[1].tv_nsec = 0;
        if (::utimensat(AT_FDCWD, path.asPointer(), ts, 0) == 0)
        {
            return Ark::makeOutcome();
        }
#endif
        int fd = ::open(path.asPointer(), O_RDONLY);
        if (fd >= 0)
        {
#if defined(UTIME_OMIT)
            ts[0].tv_sec = 0;
            ts[0].tv_nsec = UTIME_OMIT;
            ts[1].tv_sec = static_cast<time_t>(seconds);
            ts[1].tv_nsec = 0;
            int rr = ::futimens(fd, ts);
            ::close(fd);
            return (rr == 0) ? Ark::makeOutcome() : Ark::makeError();
#else
            ::close(fd);
#endif
        }

        struct timeval tv[2];
        tv[0].tv_sec = 0;
        tv[0].tv_usec = -1;
        tv[1].tv_sec = static_cast<time_t>(seconds);
        tv[1].tv_usec = 0;
        return (::utimes(path.asPointer(), tv) == 0) ? Ark::makeOutcome() : Ark::makeError();
    }
}
#endif

namespace Ark::Storage
{
    namespace fs = std::filesystem;

    String FileSystem::normalizePath(StringSlice path)
    {
        fs::path p(path.asPointer());
        p = p.lexically_normal();
        return String(p.generic_string().c_str());
    }

    Result<bool> FileSystem::exists(StringSlice path)
    {
        std::error_code errorCode;
        bool ok = fs::exists(fs::path(path.asPointer()), errorCode);
        if (errorCode)
        {
            return Result<bool>(unexpectedResult);
        }
        return ok;
    }

    Result<bool> FileSystem::directoryExists(StringSlice path)
    {
        std::error_code errorCode;
        bool ok = fs::is_directory(fs::path(path.asPointer()), errorCode);
        if (errorCode)
        {
            return Result<bool>(unexpectedResult);
        }
        return ok;
    }

    Outcome FileSystem::createDirectory(StringSlice path)
    {
        std::error_code errorCode;
        fs::create_directories(fs::path(path.asPointer()), errorCode);
        return errorCode ? makeError() : makeOutcome();
    }

    Outcome FileSystem::deleteDirectory(StringSlice path)
    {
        std::error_code errorCode;
        fs::remove(fs::path(path.asPointer()), errorCode);
        return errorCode ? makeError() : makeOutcome();
    }

    Outcome FileSystem::deleteDirectoryRecursively(StringSlice path)
    {
        std::error_code errorCode;
        fs::remove_all(fs::path(path.asPointer()), errorCode);
        return errorCode ? makeError() : makeOutcome();
    }

    Outcome FileSystem::enumerateDirectory(StringSlice path, EnumerateDirectoryCallback callback)
    {
        std::error_code errorCode;
        fs::directory_iterator it(fs::path(path.asPointer()), errorCode);
        if (errorCode)
        {
            return makeError();
        }
        for (auto const& entry : it)
        {
            bool isFile = entry.is_regular_file();
            Path p{StringSlice(entry.path().generic_string().c_str())};
            if (!callback(p, isFile))
            {
                break;
            }
        }
        return makeOutcome();
    }

    Outcome FileSystem::enumerateDirectory(StringSlice path, bool recursive, EnumerateDirectoryCallback callback)
    {
        std::error_code errorCode;
        if (recursive)
        {
            fs::recursive_directory_iterator iter(fs::path(path.asPointer()), errorCode);
            if (errorCode)
            {
                return makeError();
            }
            for (auto const& entry : iter)
            {
                bool isFile = entry.is_regular_file();
                Path entryPath{StringSlice(entry.path().generic_string().data())};
                if (!callback(entryPath, isFile))
                {
                    break;
                }
            }

            return makeOutcome();
        }

        return enumerateDirectory(path, Ark::move(callback));
    }

    Result<Collections::Array<String>> FileSystem::getFiles(StringSlice path)
    {
        Collections::Array<String> files;
        std::error_code errorCode;
        fs::directory_iterator it(fs::path(path.asPointer()), errorCode);
        if (errorCode)
        {
            return Result<Collections::Array<String>>(unexpectedResult);
        }
        for (auto const& entry : it)
        {
            if (entry.is_regular_file())
            {
                files.append(String(entry.path().generic_string().c_str()));
            }
        }
        return files;
    }

    Result<bool> FileSystem::fileExists(StringSlice path)
    {
        std::error_code errorCode;
        bool ok = fs::is_regular_file(fs::path(path.asPointer()), errorCode);
        if (errorCode)
        {
            return Result<bool>(unexpectedResult);
        }
        return ok;
    }

    Outcome FileSystem::deleteFile(StringSlice path)
    {
        std::error_code errorCode;
        fs::remove(fs::path(path.asPointer()), errorCode);
        return errorCode ? makeError() : makeOutcome();
    }

    Result<FileInformation> FileSystem::getFileInformation(StringSlice path)
    {
        FileInformation info;
        info.path = String(path.asPointer());

        std::error_code errorCode;
        fs::file_status st = fs::status(fs::path(path.asPointer()), errorCode);
        if (errorCode)
        {
            return Result<FileInformation>(unexpectedResult);
        }

        info.exists = fs::exists(st);
        if (fs::is_regular_file(st))
        {
            info.type = FileType::File;
            info.isFile = true;
            info.isDirectory = false;
            auto sz = fs::file_size(fs::path(path.asPointer()), errorCode);
            if (!errorCode)
            {
                info.size = static_cast<usize>(sz);
            }
            auto perms = st.permissions();
            info.isReadOnly = (static_cast<bool>(perms & fs::perms::owner_write) == false);
        }
        else if (fs::is_directory(st))
        {
            info.type = FileType::Directory;
            info.isFile = false;
            info.isDirectory = true;
        }
        else if (fs::is_symlink(st))
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
        std::error_code errorCode;
        auto sz = fs::file_size(fs::path(path.asPointer()), errorCode);
        if (errorCode)
        {
            return Result<uint64>(unexpectedResult);
        }
        return static_cast<uint64>(sz);
    }

    Result<Storage::FileTime> FileSystem::getCreationTime(StringSlice path)
    {
#if defined(ARK_PLATFORM_WINDOWS)
        return Result<Storage::FileTime>(unexpectedResult);
#else
        struct stat st{};
        if (::stat(path.asPointer(), &st) != 0)
        {
            return Result<Storage::FileTime>(unexpectedResult);
        }
#if defined(ARK_PLATFORM_APPLE)
        long long const seconds = static_cast<long long>(st.st_birthtimespec.tv_sec);
#elif defined(ARK_PLATFORM_LINUX)
        long long const seconds = static_cast<long long>(st.st_ctim.tv_sec);
#else
        long long const seconds = static_cast<long long>(st.st_mtime);
#endif
        return fromUnixSeconds(seconds);
#endif
    }

    Result<Storage::FileTime> FileSystem::getLastAccessTime(StringSlice path)
    {
#if defined(ARK_PLATFORM_WINDOWS)
        return Result<Storage::FileTime>(unexpectedResult);
#else
        struct stat st{};
        if (::stat(path.asPointer(), &st) != 0)
        {
            return Result<Storage::FileTime>(unexpectedResult);
        }
#if defined(ARK_PLATFORM_APPLE)
        long long const seconds = static_cast<long long>(st.st_atimespec.tv_sec);
#else
        long long const seconds = static_cast<long long>(st.st_atime);
#endif
        return fromUnixSeconds(seconds);
#endif
    }

    Result<Storage::FileTime> FileSystem::getLastModificationTime(StringSlice path)
    {
#if defined(ARK_PLATFORM_WINDOWS)
        return Result<Storage::FileTime>(unexpectedResult);
#else
        struct stat st{};
        if (::stat(path.asPointer(), &st) != 0)
        {
            return Result<Storage::FileTime>(unexpectedResult);
        }
#if defined(ARK_PLATFORM_APPLE)
        long long const seconds = static_cast<long long>(st.st_mtimespec.tv_sec);
#else
        long long const seconds = static_cast<long long>(st.st_mtime);
#endif
        return fromUnixSeconds(seconds);
#endif
    }

    Outcome FileSystem::setLastModificationTime(StringSlice path, Storage::FileTime const& time)
    {
#if defined(ARK_PLATFORM_WINDOWS)
        return makeError();
#else
        long long const seconds = toUnixSeconds(time);
        if (seconds < 0)
        {
            return makeError();
        }
        return setMtimeSeconds(path, seconds);
#endif
    }

    Result<bool> FileSystem::isReadOnly(StringSlice path)
    {
        std::error_code errorCode;
        auto perms = fs::status(fs::path(path.asPointer()), errorCode).permissions();
        if (errorCode)
        {
            return Result<bool>(unexpectedResult);
        }
        bool ro = (static_cast<bool>(perms & fs::perms::owner_write) == false);
        return ro;
    }

    Outcome FileSystem::setReadOnly(StringSlice path, bool value)
    {
        std::error_code errorCode;
        auto p = fs::path(path.asPointer());
        auto perms = fs::status(p, errorCode).permissions();
        if (errorCode)
        {
            return makeError();
        }
        if (value)
        {
            perms &= ~fs::perms::owner_write;
        }
        else
        {
            perms |= fs::perms::owner_write;
        }
        fs::permissions(p, perms, errorCode);
        return errorCode ? makeError() : makeOutcome();
    }

    Outcome FileSystem::copyDirectory(StringSlice sourcePath, StringSlice destinationPath, bool isRecursive, bool overwriteExisting)
    {
        std::error_code errorCode;
        fs::copy_options options = fs::copy_options::none;
        if (isRecursive)
        {
            options |= fs::copy_options::recursive;
        }
        if (overwriteExisting)
        {
            options |= fs::copy_options::overwrite_existing;
        }
        fs::copy(fs::path(sourcePath.asPointer()), fs::path(destinationPath.asPointer()), options, errorCode);
        return errorCode ? makeError() : makeOutcome();
    }

    Outcome FileSystem::moveDirectory(StringSlice sourcePath, StringSlice destinationPath, bool overwriteExisting)
    {
        std::error_code errorCode;
        if (overwriteExisting)
        {
            fs::remove_all(fs::path(destinationPath.asPointer()), errorCode);
            if (errorCode)
            {
                return makeError();
            }
        }
        fs::rename(fs::path(sourcePath.asPointer()), fs::path(destinationPath.asPointer()), errorCode);
        return errorCode ? makeError() : makeOutcome();
    }

    Outcome FileSystem::renameDirectory(StringSlice oldPath, StringSlice newPath)
    {
        std::error_code errorCode;
        fs::rename(fs::path(oldPath.asPointer()), fs::path(newPath.asPointer()), errorCode);
        return errorCode ? makeError() : makeOutcome();
    }

    Outcome FileSystem::copyFile(StringSlice sourcePath, StringSlice destinationPath, bool overwriteExisting)
    {
        std::error_code errorCode;
        fs::copy_options options = overwriteExisting ? fs::copy_options::overwrite_existing : fs::copy_options::none;
        fs::copy_file(fs::path(sourcePath.asPointer()), fs::path(destinationPath.asPointer()), options, errorCode);
        return errorCode ? makeError() : makeOutcome();
    }

    Outcome FileSystem::moveFile(StringSlice sourcePath, StringSlice destinationPath, bool overwriteExisting)
    {
        std::error_code errorCode;
        if (overwriteExisting)
        {
            fs::remove(fs::path(destinationPath.asPointer()), errorCode);
            if (errorCode)
            {
                return makeError();
            }
        }
        fs::rename(fs::path(sourcePath.asPointer()), fs::path(destinationPath.asPointer()), errorCode);
        return errorCode ? makeError() : makeOutcome();
    }

    Outcome FileSystem::renameFile(StringSlice oldPath, StringSlice newPath)
    {
        std::error_code errorCode;
        fs::rename(fs::path(oldPath.asPointer()), fs::path(newPath.asPointer()), errorCode);
        return errorCode ? makeError() : makeOutcome();
    }
}
