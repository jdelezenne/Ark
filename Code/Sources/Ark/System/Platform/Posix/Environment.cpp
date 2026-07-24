#if defined(ARK_PLATFORM_MACOS) || defined(ARK_PLATFORM_LINUX)

#include "Ark/System/Environment.hpp"

#include <cstdlib>

namespace Ark::System
{
    Result<String, String> getEnvironmentVariable(StringSlice name)
    {
        char const* value = std::getenv(name.asPointer());
        if (value == nullptr)
        {
            return Result<String, String>(unexpectedResult, String("Variable not found"));
        }
        return String(value);
    }

    Result<Void, String> setEnvironmentVariable(StringSlice name, StringSlice value)
    {
        if (setenv(name.asPointer(), value.asPointer(), 1) != 0)
        {
            return Result<Void, String>(unexpectedResult, String("setenv failed"));
        }
        return Result<Void, String>();
    }

    Result<Void, String> unsetEnvironmentVariable(StringSlice name)
    {
        if (unsetenv(name.asPointer()) != 0)
        {
            return Result<Void, String>(unexpectedResult, String("unsetenv failed"));
        }
        return Result<Void, String>();
    }
}

#endif
