#include "Ark/Storage/File.hpp"
#include "Ark/Collections/Array.hpp"

namespace Ark::Storage
{
    Result<Collections::Array<uint8>> File::readAllBinary(StringSlice path)
    {
        auto file = File::create(path, FileMode::OpenExisting, {FileAccess::Read, FileShare::All});
        if (file == nullptr)
        {
            return Result<Collections::Array<uint8>>(unexpectedResult);
        }

        auto const size = file->getSize();
        if (!size)
        {
            return Result<Collections::Array<uint8>>(unexpectedResult);
        }

        Collections::Array<uint8> result;
        result.resize(*size);
        auto const bytesRead = file->read(result.asMutablePointer(), *size);
        if (!bytesRead || *bytesRead != *size)
        {
            return Result<Collections::Array<uint8>>(unexpectedResult);
        }
        return result;
    }

    Result<String> File::readAllText(StringSlice path)
    {
        auto file = File::create(path, FileMode::OpenExisting, {FileAccess::Read, FileShare::All});
        if (file == nullptr)
        {
            return Result<String>(unexpectedResult);
        }

        auto const size = file->getSize();
        if (!size)
        {
            return Result<String>(unexpectedResult);
        }

        String result;
        result.resize(*size);
        auto const bytesRead = file->read(result.asPointer(), *size);
        if (!bytesRead || *bytesRead != *size)
        {
            return Result<String>(unexpectedResult);
        }
        return result;
    }

    Outcome File::writeAllBinary(StringSlice path, Collections::Array<uint8> const& data)
    {
        auto file = File::create(path, FileMode::CreateAlways, {FileAccess::Write, FileShare::All});
        if (file == nullptr)
        {
            return makeError();
        }

        auto const bytesWritten = file->write(data.asPointer(), data.getCount());
        if (!bytesWritten || *bytesWritten != data.getCount())
        {
            return makeError();
        }
        return makeOutcome();
    }

    Outcome File::writeAllText(StringSlice path, StringSlice text)
    {
        auto file = File::create(path, FileMode::CreateAlways, {FileAccess::Write, FileShare::All});
        if (file == nullptr)
        {
            return makeError();
        }

        auto const bytesWritten = file->write(text.asPointer(), text.getLength());
        if (!bytesWritten || *bytesWritten != text.getLength())
        {
            return makeError();
        }
        return makeOutcome();
    }
}
