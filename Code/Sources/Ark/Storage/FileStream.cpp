#include "Ark/Storage/FileStream.hpp"

namespace Ark::Storage
{
    FileStream::FileStream(StringSlice path, FileMode mode, FileDescriptor const& descriptor)
    {
        file = File::create(path, mode, descriptor);
    }

    FileStream::~FileStream()
    {
        close();
    }

    String FileStream::getPath() const
    {
        if (file == nullptr)
        {
            return String();
        }

        return file->getPath();
    }

    bool FileStream::canSeek() const
    {
        return true;
    }

    bool FileStream::canRead() const
    {
        return true;
    }

    bool FileStream::canWrite() const
    {
        return true;
    }

    bool FileStream::isOpen() const
    {
        if (file == nullptr)
        {
            return false;
        }

        return file->isOpen();
    }

    Outcome FileStream::open(StringSlice path, FileMode mode, FileDescriptor const& descriptor)
    {
        if (file != nullptr)
        {
            close();
        }

        file = File::create(path, mode, descriptor);
        return file != nullptr ? makeOutcome() : makeError();
    }

    Outcome FileStream::close()
    {
        if (file == nullptr)
        {
            return makeOutcome();
        }

        Outcome const closed = file->close();
        file = nullptr;
        return closed;
    }

    Result<usize> FileStream::read(void* buffer, usize count)
    {
        if (file == nullptr)
        {
            return Result<usize>(unexpectedResult);
        }

        return file->read(buffer, count);
    }

    Result<usize> FileStream::write(void const* buffer, usize count)
    {
        if (file == nullptr)
        {
            return Result<usize>(unexpectedResult);
        }

        return file->write(buffer, count);
    }

    Outcome FileStream::flush()
    {
        if (file == nullptr)
        {
            return makeError();
        }

        return file->flush();
    }

    Result<usize> FileStream::getSize() const
    {
        if (file == nullptr)
        {
            return Result<usize>(unexpectedResult);
        }

        return file->getSize();
    }

    Result<usize> FileStream::getPosition() const
    {
        if (file == nullptr)
        {
            return Result<usize>(unexpectedResult);
        }

        return file->getPosition();
    }

    Outcome FileStream::setPosition(isize position, SeekOrigin seekOrigin)
    {
        if (file == nullptr)
        {
            return makeError();
        }

        return file->setPosition(position, seekOrigin);
    }
}
