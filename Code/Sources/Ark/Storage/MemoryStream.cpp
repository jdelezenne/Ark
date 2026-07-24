#include "Ark/Storage/MemoryStream.hpp"
#include "Ark/Core/Debug/Assert.hpp"

namespace Ark::Storage
{
    MemoryStream::MemoryStream(bool isReadOnly)
        : buffer{nullptr}
        , size{0}
        , readOnly{isReadOnly}
    {
    }

    MemoryStream::MemoryStream(void const* buffer, usize count, bool isReadOnly)
        : buffer{reinterpret_cast<uint8 const*>(buffer)}
        , size{count}
        , readOnly{isReadOnly}
    {
        ARK_ASSERT(buffer != nullptr || count == 0);
    }

    MemoryStream::~MemoryStream()
    {
        close();
    }

    bool MemoryStream::isReadOnly() const
    {
        return readOnly;
    }

    bool MemoryStream::canSeek() const
    {
        return true;
    }

    bool MemoryStream::canRead() const
    {
        return true;
    }

    bool MemoryStream::canWrite() const
    {
        return !readOnly;
    }

    bool MemoryStream::isOpen() const
    {
        return buffer != nullptr;
    }

    Outcome MemoryStream::close()
    {
        buffer = nullptr;
        size = 0;
        return makeOutcome();
    }

    Result<usize> MemoryStream::read(void* buffer, usize count)
    {
        ARK_ASSERT(buffer != nullptr);

        if (position >= this->size)
        {
            return Result<usize>(unexpectedResult);
        }

        usize const bytesLeft = this->size - position;
        if (count > bytesLeft)
        {
            count = bytesLeft;
        }

        if (count > 0)
        {
            Memory::copy(this->buffer + position, buffer, count);
            position += count;
        }

        return count;
    }

    Result<usize> MemoryStream::write(void const* buffer, usize count)
    {
        ARK_ASSERT(buffer != nullptr);

        if (readOnly)
        {
            ARK_ASSERT_FAIL_MSG("Cannot write to a stream that is read-only.");
            return Result<usize>(unexpectedResult);
        }

        if (position >= this->size)
        {
            return Result<usize>(unexpectedResult);
        }

        usize const bytesLeft = this->size - position;
        if (count > bytesLeft)
        {
            count = bytesLeft;
        }

        if (count > 0)
        {
            Memory::copy(buffer, const_cast<uint8*>(this->buffer) + position, count);
            position += count;
        }

        return count;
    }

    Outcome MemoryStream::flush()
    {
        return makeOutcome();
    }

    Result<usize> MemoryStream::getSize() const
    {
        return size;
    }

    Result<usize> MemoryStream::getPosition() const
    {
        return position;
    }

    Outcome MemoryStream::setPosition(isize position, SeekOrigin seekOrigin)
    {
        usize absPosition = static_cast<usize>(position < 0 ? -position : position);

        switch (seekOrigin)
        {
            case SeekOrigin::Begin:
            {
                if (position > 0)
                {
                    this->position = absPosition;
                    return makeOutcome();
                }
                else
                {
                    this->position = 0;
                    return makeOutcome();
                }
            }
            break;

            case SeekOrigin::Current:
            {
                if (position > 0)
                {
                    this->position = this->position + absPosition;
                    return makeOutcome();
                }
                else
                {
                    if (this->position >= absPosition)
                    {
                        this->position = this->position - absPosition;
                        return makeOutcome();
                    }
                    else
                    {
                        this->position = 0;
                        return makeOutcome();
                    }
                }
            }
            break;

            case SeekOrigin::End:
            {
                if (position > 0)
                {
                    this->position = this->size + absPosition;
                    return makeOutcome();
                }
                else
                {
                    if (this->size >= absPosition)
                    {
                        this->position = this->size - absPosition;
                        return makeOutcome();
                    }
                    else
                    {
                        this->position = 0;
                        return makeOutcome();
                    }
                }
            }
            break;

            default:
                ARK_ASSERT_FAIL();
                return makeError();
        }

        return makeOutcome();
    }
}
