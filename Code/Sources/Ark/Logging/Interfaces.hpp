#pragma once

#include "Ark/Core/Types.hpp"
#include "Ark/Logging/Message.hpp"
#include "Ark/Strings/StringSlice.hpp"

namespace Ark::Logging
{
    /// Log Entry
    /// @details Represents a log message.
    struct Entry final
    {
        Message message;         ///< The log message.
        String formattedMessage; ///< The formatted log message (owned).
    };

    /// Interface for log message filtering.
    class Filter
    {
    public:
        virtual ~Filter() = default;

        /// Determines whether a log message should be logged.
        /// @param message The log message to check.
        /// @return `true` if the message should be logged, otherwise `false`.
        virtual bool shouldLog(const Message& message) const = 0;
    };

    /// Interface for log message formatting.
    class Formatter
    {
    public:
        virtual ~Formatter() = default;

        /// Formats a log message into a string.
        /// @param message The log message to format.
        /// @return The formatted log message as a string.
        virtual String format(const Message& message) = 0;
    };

    /// Interface for log output targets.
    class Target
    {
    public:
        virtual ~Target() = default;

        /// Writes a formatted message to the target.
        /// @param entry The log entry to write.
        virtual void write(const Entry& entry) = 0;

        /// Flushes any buffered log messages.
        virtual void flush() = 0;
    };
}
