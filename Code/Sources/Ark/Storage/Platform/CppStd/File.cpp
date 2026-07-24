#include "Ark/Storage/File.hpp"
#include <filesystem>
#include <fstream>

namespace Ark::Storage
{
    namespace
    {
        static std::ios::openmode toOpenMode(FileMode fileMode, FileDescriptor const& descriptor)
        {
            std::ios::openmode mode = std::ios::binary;

            if ((static_cast<uint>(descriptor.access) & static_cast<uint>(FileAccess::Read)) != 0)
            {
                mode |= std::ios::in;
            }

            if ((static_cast<uint>(descriptor.access) & static_cast<uint>(FileAccess::Write)) != 0)
            {
                mode |= std::ios::out;
            }

            switch (fileMode)
            {
                case FileMode::CreateNew:
                case FileMode::CreateAlways:
                case FileMode::TruncateExisting:
                    mode |= std::ios::out | std::ios::trunc;
                    break;

                case FileMode::OpenExisting:
                case FileMode::OpenAlways:
                default:
                    break;
            }

            return mode;
        }

        class CppStdFile final : public File
        {
        private:
            String path;
            std::fstream stream;

        public:
            ~CppStdFile() override
            {
                close();
            }

            String getPath() const override
            {
                return path;
            }

            bool isOpen() const override
            {
                return stream.is_open();
            }

            Outcome open(StringSlice p, FileMode mode, FileDescriptor const& descriptor) override
            {
                if (isOpen())
                {
                    return makeOutcome();
                }

                path = String(p.asPointer());

                namespace fs = std::filesystem;
                fs::path fsPath(path.asPointer());
                bool exists = fs::exists(fsPath);

                if (mode == FileMode::CreateNew && exists)
                {
                    return makeError();
                }
                if ((mode == FileMode::OpenExisting || mode == FileMode::TruncateExisting) && !exists)
                {
                    return makeError();
                }

                if (mode == FileMode::OpenAlways && !exists)
                {
                    std::ofstream create(path.asPointer(), std::ios::binary);
                }

                std::ios::openmode openMode = toOpenMode(mode, descriptor);
                stream.open(path.asPointer(), openMode);

                return stream.is_open() ? makeOutcome() : makeError();
            }

            Outcome close() override
            {
                if (stream.is_open())
                {
                    stream.close();
                }
                return makeOutcome();
            }

            Result<usize> read(void* buffer, usize count) override
            {
                if (!stream.is_open())
                {
                    return Result<usize>(unexpectedResult);
                }

                stream.read(reinterpret_cast<char*>(buffer), static_cast<std::streamsize>(count));
                auto const readCount = static_cast<usize>(stream.gcount());
                return readCount;
            }

            Result<usize> write(void const* buffer, usize count) override
            {
                if (!stream.is_open())
                {
                    return Result<usize>(unexpectedResult);
                }

                stream.write(reinterpret_cast<char const*>(buffer), static_cast<std::streamsize>(count));
                if (!stream)
                {
                    return Result<usize>(unexpectedResult);
                }
                return count;
            }

            Outcome flush() override
            {
                if (!stream.is_open())
                {
                    return makeError();
                }

                stream.flush();
                return stream ? makeOutcome() : makeError();
            }

            Result<usize> getSize() const override
            {
                namespace fs = std::filesystem;
                if (!stream.is_open())
                {
                    return Result<usize>(unexpectedResult);
                }

                std::error_code errorCode;
                auto sz = fs::file_size(fs::path(path.asPointer()), errorCode);
                if (errorCode)
                {
                    return Result<usize>(unexpectedResult);
                }
                return static_cast<usize>(sz);
            }

            Result<usize> getPosition() const override
            {
                if (!stream.is_open())
                {
                    return Result<usize>(unexpectedResult);
                }

                auto position = stream.tellg();
                if (position == static_cast<std::streampos>(-1))
                {
                    position = stream.tellp();
                }

                if (position == static_cast<std::streampos>(-1))
                {
                    return Result<usize>(unexpectedResult);
                }

                return static_cast<usize>(position);
            }

            Outcome setPosition(isize offset, SeekOrigin origin) override
            {
                if (!stream.is_open())
                {
                    return makeError();
                }

                std::ios::seekdir dir = std::ios::beg;
                switch (origin)
                {
                    case SeekOrigin::Begin:
                        dir = std::ios::beg;
                        break;

                    case SeekOrigin::Current:
                        dir = std::ios::cur;
                        break;

                    case SeekOrigin::End:
                        dir = std::ios::end;
                        break;

                    default:
                        ARK_UNREACHABLE();
                        return makeError();
                }

                stream.seekg(offset, dir);
                stream.seekp(offset, dir);
                return stream ? makeOutcome() : makeError();
            }

            Result<uint64> getModificationTime() const override
            {
                return Result<uint64>(unexpectedResult);
            }
        };
    }

    UniquePointer<File> File::create()
    {
        return makeUnique<CppStdFile>();
    }

    UniquePointer<File> File::create(StringSlice path, FileMode mode, FileDescriptor const& descriptor)
    {
        auto file = makeUnique<CppStdFile>();

        file->open(path, mode, descriptor);
        if (!file->isOpen())
        {
            return nullptr;
        }

        return file;
    }

}
