#pragma once

#include "Ark/Collections/Array.hpp"
#include "Ark/Core/NumericLimits.hpp"
#include "Ark/Core/Pair.hpp"
#include "Ark/Core/Types.hpp"
#include "Ark/Strings/AnsiConversions.hpp"
#include "Ark/Strings/AnsiStrings.hpp"
#include "Ark/Strings/String.hpp"
#include "Ark/Strings/StringSlice.hpp"

namespace Ark::Documents::Details
{
    enum class JsonArkType : uint8
    {
        Null,
        Object,
        Array,
        String,
        Bool,
        Number,
    };

    struct JsonArkValue final
    {
        JsonArkType type = JsonArkType::Null;
        Collections::Array<Pair<String, JsonArkValue>> objectValues;
        Collections::Array<JsonArkValue> arrayValues;
        String stringValue;
        bool boolValue = false;
        double numberValue = 0.0;
        int64 signedValue = 0;
        uint64 unsignedValue = 0;
        bool hasSigned = false;
        bool hasUnsigned = false;
    };

    struct JsonArkDocument final
    {
        JsonArkValue root;
        String error;
        bool isValid = false;
    };

    inline void jsonArkReset(JsonArkDocument& document)
    {
        document.root = JsonArkValue();
        document.error = String();
        document.isValid = false;
    }

    inline bool jsonArkEquals(StringSlice lhs, String const& rhs)
    {
        return lhs.isEqual(rhs.toSlice());
    }

    inline JsonArkValue const* jsonArkFindMember(JsonArkValue const& value, StringSlice name)
    {
        if (value.type != JsonArkType::Object)
        {
            return nullptr;
        }

        for (auto const& member : value.objectValues)
        {
            if (jsonArkEquals(name, member.first))
            {
                return &member.second;
            }
        }
        return nullptr;
    }

    class JsonArkParser final
    {
    private:
        char const* data = nullptr;
        usize length = 0;
        usize cursor = 0;

    private:
        static bool isDigit(char c)
        {
            return c >= '0' && c <= '9';
        }

        char peek() const
        {
            return cursor < length ? data[cursor] : '\0';
        }

        bool consume(char expected)
        {
            if (peek() != expected)
            {
                return false;
            }
            ++cursor;
            return true;
        }

        void skipWhitespace()
        {
            while (cursor < length && Characters::isWhitespace(data[cursor]))
            {
                ++cursor;
            }
        }

        bool parseLiteral(char const* literal)
        {
            usize const literalLength = AnsiStrings::getLengthUnsafe(literal);
            for (usize i = 0; literal[i] != '\0'; ++i)
            {
                if (cursor + i >= length || data[cursor + i] != literal[i])
                {
                    return false;
                }
            }
            cursor += literalLength;
            return true;
        }

        static void appendUtf8(String& output, uint32 codepoint)
        {
            if (codepoint <= 0x7F)
            {
                output.append(static_cast<char>(codepoint));
            }
            else if (codepoint <= 0x7FF)
            {
                output.append(static_cast<char>(0xC0 | (codepoint >> 6)));
                output.append(static_cast<char>(0x80 | (codepoint & 0x3F)));
            }
            else if (codepoint <= 0xFFFF)
            {
                output.append(static_cast<char>(0xE0 | (codepoint >> 12)));
                output.append(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
                output.append(static_cast<char>(0x80 | (codepoint & 0x3F)));
            }
            else
            {
                output.append(static_cast<char>(0xF0 | (codepoint >> 18)));
                output.append(static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F)));
                output.append(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
                output.append(static_cast<char>(0x80 | (codepoint & 0x3F)));
            }
        }

        bool parseHex4(uint32& value)
        {
            value = 0;
            for (int i = 0; i < 4; ++i)
            {
                char const c = peek();
                if (c == '\0')
                {
                    return false;
                }
                value <<= 4;
                if (c >= '0' && c <= '9')
                {
                    value |= static_cast<uint32>(c - '0');
                }
                else if (c >= 'a' && c <= 'f')
                {
                    value |= static_cast<uint32>(10 + c - 'a');
                }
                else if (c >= 'A' && c <= 'F')
                {
                    value |= static_cast<uint32>(10 + c - 'A');
                }
                else
                {
                    return false;
                }
                ++cursor;
            }
            return true;
        }

        bool parseString(String& output)
        {
            if (!consume('"'))
            {
                return false;
            }

            output.clear();
            while (cursor < length)
            {
                char const c = data[cursor++];
                if (c == '"')
                {
                    return true;
                }
                if (c == '\\')
                {
                    if (cursor >= length)
                    {
                        return false;
                    }

                    char const escaped = data[cursor++];
                    switch (escaped)
                    {
                        case '"':
                        case '\\':
                        case '/':
                            output.append(escaped);
                            break;
                        case 'b':
                            output.append('\b');
                            break;
                        case 'f':
                            output.append('\f');
                            break;
                        case 'n':
                            output.append('\n');
                            break;
                        case 'r':
                            output.append('\r');
                            break;
                        case 't':
                            output.append('\t');
                            break;
                        case 'u':
                        {
                            uint32 codepoint = 0;
                            if (!parseHex4(codepoint))
                            {
                                return false;
                            }
                            appendUtf8(output, codepoint);
                            break;
                        }
                        default:
                            return false;
                    }
                    continue;
                }

                if (static_cast<unsigned char>(c) < 0x20)
                {
                    return false;
                }
                output.append(c);
            }

            return false;
        }

        bool parseNumber(JsonArkValue& output)
        {
            usize const begin = cursor;

            if (peek() == '-')
            {
                ++cursor;
            }
            if (!isDigit(peek()))
            {
                return false;
            }

            if (peek() == '0')
            {
                ++cursor;
            }
            else
            {
                while (isDigit(peek()))
                {
                    ++cursor;
                }
            }

            bool isFloating = false;
            if (peek() == '.')
            {
                isFloating = true;
                ++cursor;
                if (!isDigit(peek()))
                {
                    return false;
                }
                while (isDigit(peek()))
                {
                    ++cursor;
                }
            }

            if (peek() == 'e' || peek() == 'E')
            {
                isFloating = true;
                ++cursor;
                if (peek() == '+' || peek() == '-')
                {
                    ++cursor;
                }
                if (!isDigit(peek()))
                {
                    return false;
                }
                while (isDigit(peek()))
                {
                    ++cursor;
                }
            }

            StringSlice const token(data + begin, cursor - begin);
            output = JsonArkValue();
            output.type = JsonArkType::Number;

            if (isFloating)
            {
                Option<float64> const value = Strings::toFloat<float64>(token);
                if (!value.hasValue())
                {
                    return false;
                }
                output.numberValue = value.getValue();
                return true;
            }

            if (!token.isEmpty() && token[0] == '-')
            {
                Option<int64> const value = Strings::toInt<int64>(token, 10);
                if (!value.hasValue())
                {
                    return false;
                }
                output.signedValue = value.getValue();
                output.numberValue = static_cast<double>(value.getValue());
                output.hasSigned = true;
                return true;
            }

            Option<uint64> const value = Strings::toInt<uint64>(token, 10);
            if (!value.hasValue())
            {
                return false;
            }

            output.unsignedValue = value.getValue();
            output.numberValue = static_cast<double>(value.getValue());
            output.hasUnsigned = true;
            if (value.getValue() <= static_cast<uint64>(NumericLimits<int64>::max()))
            {
                output.signedValue = static_cast<int64>(value.getValue());
                output.hasSigned = true;
            }
            return true;
        }

        bool parseArray(JsonArkValue& output)
        {
            if (!consume('['))
            {
                return false;
            }

            output = JsonArkValue();
            output.type = JsonArkType::Array;

            skipWhitespace();
            if (consume(']'))
            {
                return true;
            }

            while (true)
            {
                auto& value = output.arrayValues.appendMake();
                if (!parseValue(value))
                {
                    return false;
                }
                skipWhitespace();

                if (consume(']'))
                {
                    return true;
                }
                if (!consume(','))
                {
                    return false;
                }
                skipWhitespace();
            }
        }

        bool parseObject(JsonArkValue& output)
        {
            if (!consume('{'))
            {
                return false;
            }

            output = JsonArkValue();
            output.type = JsonArkType::Object;

            skipWhitespace();
            if (consume('}'))
            {
                return true;
            }

            while (true)
            {
                String key;
                if (!parseString(key))
                {
                    return false;
                }
                skipWhitespace();
                if (!consume(':'))
                {
                    return false;
                }
                skipWhitespace();

                auto& member = output.objectValues.appendMake();
                member.first = Ark::move(key);
                if (!parseValue(member.second))
                {
                    return false;
                }
                skipWhitespace();

                if (consume('}'))
                {
                    return true;
                }
                if (!consume(','))
                {
                    return false;
                }
                skipWhitespace();
            }
        }

    public:
        JsonArkParser(char const* text, usize size)
            : data(text)
            , length(size)
            , cursor(0)
        {
        }

        bool parseValue(JsonArkValue& output)
        {
            skipWhitespace();
            char const c = peek();

            if (c == '{')
            {
                return parseObject(output);
            }
            if (c == '[')
            {
                return parseArray(output);
            }
            if (c == '"')
            {
                output = JsonArkValue();
                output.type = JsonArkType::String;
                return parseString(output.stringValue);
            }
            if (c == 't')
            {
                if (!parseLiteral("true"))
                {
                    return false;
                }
                output = JsonArkValue();
                output.type = JsonArkType::Bool;
                output.boolValue = true;
                return true;
            }
            if (c == 'f')
            {
                if (!parseLiteral("false"))
                {
                    return false;
                }
                output = JsonArkValue();
                output.type = JsonArkType::Bool;
                output.boolValue = false;
                return true;
            }
            if (c == 'n')
            {
                if (!parseLiteral("null"))
                {
                    return false;
                }
                output = JsonArkValue();
                output.type = JsonArkType::Null;
                return true;
            }
            return parseNumber(output);
        }

        bool parseDocument(JsonArkDocument& document)
        {
            jsonArkReset(document);
            if (!parseValue(document.root))
            {
                document.error = String("Invalid JSON.");
                return false;
            }

            skipWhitespace();
            if (cursor != length)
            {
                document.error = String("Unexpected trailing characters in JSON.");
                return false;
            }

            document.isValid = true;
            return true;
        }
    };

    inline void jsonArkAppendEscaped(String& output, StringSlice value)
    {
        output.append('"');
        for (usize i = 0; i < value.getLength(); ++i)
        {
            char const c = value.asPointer()[i];
            switch (c)
            {
                case '"':
                    output += "\\\"";
                    break;
                case '\\':
                    output += "\\\\";
                    break;
                case '\b':
                    output += "\\b";
                    break;
                case '\f':
                    output += "\\f";
                    break;
                case '\n':
                    output += "\\n";
                    break;
                case '\r':
                    output += "\\r";
                    break;
                case '\t':
                    output += "\\t";
                    break;
                default:
                    if (static_cast<unsigned char>(c) < 0x20)
                    {
                        output += "\\u00";
                        static constexpr char Hex[] = "0123456789ABCDEF";
                        auto const uc = static_cast<unsigned char>(c);
                        output.append(Hex[(uc >> 4) & 0xF]);
                        output.append(Hex[uc & 0xF]);
                    }
                    else
                    {
                        output.append(c);
                    }
                    break;
            }
        }
        output.append('"');
    }
}
