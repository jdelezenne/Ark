#pragma once

#include "Ark/Collections/Array.hpp"
#include "Ark/Core/Types.hpp"
#include "Ark/Strings/String.hpp"
#include "Ark/System/DateTime.hpp"

namespace Ark::System::SystemInfo
{
    struct MemoryInfo final
    {
        uint64 totalPhysicalBytes{0};
        uint64 availablePhysicalBytes{0};
        uint64 totalVirtualBytes{0};
        uint64 availableVirtualBytes{0};
        uint64 processResidentBytes{0};
        uint64 processVirtualBytes{0};
    };

    /// Application/UI theme preference.
    enum class Theme
    {
        Unknown,
        Light,
        Dark,
    };

    /// RGBA color with 8-bit channels (0-255).
    struct RgbaColor final
    {
        uint8 r{0};
        uint8 g{0};
        uint8 b{0};
        uint8 a{255};
    };

    /// Power status information for the current system.
    struct PowerStatus final
    {
        bool batteryPresent{false};
        bool charging{false};
        bool onBattery{false};
        int32 batteryPercent{-1};
        int32 minutesRemaining{-1};
    };

    /// Get the operating system name (e.g., "Windows", "macOS", "Linux").
    String getOsName();

    /// Get the operating system version (platform-specific format).
    String getOsVersion();

    /// Get the local machine/computer name (hostname).
    String getComputerName();

    /// Get the current user's login name.
    String getUserName();

    /// Get the current process name (executable name without path).
    String getCurrentProcessName();

    /// Get the command line arguments for the current process.
    Collections::Array<String> getCommandLineArguments();

    /// Get the current local date and time.
    DateTime getLocalDateTime();

    /// Get memory information for the system and current process.
    MemoryInfo getMemoryInfo();

    /// Get the current user locale identifier (platform-specific format, UTF-8).
    /// Examples: "en-US", "fr_FR", "en_US" or values from environment on Linux.
    String getLocale();

    /// Get the preferred application/UI theme.
    Theme getAppTheme();

    /// Get the system accent color if available; alpha may be 255 or platform-defined.
    /// Returns {0,0,0,0} when unavailable.
    RgbaColor getAccentColor();

    /// Get current power status (battery/AC/charging and estimates when available).
    PowerStatus getPowerStatus();
}
