#pragma once

#include "Ark/Collections/Array.hpp"
#include "Ark/Core/Configuration.hpp"
#include "Ark/Logging/Interfaces.hpp"
#include "Ark/Logging/Level.hpp"

#if (ARK_USE_STANDARD_LIBRARY == ARK_STANDARD_LIBRARY_CPP)
#include <regex>
#include <string>
#endif

namespace Ark::Logging
{
    /// Level-based log filter.
    /// Allows messages at or above a specified severity level.
    class LevelFilter final : public Filter
    {
    private:
        /// Current minimum log level.
        Level level{Level::None};

    public:
        LevelFilter() = default;

        explicit LevelFilter(Level minimumLevel)
            : level{minimumLevel}
        {
        }

        /// Determines if a message should be logged.
        /// @param message Log message to evaluate.
        /// @return True if message level >= filter level.
        bool shouldLog(const Message& message) const override;

        /// Gets the minimum log level.
        Level getLevel() const
        {
            return level;
        }

        /// Sets the minimum log level.
        /// @param value New minimum level.
        void setLevel(Level value)
        {
            level = value;
        }
    };

    /// Category-based log filter.
    /// Allows messages from specified category names.
    class CategoryFilter final : public Filter
    {
    private:
        /// Allowed category names.
        Collections::Array<String> categories;

    public:
        /// Determines if a message should be logged.
        /// @param message Log message to evaluate.
        /// @return True if message category is in allowed list.
        bool shouldLog(const Message& message) const override;

        /// Adds a category to the allowed list.
        /// @param category Category name to allow.
        void addCategory(StringSlice category);
    };

#if (ARK_USE_STANDARD_LIBRARY == ARK_STANDARD_LIBRARY_CPP)
    /// Regex-based log filter (CppStd backend only).
    /// Allows messages where the formatted text matches a regex pattern.
    class RegexFilter final : public Filter
    {
    private:
        /// Regular expression pattern to match against messages.
        std::regex regex;

    public:
        /// Constructs a regex filter.
        /// @param pattern Regular expression pattern.
        explicit RegexFilter(std::string const& pattern)
            : regex{pattern}
        {
        }

        /// Determines if a message should be logged.
        /// @param message Log message to evaluate.
        /// @return True if message text matches regex pattern.
        bool shouldLog(const Message& message) const override;
    };
#endif

    /// Time range-based log filter.
    /// Allows messages within a specified timestamp range.
    class TimestampFilter final : public Filter
    {
    private:
        /// Start timestamp (inclusive).
        uint64 startTime;
        /// End timestamp (inclusive).
        uint64 endTime;

    public:
        /// Constructs a timestamp range filter.
        /// @param startTime Start timestamp (milliseconds since epoch).
        /// @param endTime End timestamp (milliseconds since epoch).
        TimestampFilter(uint64 startTime, uint64 endTime)
            : startTime{startTime}
            , endTime{endTime}
        {
        }

        /// Determines if a message should be logged.
        /// @param message Log message to evaluate.
        /// @return True if message timestamp is within [startTime, endTime].
        bool shouldLog(const Message& message) const override;
    };
}
