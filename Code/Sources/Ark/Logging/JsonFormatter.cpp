#include "Ark/Logging/JsonFormatter.hpp"
#include "Ark/Logging/Message.hpp"

namespace Ark::Logging
{
    namespace
    {
        String escapeJson(StringSlice data)
        {
            String buffer;
            buffer.reserve(data.getLength());
            for (usize index = 0; index != data.getLength(); ++index)
            {
                char const ch = data[index];
                switch (ch)
                {
                    case '\"':
                        buffer.append("\\\"");
                        break;
                    case '\\':
                        buffer.append("\\\\");
                        break;
                    case '\b':
                        buffer.append("\\b");
                        break;
                    case '\f':
                        buffer.append("\\f");
                        break;
                    case '\n':
                        buffer.append("\\n");
                        break;
                    case '\r':
                        buffer.append("\\r");
                        break;
                    case '\t':
                        buffer.append("\\t");
                        break;
                    default:
                        if (static_cast<unsigned char>(ch) < 0x20)
                        {
                            char hex[7] = {'\\', 'u', '0', '0', '0', '0', '\0'};
                            constexpr char digits[] = "0123456789abcdef";
                            hex[4] = digits[(static_cast<unsigned char>(ch) >> 4) & 0xF];
                            hex[5] = digits[static_cast<unsigned char>(ch) & 0xF];
                            buffer.append(hex);
                        }
                        else
                        {
                            buffer.append(ch);
                        }
                        break;
                }
            }
            return buffer;
        }

        void appendField(String& output, StringSlice key, StringSlice value, bool prettyPrint, bool isLast)
        {
            if (prettyPrint)
            {
                output.append("  ");
            }
            output.append("\"");
            output.append(key);
            output.append("\":\"");
            output.append(escapeJson(value));
            output.append("\"");
            if (!isLast)
            {
                output.append(",");
            }
            if (prettyPrint)
            {
                output.append("\n");
            }
        }

        void appendNumberField(String& output, StringSlice key, uint64 value, bool prettyPrint, bool isLast)
        {
            if (prettyPrint)
            {
                output.append("  ");
            }
            output.append("\"");
            output.append(key);
            output.append("\":");

            char digits[32]{};
            usize index = sizeof(digits);
            uint64 remaining = value;
            do
            {
                digits[--index] = static_cast<char>('0' + (remaining % 10));
                remaining /= 10;
            }
            while (remaining != 0);

            output.append(StringSlice(digits + index, sizeof(digits) - index));
            if (!isLast)
            {
                output.append(",");
            }
            if (prettyPrint)
            {
                output.append("\n");
            }
        }
    }

    String JsonFormatter::format(Message const& message)
    {
        String output;
        output.append("{");
        if (prettyPrint)
        {
            output.append("\n");
        }

        appendField(output, "message", message.message, prettyPrint, false);
        appendField(output, "level", levelToString(message.level), prettyPrint, false);
        appendField(output, "category", message.category, prettyPrint, false);
        appendNumberField(output, "timestamp", message.timestamp, prettyPrint, false);
        appendNumberField(output, "threadId", message.threadId, prettyPrint, false);
        appendField(output, "fileName", message.location.fileName, prettyPrint, false);
        appendNumberField(output, "lineNumber", message.location.lineNumber, prettyPrint, false);
        appendField(output, "functionName", message.location.functionName, prettyPrint, true);

        if (prettyPrint)
        {
            output.append("}");
        }
        else
        {
            output.append("}");
        }
        return output;
    }
}
