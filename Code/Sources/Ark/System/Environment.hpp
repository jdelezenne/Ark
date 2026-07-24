#pragma once

#include "Ark/Core/Result.hpp"
#include "Ark/Strings/String.hpp"
#include "Ark/Strings/StringSlice.hpp"

namespace Ark::System
{
    /// Gets an environment variable value.
    /// @param name Variable name.
    /// @return Variable value on success, or an error string.
    Result<String, String> getEnvironmentVariable(StringSlice name);

    /// Sets or updates an environment variable.
    /// @param name Variable name.
    /// @param value Variable value.
    /// @return Success or an error string.
    Result<Void, String> setEnvironmentVariable(StringSlice name, StringSlice value);

    /// Removes an environment variable.
    /// @param name Variable name.
    /// @return Success or an error string.
    Result<Void, String> unsetEnvironmentVariable(StringSlice name);
}
