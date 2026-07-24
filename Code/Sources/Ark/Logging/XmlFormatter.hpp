#pragma once

#include "Ark/Logging/Interfaces.hpp"
#include "Ark/Strings/String.hpp"

namespace Ark::Logging
{
    /// XML-format log message formatter.
    /// Outputs log entries as XML elements with fields: timestamp, level, category, message.
    class XmlFormatter final : public Formatter
    {
    private:
        bool prettyPrint;

    public:
        /// Constructs an XML formatter.
        /// @param prettyPrint If true, output formatted XML with indentation.
        explicit XmlFormatter(bool prettyPrint = false)
            : prettyPrint{prettyPrint}
        {
        }

        /// Formats a log message as XML.
        /// @param message Log message to format.
        /// @return XML-formatted string.
        String format(const Message& message) override;

    private:
        void addElement(String& output, StringSlice name, StringSlice value, int indentLevel);

        String escapeXml(StringSlice data);

        static String formatTimestamp(uint64 timestamp);
    };
}
