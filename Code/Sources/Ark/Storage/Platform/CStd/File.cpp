#include "Ark/Storage/File.hpp"
#include <cstdio>

namespace Ark::Storage
{
    namespace
    {
        class CStdFile final : public File
        {
        private:
            String path;
            std::FILE* handle{nullptr};

        public:
            ~CStdFile() override
            {
                close();
            }

            String getPath() const override
            {
                return path;
            }

            bool isOpen() const override
            {
                return handle != nullptr;
            }

            Outcome open(StringSlice p, FileMode mode, FileDescriptor const&) override
            {
                if (isOpen())
                {
                    return makeOutcome();
                }

                path = String(p.asPointer());
                char const* m = nullptr;
                switch (mode)
                {
                    case FileMode::CreateNew:
                        m = "wbx";
                        break;
                    case FileMode::CreateAlways:
                        m = "wb";
                        break;
                    case FileMode::OpenExisting:
                        m = "rb";
                        break;
                    case FileMode::OpenAlways:
                        m = "ab+";
                        break;
                    case FileMode::TruncateExisting:
                        m = "wb";
                        break;
                    default:
                        return makeError();
                }
                handle = std::fopen(path.asPointer(), m);
                return (handle != nullptr) ? makeOutcome() : makeError();
            }

            Outcome close() override
            {
                if (handle != nullptr)
                {
                    std::fclose(handle);
                    handle = nullptr;
                }
                return makeOutcome();
            }

            Result<usize> read(void* buffer, usize count) override
            {
                if (handle == nullptr)
                {
                    return Result<usize>(unexpectedResult);
                }
                auto const n = std::fread(buffer, 1, count, handle);
                return static_cast<usize>(n);
            }

            Result<usize> write(void const* buffer, usize count) override
            {
                if (handle == nullptr)
                {
                    return Result<usize>(unexpectedResult);
                }
                auto const n = std::fwrite(buffer, 1, count, handle);
                return static_cast<usize>(n);
            }

            Outcome flush() override
            {
                if (handle == nullptr)
                {
                    return makeError();
                }
                return (std::fflush(handle) == 0) ? makeOutcome() : makeError();
            }

            Result<usize> getSize() const override
            {
                if (handle == nullptr)
                {
                    return Result<usize>(unexpectedResult);
                }
                auto const position = std::ftell(handle);
                if (position < 0)
                {
                    return Result<usize>(unexpectedResult);
                }
                if (std::fseek(handle, 0, SEEK_END) != 0)
                {
                    return Result<usize>(unexpectedResult);
                }
                auto const end = std::ftell(handle);
                if (end < 0)
                {
                    std::fseek(handle, position, SEEK_SET);
                    return Result<usize>(unexpectedResult);
                }
                std::fseek(handle, position, SEEK_SET);
                return static_cast<usize>(end);
            }

            Result<usize> getPosition() const override
            {
                if (handle == nullptr)
                {
                    return Result<usize>(unexpectedResult);
                }
                auto const position = std::ftell(handle);
                if (position < 0)
                {
                    return Result<usize>(unexpectedResult);
                }
                return static_cast<usize>(position);
            }

            Outcome setPosition(isize offset, SeekOrigin origin) override
            {
                if (handle == nullptr)
                {
                    return makeError();
                }
                int whence = SEEK_SET;
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
                }
                return (std::fseek(handle, static_cast<long>(offset), whence) == 0) ? makeOutcome() : makeError();
            }

            Result<uint64> getModificationTime() const override
            {
                return Result<uint64>(unexpectedResult);
            }
        };
    }

    UniquePointer<File> File::create()
    {
        return makeUnique<CStdFile>();
    }

    UniquePointer<File> File::create(StringSlice path, FileMode mode, FileDescriptor const& descriptor)
    {
        auto f = makeUnique<CStdFile>();
        f->open(path, mode, descriptor);
        if (!f->isOpen())
        {
            return {};
        }
        return f;
    }
}
