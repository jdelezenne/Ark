#pragma once

#include "Ark/Core/Core.hpp"

namespace Ark
{
    /// @defgroup Literals Binary Size Literals
    /// Convenient suffix operators for expressing sizes in kilobytes, megabytes, and gigabytes.
    /// @{

    /// Kilobyte literal (1024 bytes).
    /// @param Size Number of kilobytes.
    /// @return Size in bytes.
    constexpr usize operator""_kb(unsigned long long Size)
    {
        return static_cast<usize>(Size * 1024);
    }

    /// Megabyte literal (1024 kilobytes).
    /// @param Size Number of megabytes.
    /// @return Size in bytes.
    constexpr usize operator""_mb(unsigned long long Size)
    {
        return static_cast<usize>(Size * 1024_kb);
    }

    /// Gigabyte literal (1024 megabytes).
    /// @param Size Number of gigabytes.
    /// @return Size in bytes.
    constexpr usize operator""_gb(unsigned long long Size)
    {
        return static_cast<usize>(Size * 1024_mb);
    }

    /// @}
}
