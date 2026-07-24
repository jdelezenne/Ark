#pragma once

#include "Ark/Strings/StringSlice.hpp"

namespace Ark::Logging
{
    /// Represents the severity level of a log message.
    /// Log message severity level.
    enum class Level
    {
        None = 0,    ///< No logging (disabled).
        Debug = 1,   ///< Debug-level message (low priority, dev-time).
        Info = 2,    ///< Informational message (routine operation).
        Warning = 3, ///< Warning message (potential issue).
        Error = 4,   ///< Error message (failure occurred).
        Fatal = 5,   ///< Fatal error message (unrecoverable).
    };

    /// Converts a log level to its string representation.
    /// @param level Log severity level.
    /// @return String name of the level (e.g., "Debug", "Error").
    StringSlice levelToString(Level level);
}
