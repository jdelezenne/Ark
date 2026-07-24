#include "Ark/Logging/XmlFormatter.hpp"
#include "Ark/Logging/FormatUtilities.hpp"
#include "Ark/Logging/Message.hpp"

namespace Ark::Logging
{
    String XmlFormatter::format(const Message& message)
    {
        String output;
        output.append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
        output.append("<message>\n");
        addElement(output, "timestamp", formatTimestamp(message.timestamp), 1);
        addElement(output, "level", levelToString(message.level), 1);
        addElement(output, "category", message.category, 1);
        addElement(output, "message", message.message, 1);
        addElement(output, "thread_id", Internal::formatUnsigned(message.threadId), 1);

        output.append("  <source_location>\n");
        addElement(output, "file", message.location.fileName, 2);
        addElement(output, "line", Internal::formatUnsigned(message.location.lineNumber), 2);
        addElement(output, "function", message.location.functionName, 2);
        output.append("  </source_location>\n");

        output.append("</message>\n");
        return output;
    }

    void XmlFormatter::addElement(String& output, StringSlice name, StringSlice value, int indentLevel)
    {
        if (prettyPrint)
        {
            for (int i = 0; i < indentLevel * 2; ++i)
            {
                output.append(' ');
            }
        }

        output.append('<');
        output.append(name);
        output.append('>');
        output.append(escapeXml(value));
        output.append("</");
        output.append(name);
        output.append(">\n");
    }

    String XmlFormatter::escapeXml(StringSlice data)
    {
        String buffer;
        buffer.reserve(data.getLength());
        for (usize index = 0; index != data.getLength(); ++index)
        {
            switch (data[index])
            {
                case '&':
                    buffer.append("&amp;");
                    break;
                case '\"':
                    buffer.append("&quot;");
                    break;
                case '\'':
                    buffer.append("&apos;");
                    break;
                case '<':
                    buffer.append("&lt;");
                    break;
                case '>':
                    buffer.append("&gt;");
                    break;
                default:
                    buffer.append(data[index]);
                    break;
            }
        }
        return buffer;
    }

    String XmlFormatter::formatTimestamp(uint64 timestamp)
    {
        return Internal::formatDateTime(timestamp, '-', 'T', true);
    }
}
