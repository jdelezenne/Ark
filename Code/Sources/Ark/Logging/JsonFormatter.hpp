#pragma once

#include "Ark/Logging/Interfaces.hpp"

namespace Ark::Logging
{
    /// JSON-format log message formatter.
    /// Outputs log entries as JSON objects with fields: timestamp, level, category, message.
    class JsonFormatter final : public Formatter
    {
    private:
        bool prettyPrint;

    public:
        /// Constructs a JSON formatter.
        /// @param prettyPrint If true, output formatted JSON with indentation.
        explicit JsonFormatter(bool prettyPrint = false)
            : prettyPrint{prettyPrint}
        {
        }

        /// Formats a log message as JSON.
        /// @param message Log message to format.
        /// @return JSON-formatted string.
        String format(const Message& message) override;
    };
}
