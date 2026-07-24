#pragma once

#include "Ark/Strings/Internal/Format.hpp"
#include "Ark/Strings/String.hpp"

namespace Ark
{
    /// Formats a message into an Ark `String`.
    /// Forwards to `std::format` (CppStd) or `fmt::format` (Generic/C).
    template <typename... Args>
    [[nodiscard]] inline String format(FormatString<Args...> format, Args&&... args)
    {
        return String::format(format, Ark::forward<Args>(args)...);
    }
}
