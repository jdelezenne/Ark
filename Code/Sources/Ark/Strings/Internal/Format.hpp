#pragma once

#include "Ark/Core/Configuration.hpp"
#include "Ark/Core/Functions.hpp"
#include "Ark/Core/Types.hpp"

#if (ARK_USE_STANDARD_LIBRARY == ARK_STANDARD_LIBRARY_CPP)
#include <format>
#define ARK_FORMAT_NAMESPACE std
#else
#include <fmt/format.h>
#define ARK_FORMAT_NAMESPACE fmt
#endif

namespace Ark
{
#if (ARK_USE_STANDARD_LIBRARY == ARK_STANDARD_LIBRARY_CPP)
    /// Compile-time checked format string (`std::format_string` on CppStd).
    template <typename... Args>
    using FormatString = std::format_string<Args...>;
#else
    /// Compile-time checked format string (`fmt::format_string` when CppStd is disabled).
    template <typename... Args>
    using FormatString = fmt::format_string<Args...>;
#endif

    namespace Internal
    {
        template <typename... Args>
        [[nodiscard]] inline auto formatToText(FormatString<Args...> format, Args&&... args)
        {
#if (ARK_USE_STANDARD_LIBRARY == ARK_STANDARD_LIBRARY_CPP)
            return std::format(format, Ark::forward<Args>(args)...);
#else
            return fmt::format(format, Ark::forward<Args>(args)...);
#endif
        }
    }
}
