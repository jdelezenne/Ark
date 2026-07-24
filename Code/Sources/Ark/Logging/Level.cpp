#include "Ark/Logging/Level.hpp"

namespace Ark::Logging
{
    StringSlice levelToString(Level level)
    {
        switch (level)
        {
            case Level::Debug:
                return "Debug";
            case Level::Info:
                return "Info";
            case Level::Warning:
                return "Warning";
            case Level::Error:
                return "Error";
            case Level::Fatal:
                return "Fatal";
            default:
                return "Unknown";
        }
    }
}
