#pragma once

#include "Ark/Collections/HashMap.hpp"
#include "Ark/Logging/Interfaces.hpp"
#include "Ark/Strings/StringSlice.hpp"

namespace Ark::Logging
{
    /// Pattern-based log message formatter.
    /// Formats messages using a pattern string with named replacements.
    /// Supports tokens like {timestamp}, {level}, {category}, {message}, etc.
    class PatternFormatter final : public Formatter
    {
    private:
        using ReplaceCallback = String (*)(const Message&);
        static Collections::HashMap<String, ReplaceCallback> patternMap;

        String pattern;

    public:
        /// Constructs a pattern formatter.
        /// @param pattern Format string with {timestamp}, {level}, {category}, {message} tokens.
        explicit PatternFormatter(StringSlice pattern);

        /// Formats a log message according to the pattern.
        /// @param message Log message to format.
        /// @return Formatted string.
        String format(const Message& message) override;

    private:
        static void initialize();

        static String formatTimestamp(uint64 timestamp);

        static String formatDateTime(uint64 timestamp);

        static String formatDate(uint64 timestamp);

        static String formatTime(uint64 timestamp);
    };
}
