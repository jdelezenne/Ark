#if defined(_WIN32)
#include "Ark/Storage/FileSystem.hpp"
#include "Ark/Core/DateTime.hpp"

#include <functional>
#include <string>
#include <windows.h>

namespace
{
    static Ark::Result<Ark::Storage::FileTime> fromWindowsFileTime(FILETIME const& ft)
    {
        ULARGE_INTEGER ui{};
        ui.LowPart = ft.dwLowDateTime;
        ui.HighPart = ft.dwHighDateTime;
        unsigned long long const WINDOWS_TO_UNIX_EPOCH_100NS = 116444736000000000ULL;
        if (ui.QuadPart < WINDOWS_TO_UNIX_EPOCH_100NS)
        {
            return Ark::Result<Ark::Storage::FileTime>(Ark::unexpectedResult);
        }

        unsigned long long const epoch100ns = ui.QuadPart - WINDOWS_TO_UNIX_EPOCH_100NS;
        unsigned long long const ms = epoch100ns / 10000ULL;
        auto dt = Ark::DateTimeConvert::fromUnixMilliseconds(ms, false);
        return dt ? Ark::Result<Ark::Storage::FileTime>(dt.getValue()) : Ark::Result<Ark::Storage::FileTime>(Ark::unexpectedResult);
    }

    static bool toWindowsFileTime(Ark::Storage::FileTime const& time, FILETIME& out)
    {
        auto const result = Ark::DateTimeConvert::toUnixMilliseconds(time);
        if (!result)
        {
            return false;
        }

        unsigned long long const ms = result.getValue();
        unsigned long long const epoch100ns = ms * 10000ULL;
        unsigned long long const WINDOWS_TO_UNIX_EPOCH_100NS = 116444736000000000ULL;
        unsigned long long const ftValue = epoch100ns + WINDOWS_TO_UNIX_EPOCH_100NS;
        ULARGE_INTEGER ui{};
        ui.QuadPart = ftValue;
        out.dwLowDateTime = ui.LowPart;
        out.dwHighDateTime = ui.HighPart;
        return true;
    }
}

namespace Ark::Storage
{
    String FileSystem::normalizePath(StringSlice path)
    {
        char buffer[MAX_PATH];
        DWORD const len = ::GetFullPathNameA(path.asPointer(), static_cast<DWORD>(sizeof(buffer)), buffer, nullptr);
        if (len == 0 || len >= sizeof(buffer))
        {
            return String(path.asPointer());
        }
        std::string s(buffer);
        for (char& ch : s)
        {
            if (ch == '\\')
            {
                ch = '/';
            }
        }
        return String(s.c_str());
    }

    Result<bool> FileSystem::exists(StringSlice path)
    {
        DWORD attrs = ::GetFileAttributesA(path.asPointer());
        if (attrs == INVALID_FILE_ATTRIBUTES)
        {
            DWORD err = ::GetLastError();
            if (err == ERROR_FILE_NOT_FOUND || err == ERROR_PATH_NOT_FOUND)
            {
                return Result<bool>(false);
            }
            return Result<bool>(Ark::unexpectedResult);
        }
        return true;
    }

    Result<bool> FileSystem::directoryExists(StringSlice path)
    {
        DWORD attrs = ::GetFileAttributesA(path.asPointer());
        if (attrs == INVALID_FILE_ATTRIBUTES)
        {
            DWORD err = ::GetLastError();
            if (err == ERROR_FILE_NOT_FOUND || err == ERROR_PATH_NOT_FOUND)
            {
                return Result<bool>(false);
            }
            return Result<bool>(Ark::unexpectedResult);
        }
        return (attrs & FILE_ATTRIBUTE_DIRECTORY) != 0;
    }

    Outcome FileSystem::createDirectory(StringSlice path)
    {
        char const* input = path.asPointer();
        if (input == nullptr || *input == '\0')
        {
            return makeError();
        }

        usize const pathLen = Utf8Strings::getByteLengthUnsafe(input);
        String normalizedPath;
        normalizedPath.reserve(pathLen);

        for (usize i = 0; i < pathLen; ++i)
        {
            char const ch = input[i];
            normalizedPath.append((ch == '/') ? '\\' : ch);
        }

        if (::CreateDirectoryA(normalizedPath.asPointer(), nullptr) != 0)
        {
            return makeOutcome();
        }

        DWORD const err = ::GetLastError();
        if (err == ERROR_ALREADY_EXISTS)
        {
            return makeOutcome();
        }

        if (err != ERROR_PATH_NOT_FOUND)
        {
            return makeError();
        }

        String current;
        current.reserve(pathLen);

        for (usize i = 0; i < normalizedPath.getLength(); ++i)
        {
            char const c = normalizedPath[i];
            current.append(c);

            if (c == '\\' || i + 1 == normalizedPath.getLength())
            {
                if (!current.isEmpty() && current[current.getLength() - 1] == '\\')
                {
                    if (current.getLength() <= 3)
                    {
                        continue;
                    }
                    current = current.prefix(current.getLength() - 1).toString();
                }

                if (!current.isEmpty())
                {
                    if (::CreateDirectoryA(current.asPointer(), nullptr) == 0)
                    {
                        DWORD const createErr = ::GetLastError();
                        if (createErr != ERROR_ALREADY_EXISTS)
                        {
                            return makeError();
                        }
                    }
                }

                if (i + 1 < normalizedPath.getLength() && normalizedPath[i + 1] == '\\')
                {
                    while (i + 1 < normalizedPath.getLength() && normalizedPath[i + 1] == '\\')
                    {
                        ++i;
                    }
                }
            }
        }

        return makeOutcome();
    }

    Outcome FileSystem::deleteDirectory(StringSlice path)
    {
        std::string p(path.asPointer());
        for (char& ch : p)
        {
            if (ch == '/')
            {
                ch = '\\';
            }
        }
        BOOL ok = ::RemoveDirectoryA(p.c_str());
        return ok ? makeOutcome() : makeError();
    }

    Outcome FileSystem::deleteDirectoryRecursively(StringSlice path)
    {
        // Recursive removal
        std::string base(path.asPointer());
        for (char& ch : base)
        {
            if (ch == '/')
            {
                ch = '\\';
            }
        }
        if (!base.empty() && (base.back() == '\\' || base.back() == '/'))
        {
            base.pop_back();
        }

        std::string search = base;
        search += "\\*";

        WIN32_FIND_DATAA data{};
        HANDLE h = ::FindFirstFileA(search.c_str(), &data);
        if (h != INVALID_HANDLE_VALUE)
        {
            do
            {
                char const* name = data.cFileName;
                if (name[0] == '.' && (name[1] == '\0' || (name[1] == '.' && name[2] == '\0')))
                {
                    continue;
                }
                std::string child = base;
                child += "\\";
                child += name;
                if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
                {
                    FileSystem::deleteDirectoryRecursively(StringSlice(child.c_str()));
                }
                else
                {
                    ::SetFileAttributesA(child.c_str(), FILE_ATTRIBUTE_NORMAL);
                    ::DeleteFileA(child.c_str());
                }
            }
            while (::FindNextFileA(h, &data) != 0);
            ::FindClose(h);
        }

        BOOL ok = ::RemoveDirectoryA(base.c_str());
        return ok ? makeOutcome() : makeError();
    }

    Outcome FileSystem::enumerateDirectory(StringSlice path, EnumerateDirectoryCallback callback)
    {
        std::string base(path.asPointer());
        std::string baseGeneric = base;
        for (char& ch : baseGeneric)
        {
            if (ch == '\\')
            {
                ch = '/';
            }
        }
        if (!baseGeneric.empty() && baseGeneric.back() != '/')
        {
            baseGeneric.push_back('/');
        }

        std::string search(path.asPointer());
        for (char& ch : search)
        {
            if (ch == '/')
            {
                ch = '\\';
            }
        }
        if (!search.empty() && search.back() != '\\')
        {
            search.push_back('\\');
        }
        search.push_back('*');

        WIN32_FIND_DATAA data{};
        HANDLE h = ::FindFirstFileA(search.c_str(), &data);
        if (h == INVALID_HANDLE_VALUE)
        {
            return makeError();
        }
        do
        {
            char const* name = data.cFileName;
            if (name[0] == '.' && (name[1] == '\0' || (name[1] == '.' && name[2] == '\0')))
            {
                continue;
            }
            bool const isFile = (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
            std::string fullGeneric = baseGeneric;
            fullGeneric += name;
            Path entryPath{StringSlice(fullGeneric.c_str())};
            if (!callback(entryPath, isFile))
            {
                break;
            }
        }
        while (::FindNextFileA(h, &data) != 0);
        ::FindClose(h);
        return makeOutcome();
    }

    Outcome FileSystem::enumerateDirectory(StringSlice path, bool recursive, EnumerateDirectoryCallback callback)
    {
        if (!recursive)
        {
            return enumerateDirectory(path, Ark::move(callback));
        }

        std::string start(path.asPointer());
        // Normalize to generic for assembling full paths
        std::string startGeneric = start;
        for (char& ch : startGeneric)
        {
            if (ch == '\\')
                ch = '/';
        }
        if (!startGeneric.empty() && startGeneric.back() != '/')
            startGeneric.push_back('/');

        // Stack of pairs: search base native and generic base for output
        struct Item
        {
            std::string baseNative;
            std::string baseGeneric;
        };
        Ark::Collections::Array<Item> stack;
        {
            std::string baseNative = start;
            for (char& ch : baseNative)
                if (ch == '/')
                    ch = '\\';
            if (!baseNative.empty() && baseNative.back() != '\\')
                baseNative.push_back('\\');
            stack.append(Item{baseNative, startGeneric});
        }

        while (!stack.isEmpty())
        {
            Item it = stack.getLast();
            stack.pop();

            std::string search = it.baseNative;
            search.push_back('*');
            WIN32_FIND_DATAA data{};
            HANDLE h = ::FindFirstFileA(search.c_str(), &data);
            if (h == INVALID_HANDLE_VALUE)
            {
                continue;
            }
            do
            {
                char const* name = data.cFileName;
                if (name[0] == '.' && (name[1] == '\0' || (name[1] == '.' && name[2] == '\0')))
                {
                    continue;
                }
                bool const isDir = (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
                bool const isFile = !isDir;
                std::string fullGeneric = it.baseGeneric;
                fullGeneric += name;
                Path entryPath{StringSlice(fullGeneric.c_str())};
                if (!callback(entryPath, isFile))
                {
                    break;
                }
                if (isDir)
                {
                    std::string childNative = it.baseNative;
                    childNative += name;
                    childNative.push_back('\\');
                    std::string childGeneric = it.baseGeneric;
                    childGeneric += name;
                    childGeneric.push_back('/');
                    stack.append(Item{childNative, childGeneric});
                }
            }
            while (::FindNextFileA(h, &data) != 0);
            ::FindClose(h);
        }
        return makeOutcome();
    }

    Result<Collections::Array<String>> FileSystem::getFiles(StringSlice path)
    {
        Collections::Array<String> files;
        Outcome const rr = enumerateDirectory(path,
                                              [&files](Path const& p, bool isFile)
                                              {
                                                  if (isFile)
                                                  {
                                                      files.append(p.string());
                                                  }
                                                  return true;
                                              });
        if (!rr)
        {
            return Result<Collections::Array<String>>(Ark::unexpectedResult);
        }
        return files;
    }

    Result<bool> FileSystem::fileExists(StringSlice path)
    {
        DWORD attrs = ::GetFileAttributesA(path.asPointer());
        if (attrs == INVALID_FILE_ATTRIBUTES)
        {
            DWORD err = ::GetLastError();
            if (err == ERROR_FILE_NOT_FOUND || err == ERROR_PATH_NOT_FOUND)
            {
                return Result<bool>(false);
            }
            return Result<bool>(Ark::unexpectedResult);
        }
        return (attrs & FILE_ATTRIBUTE_DIRECTORY) == 0;
    }

    Outcome FileSystem::deleteFile(StringSlice path)
    {
        BOOL ok = ::DeleteFileA(path.asPointer());
        if (!ok)
        {
            // Try clearing read-only and retry
            DWORD attrs = ::GetFileAttributesA(path.asPointer());
            if (attrs != INVALID_FILE_ATTRIBUTES)
            {
                attrs &= ~FILE_ATTRIBUTE_READONLY;
                ::SetFileAttributesA(path.asPointer(), attrs);
                ok = ::DeleteFileA(path.asPointer());
            }
        }
        return ok ? makeOutcome() : makeError();
    }

    Result<FileInformation> FileSystem::getFileInformation(StringSlice path)
    {
        FileInformation info;
        info.path = String(path.asPointer());

        WIN32_FILE_ATTRIBUTE_DATA fad{};
        if (::GetFileAttributesExA(path.asPointer(), GetFileExInfoStandard, &fad) == 0)
        {
            info.exists = false;
            info.type = FileType::Unknown;
            return info;
        }

        info.exists = true;
        bool const isDir = (fad.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
        if (isDir)
        {
            info.type = FileType::Directory;
            info.isFile = false;
            info.isDirectory = true;
            info.isReadOnly = (fad.dwFileAttributes & FILE_ATTRIBUTE_READONLY) != 0;
        }
        else
        {
            info.type = FileType::File;
            info.isFile = true;
            info.isDirectory = false;
            ULARGE_INTEGER ui{};
            ui.HighPart = fad.nFileSizeHigh;
            ui.LowPart = fad.nFileSizeLow;
            info.size = static_cast<usize>(ui.QuadPart);
            info.isReadOnly = (fad.dwFileAttributes & FILE_ATTRIBUTE_READONLY) != 0;
        }

        return info;
    }

    Result<uint64> FileSystem::getFileSize(StringSlice path)
    {
        WIN32_FILE_ATTRIBUTE_DATA fad{};
        if (::GetFileAttributesExA(path.asPointer(), GetFileExInfoStandard, &fad) == 0)
        {
            return Result<uint64>(Ark::unexpectedResult);
        }
        if ((fad.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
        {
            return Result<uint64>(Ark::unexpectedResult);
        }
        ULARGE_INTEGER ui{};
        ui.HighPart = fad.nFileSizeHigh;
        ui.LowPart = fad.nFileSizeLow;
        return static_cast<uint64>(ui.QuadPart);
    }

    Result<Storage::FileTime> FileSystem::getCreationTime(StringSlice path)
    {
        WIN32_FILE_ATTRIBUTE_DATA fad{};
        if (::GetFileAttributesExA(path.asPointer(), GetFileExInfoStandard, &fad) == 0)
        {
            return Result<Storage::FileTime>(Ark::unexpectedResult);
        }
        return fromWindowsFileTime(fad.ftCreationTime);
    }

    Result<Storage::FileTime> FileSystem::getLastAccessTime(StringSlice path)
    {
        WIN32_FILE_ATTRIBUTE_DATA fad{};
        if (::GetFileAttributesExA(path.asPointer(), GetFileExInfoStandard, &fad) == 0)
        {
            return Result<Storage::FileTime>(Ark::unexpectedResult);
        }
        return fromWindowsFileTime(fad.ftLastAccessTime);
    }

    Result<Storage::FileTime> FileSystem::getLastModificationTime(StringSlice path)
    {
        WIN32_FILE_ATTRIBUTE_DATA fad{};
        if (::GetFileAttributesExA(path.asPointer(), GetFileExInfoStandard, &fad) == 0)
        {
            return Result<Storage::FileTime>(Ark::unexpectedResult);
        }
        return fromWindowsFileTime(fad.ftLastWriteTime);
    }

    Outcome FileSystem::setLastModificationTime(StringSlice path, Storage::FileTime const& time)
    {
        FILETIME ft{};
        if (!toWindowsFileTime(time, ft))
        {
            return makeError();
        }

        HANDLE h = ::CreateFileA(
            path.asPointer(),
            FILE_WRITE_ATTRIBUTES,
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
            nullptr,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            nullptr);
        if (h == INVALID_HANDLE_VALUE)
        {
            return makeError();
        }
        BOOL ok = ::SetFileTime(h, nullptr, nullptr, &ft);
        ::CloseHandle(h);
        return ok ? makeOutcome() : makeError();
    }

    Result<bool> FileSystem::isReadOnly(StringSlice path)
    {
        DWORD attrs = ::GetFileAttributesA(path.asPointer());
        if (attrs == INVALID_FILE_ATTRIBUTES)
        {
            return Result<bool>(Ark::unexpectedResult);
        }
        return (attrs & FILE_ATTRIBUTE_READONLY) != 0;
    }

    Outcome FileSystem::setReadOnly(StringSlice path, bool value)
    {
        DWORD attrs = ::GetFileAttributesA(path.asPointer());
        if (attrs == INVALID_FILE_ATTRIBUTES)
        {
            return makeError();
        }
        if (value)
        {
            attrs |= FILE_ATTRIBUTE_READONLY;
        }
        else
        {
            attrs &= ~FILE_ATTRIBUTE_READONLY;
        }
        return (::SetFileAttributesA(path.asPointer(), attrs) != 0) ? makeOutcome() : makeError();
    }

    Outcome FileSystem::copyDirectory(StringSlice sourcePath, StringSlice destinationPath, bool isRecursive, bool overwriteExisting)
    {
        // Ensure destination exists
        Outcome cr = createDirectory(destinationPath);
        if (!cr)
        {
            return cr;
        }

        std::string src(sourcePath.asPointer());
        std::string dst(destinationPath.asPointer());
        for (char& ch : src)
            if (ch == '/')
                ch = '\\';
        for (char& ch : dst)
            if (ch == '/')
                ch = '\\';

        if (!src.empty() && src.back() != '\\')
            src.push_back('\\');
        if (!dst.empty() && dst.back() != '\\')
            dst.push_back('\\');

        std::string search = src;
        search.push_back('*');
        WIN32_FIND_DATAA data{};
        HANDLE h = ::FindFirstFileA(search.c_str(), &data);
        if (h == INVALID_HANDLE_VALUE)
        {
            return makeError();
        }
        do
        {
            char const* name = data.cFileName;
            if (name[0] == '.' && (name[1] == '\0' || (name[1] == '.' && name[2] == '\0')))
            {
                continue;
            }
            std::string childSrc = src;
            childSrc += name;
            std::string childDst = dst;
            childDst += name;
            if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
            {
                if (isRecursive)
                {
                    ::CreateDirectoryA(childDst.c_str(), nullptr);
                    Outcome rr = copyDirectory(StringSlice(childSrc.c_str()), StringSlice(childDst.c_str()), true, overwriteExisting);
                    if (!rr)
                    {
                        ::FindClose(h);
                        return rr;
                    }
                }
            }
            else
            {
                BOOL ok = ::CopyFileA(childSrc.c_str(), childDst.c_str(), overwriteExisting ? FALSE : TRUE);
                if (!ok)
                {
                    ::FindClose(h);
                    return makeError();
                }
            }
        }
        while (::FindNextFileA(h, &data) != 0);
        ::FindClose(h);
        return makeOutcome();
    }

    Outcome FileSystem::moveDirectory(StringSlice sourcePath, StringSlice destinationPath, bool overwriteExisting)
    {
        DWORD flags = MOVEFILE_COPY_ALLOWED;
        if (overwriteExisting)
        {
            flags |= MOVEFILE_REPLACE_EXISTING;
        }
        BOOL ok = ::MoveFileExA(sourcePath.asPointer(), destinationPath.asPointer(), flags);
        return ok ? makeOutcome() : makeError();
    }

    Outcome FileSystem::renameDirectory(StringSlice oldPath, StringSlice newPath)
    {
        BOOL ok = ::MoveFileExA(oldPath.asPointer(), newPath.asPointer(), 0);
        return ok ? makeOutcome() : makeError();
    }

    Outcome FileSystem::copyFile(StringSlice sourcePath, StringSlice destinationPath, bool overwriteExisting)
    {
        BOOL ok = ::CopyFileA(sourcePath.asPointer(), destinationPath.asPointer(), overwriteExisting ? FALSE : TRUE);
        return ok ? makeOutcome() : makeError();
    }

    Outcome FileSystem::moveFile(StringSlice sourcePath, StringSlice destinationPath, bool overwriteExisting)
    {
        DWORD flags = MOVEFILE_COPY_ALLOWED;
        if (overwriteExisting)
        {
            flags |= MOVEFILE_REPLACE_EXISTING;
        }
        BOOL ok = ::MoveFileExA(sourcePath.asPointer(), destinationPath.asPointer(), flags);
        return ok ? makeOutcome() : makeError();
    }

    Outcome FileSystem::renameFile(StringSlice oldPath, StringSlice newPath)
    {
        BOOL ok = ::MoveFileExA(oldPath.asPointer(), newPath.asPointer(), 0);
        return ok ? makeOutcome() : makeError();
    }
}
#endif
