#include "Ark/Storage/File.hpp"
#include <windows.h>

namespace Ark::Storage
{
    namespace
    {
        static DWORD toDesiredAccess(FileAccess access)
        {
            switch (access)
            {
                case FileAccess::Read:
                    return GENERIC_READ;

                case FileAccess::Write:
                    return GENERIC_WRITE;

                case FileAccess::ReadWrite:
                    return GENERIC_READ | GENERIC_WRITE;

                default:
                    return 0;
            }
        }

        static DWORD toShareMode(FileShare share)
        {
            DWORD mode = 0;
            if ((static_cast<uint>(share) & static_cast<uint>(FileShare::Read)) != 0)
            {
                mode |= FILE_SHARE_READ;
            }

            if ((static_cast<uint>(share) & static_cast<uint>(FileShare::Write)) != 0)
            {
                mode |= FILE_SHARE_WRITE;
            }

            if ((static_cast<uint>(share) & static_cast<uint>(FileShare::Delete)) != 0)
            {
                mode |= FILE_SHARE_DELETE;
            }
            return mode;
        }

        static DWORD toCreationDisposition(FileMode mode)
        {
            switch (mode)
            {
                case FileMode::CreateNew:
                    return CREATE_NEW;
                case FileMode::CreateAlways:
                    return CREATE_ALWAYS;
                case FileMode::OpenExisting:
                    return OPEN_EXISTING;
                case FileMode::OpenAlways:
                    return OPEN_ALWAYS;
                case FileMode::TruncateExisting:
                    return TRUNCATE_EXISTING;
            }
            return OPEN_EXISTING;
        }

        class WinFile final : public File
        {
        private:
            String path;
            HANDLE handle{INVALID_HANDLE_VALUE};

        public:
            ~WinFile() override
            {
                close();
            }

            String getPath() const override
            {
                return path;
            }

            bool isOpen() const override
            {
                return handle != INVALID_HANDLE_VALUE;
            }

            Outcome open(StringSlice p, FileMode mode, FileDescriptor const& descriptor) override
            {
                if (isOpen())
                {
                    return makeOutcome();
                }

                path = String(p.asPointer());
                DWORD desiredAccess = toDesiredAccess(descriptor.access);
                DWORD shareMode = toShareMode(descriptor.share);
                DWORD creation = toCreationDisposition(mode);

                HANDLE handle = ::CreateFileA(path.asPointer(), desiredAccess, shareMode, nullptr, creation, FILE_ATTRIBUTE_NORMAL, nullptr);
                if (handle == INVALID_HANDLE_VALUE)
                {
                    return makeError();
                }

                this->handle = handle;
                return makeOutcome();
            }

            Outcome close() override
            {
                if (handle != INVALID_HANDLE_VALUE)
                {
                    ::CloseHandle(handle);
                    handle = INVALID_HANDLE_VALUE;
                }

                return makeOutcome();
            }

            Result<usize> read(void* buffer, usize count) override
            {
                if (handle == INVALID_HANDLE_VALUE)
                {
                    return Result<usize>(unexpectedResult);
                }

                DWORD bytesRead = 0;
                if (!::ReadFile(handle, buffer, static_cast<DWORD>(count), &bytesRead, nullptr))
                {
                    return Result<usize>(unexpectedResult);
                }

                return static_cast<usize>(bytesRead);
            }

            Result<usize> write(void const* buffer, usize count) override
            {
                if (handle == INVALID_HANDLE_VALUE)
                {
                    return Result<usize>(unexpectedResult);
                }

                DWORD bytesWritten = 0;
                if (!::WriteFile(handle, buffer, static_cast<DWORD>(count), &bytesWritten, nullptr))
                {
                    return Result<usize>(unexpectedResult);
                }

                return static_cast<usize>(bytesWritten);
            }

            Outcome flush() override
            {
                if (handle == INVALID_HANDLE_VALUE)
                {
                    return makeError();
                }

                return (::FlushFileBuffers(handle) != 0) ? makeOutcome() : makeError();
            }

            Result<usize> getSize() const override
            {
                if (handle == INVALID_HANDLE_VALUE)
                {
                    return Result<usize>(unexpectedResult);
                }

                LARGE_INTEGER li{};
                if (::GetFileSizeEx(handle, &li) == 0)
                {
                    return Result<usize>(unexpectedResult);
                }

                return static_cast<usize>(li.QuadPart);
            }

            Result<usize> getPosition() const override
            {
                if (handle == INVALID_HANDLE_VALUE)
                {
                    return Result<usize>(unexpectedResult);
                }

                LARGE_INTEGER move{};
                LARGE_INTEGER newPos{};
                if (::SetFilePointerEx(handle, move, &newPos, FILE_CURRENT) == 0)
                {
                    return Result<usize>(unexpectedResult);
                }

                return static_cast<usize>(newPos.QuadPart);
            }

            Outcome setPosition(isize offset, SeekOrigin origin) override
            {
                if (handle == INVALID_HANDLE_VALUE)
                {
                    return makeError();
                }

                LARGE_INTEGER move{};
                move.QuadPart = static_cast<LONGLONG>(offset);
                DWORD method = FILE_BEGIN;
                switch (origin)
                {
                    case SeekOrigin::Begin:
                        method = FILE_BEGIN;
                        break;

                    case SeekOrigin::Current:
                        method = FILE_CURRENT;
                        break;

                    case SeekOrigin::End:
                        method = FILE_END;
                        break;
                }

                if (::SetFilePointerEx(handle, move, nullptr, method) == 0)
                {
                    return makeError();
                }

                return makeOutcome();
            }

            Result<uint64> getModificationTime() const override
            {
                if (handle == INVALID_HANDLE_VALUE)
                {
                    return Result<uint64>(unexpectedResult);
                }

                FILETIME ft{};
                if (::GetFileTime(handle, nullptr, nullptr, &ft) == 0)
                {
                    return Result<uint64>(unexpectedResult);
                }

                ULARGE_INTEGER ui{};
                ui.LowPart = ft.dwLowDateTime;
                ui.HighPart = ft.dwHighDateTime;
                uint64 const WINDOWS_TO_UNIX_EPOCH_100NS = 116444736000000000ULL;
                uint64 const epoch100ns = (ui.QuadPart >= WINDOWS_TO_UNIX_EPOCH_100NS) ? (ui.QuadPart - WINDOWS_TO_UNIX_EPOCH_100NS) : 0ULL;
                return static_cast<uint64>(epoch100ns / 10000000ULL);
            }
        };
    }

    UniquePointer<File> File::create()
    {
        return makeUnique<WinFile>();
    }

    UniquePointer<File> File::create(StringSlice path, FileMode mode, FileDescriptor const& descriptor)
    {
        auto file = makeUnique<WinFile>();

        file->open(path, mode, descriptor);
        if (!file->isOpen())
        {
            return nullptr;
        }

        return file;
    }
}
