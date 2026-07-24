#include "Ark/Storage/File.hpp"
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

namespace Ark::Storage
{
    namespace
    {
        static int toFlags(FileMode mode)
        {
            switch (mode)
            {
                case FileMode::CreateNew:
                    return O_CREAT | O_EXCL | O_WRONLY;

                case FileMode::CreateAlways:
                    return O_CREAT | O_TRUNC | O_WRONLY;

                case FileMode::OpenExisting:
                    return O_RDONLY;

                case FileMode::OpenAlways:
                    return O_CREAT | O_RDWR;

                case FileMode::TruncateExisting:
                    return O_TRUNC | O_WRONLY;

                default:
                    return O_RDONLY;
            }
        }

        class PosixFile final : public File
        {
        private:
            String path;
            int fd{-1};

        public:
            ~PosixFile() override
            {
                close();
            }

            String getPath() const override
            {
                return path;
            }

            bool isOpen() const override
            {
                return fd >= 0;
            }

            Outcome open(StringSlice p, FileMode mode, FileDescriptor const&) override
            {
                if (isOpen())
                {
                    return makeOutcome();
                }

                path = String(p.asPointer());
                int flags = toFlags(mode);
                int const perms = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
                int fd = ::open(path.asPointer(), flags, perms);
                if (fd < 0)
                {
                    return makeError();
                }

                this->fd = fd;
                return makeOutcome();
            }

            Outcome close() override
            {
                if (fd >= 0)
                {
                    ::close(fd);
                    fd = -1;
                }

                return makeOutcome();
            }

            Result<usize> read(void* buffer, usize count) override
            {
                if (fd < 0)
                {
                    return Result<usize>(unexpectedResult);
                }

                ssize_t readCount = ::read(fd, buffer, count);
                if (readCount < 0)
                {
                    return Result<usize>(unexpectedResult);
                }

                return static_cast<usize>(readCount);
            }

            Result<usize> write(void const* buffer, usize count) override
            {
                if (fd < 0)
                {
                    return Result<usize>(unexpectedResult);
                }

                ssize_t writeCount = ::write(fd, buffer, count);
                if (writeCount < 0)
                {
                    return Result<usize>(unexpectedResult);
                }

                return static_cast<usize>(writeCount);
            }

            Outcome flush() override
            {
                if (fd < 0)
                {
                    return makeError();
                }

                return (::fsync(fd) == 0) ? makeOutcome() : makeError();
            }

            Result<usize> getSize() const override
            {
                if (fd < 0)
                {
                    return Result<usize>(unexpectedResult);
                }

                struct stat st{};
                if (::fstat(fd, &st) != 0)
                {
                    return Result<usize>(unexpectedResult);
                }

                return static_cast<usize>(st.st_size);
            }

            Result<usize> getPosition() const override
            {
                if (fd < 0)
                {
                    return Result<usize>(unexpectedResult);
                }

                off_t position = ::lseek(fd, 0, SEEK_CUR);
                if (position < 0)
                {
                    return Result<usize>(unexpectedResult);
                }

                return static_cast<usize>(position);
            }

            Outcome setPosition(isize offset, SeekOrigin origin) override
            {
                if (fd < 0)
                {
                    return makeError();
                }

                int whence;
                switch (origin)
                {
                    case SeekOrigin::Begin:
                        whence = SEEK_SET;
                        break;

                    case SeekOrigin::Current:
                        whence = SEEK_CUR;
                        break;

                    case SeekOrigin::End:
                        whence = SEEK_END;
                        break;

                    default:
                        ARK_UNREACHABLE();
                        whence = SEEK_SET;
                        break;
                }

                return (::lseek(fd, static_cast<off_t>(offset), whence) >= 0) ? makeOutcome() : makeError();
            }

            Result<uint64> getModificationTime() const override
            {
                if (fd < 0)
                {
                    return Result<uint64>(unexpectedResult);
                }

                struct stat st{};
                if (::fstat(fd, &st) != 0)
                {
                    return Result<uint64>(unexpectedResult);
                }

#if defined(ARK_PLATFORM_APPLE)
                return static_cast<uint64>(st.st_mtimespec.tv_sec);
#elif defined(ARK_PLATFORM_LINUX)
                return static_cast<uint64>(st.st_mtim.tv_sec);
#else
                return static_cast<uint64>(st.st_mtime);
#endif
            }
        };
    }

    UniquePointer<File> File::create()
    {
        return makeUnique<PosixFile>();
    }

    UniquePointer<File> File::create(StringSlice path, FileMode mode, FileDescriptor const& descriptor)
    {
        auto file = makeUnique<PosixFile>();

        file->open(path, mode, descriptor);
        if (!file->isOpen())
        {
            return nullptr;
        }

        return file;
    }
}
