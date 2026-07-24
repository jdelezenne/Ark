#include "Ark/Storage/UnsafeMemoryStream.hpp"

namespace Ark::Storage
{
    UnsafeMemoryStream::UnsafeMemoryStream(void* buffer, usize capacity, bool isReadOnly)
        : readOnly{isReadOnly}
        , buffer{reinterpret_cast<uint8*>(buffer)}
        , capacity{capacity}
    {
    }

    UnsafeMemoryStream::~UnsafeMemoryStream()
    {
        close();
    }

    bool UnsafeMemoryStream::isOpen() const
    {
        return buffer != nullptr && capacity > 0;
    }

    bool UnsafeMemoryStream::canSeek() const
    {
        return true;
    }

    bool UnsafeMemoryStream::canRead() const
    {
        return true;
    }

    bool UnsafeMemoryStream::canWrite() const
    {
        return !readOnly;
    }

    Outcome UnsafeMemoryStream::close()
    {
        buffer = nullptr;
        capacity = 0;
        position = 0;
        return makeOutcome();
    }

    Outcome UnsafeMemoryStream::flush()
    {
        return makeOutcome();
    }

    Result<usize> UnsafeMemoryStream::getSize() const
    {
        return capacity;
    }

    Result<usize> UnsafeMemoryStream::getPosition() const
    {
        return position;
    }

    Outcome UnsafeMemoryStream::setPosition(isize position, SeekOrigin origin)
    {
        usize absPos = static_cast<usize>(position < 0 ? -position : position);
        switch (origin)
        {
            case SeekOrigin::Begin:
            {
                this->position = absPos;
                break;
            }
            case SeekOrigin::Current:
            {
                if (position >= 0)
                {
                    this->position += absPos;
                }
                else
                {
                    this->position = (this->position >= absPos) ? this->position - absPos : 0;
                }
                break;
            }
            case SeekOrigin::End:
            {
                if (position >= 0)
                {
                    this->position = capacity + absPos;
                }
                else
                {
                    this->position = (capacity >= absPos) ? capacity - absPos : 0;
                }
                break;
            }
            default:
            {
                return makeError();
            }
        }
        return makeOutcome();
    }

    Result<usize> UnsafeMemoryStream::read(void* outBuffer, usize count)
    {
        ARK_ASSERT(outBuffer != nullptr);
        if (buffer == nullptr || position >= capacity)
        {
            return Result<usize>(unexpectedResult);
        }

        usize const bytesLeft = capacity - position;
        if (count > bytesLeft)
        {
            count = bytesLeft;
        }

        Memory::copy(buffer + position, outBuffer, count);
        position += count;
        return count;
    }

    Result<usize> UnsafeMemoryStream::write(void const* inBuffer, usize count)
    {
        ARK_ASSERT(inBuffer != nullptr);
        if (readOnly || buffer == nullptr || position >= capacity)
        {
            return Result<usize>(unexpectedResult);
        }

        usize const bytesLeft = capacity - position;
        if (count > bytesLeft)
        {
            count = bytesLeft;
        }

        Memory::copy(inBuffer, buffer + position, count);
        position += count;
        return count;
    }
}
