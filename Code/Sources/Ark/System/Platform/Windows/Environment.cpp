#if defined(ARK_PLATFORM_WINDOWS)

#include "Ark/System/Environment.hpp"
#include "Ark/Strings/Unicode.hpp"

#include <Windows.h>

namespace Ark::System
{
    Result<String, String> getEnvironmentVariable(StringSlice name)
    {
        auto wideName = Unicode::toWide(name);
        DWORD size = GetEnvironmentVariableW(wideName.asPointer(), nullptr, 0);
        if (size == 0)
        {
            return Result<String, String>(unexpectedResult, String("Variable not found"));
        }
        Collections::Array<wchar_t> buffer;
        buffer.resize(size);
        GetEnvironmentVariableW(wideName.asPointer(), buffer.asMutablePointer(), size);
        return Unicode::fromWide(buffer.asPointer());
    }

    Result<Void, String> setEnvironmentVariable(StringSlice name, StringSlice value)
    {
        auto wideName = Unicode::toWide(name);
        auto wideValue = Unicode::toWide(value);
        if (!SetEnvironmentVariableW(wideName.asPointer(), wideValue.asPointer()))
        {
            return Result<Void, String>(unexpectedResult, String("SetEnvironmentVariable failed"));
        }
        return Result<Void, String>();
    }

    Result<Void, String> unsetEnvironmentVariable(StringSlice name)
    {
        auto wideName = Unicode::toWide(name);
        if (!SetEnvironmentVariableW(wideName.asPointer(), nullptr))
        {
            return Result<Void, String>(unexpectedResult, String("UnsetEnvironmentVariable failed"));
        }
        return Result<Void, String>();
    }
}

#endif
