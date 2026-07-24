#pragma once

#include "Ark/Collections/Array.hpp"
#include "Ark/Core/Pair.hpp"
#include "Ark/Memory/UniquePointer.hpp"
#include "Ark/Storage/File.hpp"
#include "Ark/Storage/Path.hpp"
#include "Ark/Strings/AnsiCharacters.hpp"
#include "Ark/Strings/AnsiStrings.hpp"
#include "Ark/Strings/String.hpp"
#include "Ark/Strings/StringSlice.hpp"

namespace Ark::Documents::Details
{
    struct XmlArkElement final
    {
        String name;
        String text;
        bool hasText = false;
        XmlArkElement* parent = nullptr;
        XmlArkElement* nextSibling = nullptr;
        Collections::Array<Pair<String, String>> attributes;
        Collections::Array<UniquePointer<XmlArkElement>> children;
    };

    struct XmlArkDocument final
    {
        UniquePointer<XmlArkElement> root;
        String error;
    };

    inline bool xmlArkEquals(char const* lhs, String const& rhs)
    {
        if (lhs == nullptr)
        {
            return false;
        }
        usize const lhsLength = AnsiStrings::getLengthUnsafe(lhs);
        return StringSlice(lhs, lhsLength).isEqual(rhs.toSlice());
    }

    inline void xmlArkDecodeEntity(String& output, StringSlice entity)
    {
        if (entity.isEqual(StringSlice("lt")))
        {
            output.append('<');
        }
        else if (entity.isEqual(StringSlice("gt")))
        {
            output.append('>');
        }
        else if (entity.isEqual(StringSlice("amp")))
        {
            output.append('&');
        }
        else if (entity.isEqual(StringSlice("quot")))
        {
            output.append('"');
        }
        else if (entity.isEqual(StringSlice("apos")))
        {
            output.append('\'');
        }
    }

    class XmlArkParser final
    {
    private:
        char const* data = nullptr;
        usize length = 0;
        usize cursor = 0;

    private:
        char peek() const
        {
            return cursor < length ? data[cursor] : '\0';
        }

        bool startsWith(char const* token) const
        {
            usize const tokenLength = AnsiStrings::getLengthUnsafe(token);
            if (cursor + tokenLength > length)
            {
                return false;
            }
            return StringSlice(data + cursor, tokenLength).isEqual(StringSlice(token, tokenLength));
        }

        void skipWhitespace()
        {
            while (cursor < length && Characters::isWhitespace(data[cursor]))
            {
                ++cursor;
            }
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

        bool skipUntil(char const* token)
        {
            usize const tokenLength = AnsiStrings::getLengthUnsafe(token);
            while (cursor + tokenLength <= length)
            {
                if (StringSlice(data + cursor, tokenLength).isEqual(StringSlice(token, tokenLength)))
                {
                    cursor += tokenLength;
                    return true;
                }
                ++cursor;
            }
            return false;
        }

        bool parseName(String& output)
        {
            if (!(Characters::isAlphabetic(peek()) || peek() == '_' || peek() == ':'))
            {
                return false;
            }

            usize const begin = cursor;
            while (cursor < length)
            {
                char const c = data[cursor];
                if (Characters::isAlphanumeric(c) || c == '_' || c == '-' || c == ':' || c == '.')
                {
                    ++cursor;
                }
                else
                {
                    break;
                }
            }
            output.assign(data + begin, cursor - begin);
            return !output.isEmpty();
        }

        bool parseQuotedValue(String& output)
        {
            char const quote = peek();
            if (quote != '"' && quote != '\'')
            {
                return false;
            }
            ++cursor;

            output.clear();
            while (cursor < length)
            {
                char const c = data[cursor++];
                if (c == quote)
                {
                    return true;
                }
                if (c == '&')
                {
                    usize const entityStart = cursor;
                    while (cursor < length && data[cursor] != ';')
                    {
                        ++cursor;
                    }
                    if (cursor >= length)
                    {
                        return false;
                    }
                    StringSlice const entity(data + entityStart, cursor - entityStart);
                    ++cursor;
                    xmlArkDecodeEntity(output, entity);
                    continue;
                }
                output.append(c);
            }
            return false;
        }

        bool parseElement(UniquePointer<XmlArkElement>& element, XmlArkElement* parent)
        {
            if (!consume('<'))
            {
                return false;
            }
            if (startsWith("!--"))
            {
                cursor += 3;
                if (!skipUntil("-->"))
                {
                    return false;
                }
                skipWhitespace();
                return parseElement(element, parent);
            }

            String name;
            if (!parseName(name))
            {
                return false;
            }

            auto current = makeUnique<XmlArkElement>();
            current->name = Ark::move(name);
            current->parent = parent;

            skipWhitespace();
            while (peek() != '>' && !startsWith("/>"))
            {
                String attrName;
                String attrValue;
                if (!parseName(attrName))
                {
                    return false;
                }
                skipWhitespace();
                if (!consume('='))
                {
                    return false;
                }
                skipWhitespace();
                if (!parseQuotedValue(attrValue))
                {
                    return false;
                }
                current->attributes.appendMake(Ark::move(attrName), Ark::move(attrValue));
                skipWhitespace();
            }

            if (startsWith("/>"))
            {
                cursor += 2;
                element = Ark::move(current);
                return true;
            }
            if (!consume('>'))
            {
                return false;
            }

            String text;
            while (cursor < length)
            {
                if (startsWith("</"))
                {
                    cursor += 2;
                    String endName;
                    if (!parseName(endName))
                    {
                        return false;
                    }
                    skipWhitespace();
                    if (!consume('>'))
                    {
                        return false;
                    }
                    if (endName != current->name)
                    {
                        return false;
                    }
                    break;
                }

                if (peek() == '<')
                {
                    UniquePointer<XmlArkElement> child;
                    if (!parseElement(child, current.get()))
                    {
                        return false;
                    }
                    if (child != nullptr)
                    {
                        if (!current->children.isEmpty())
                        {
                            current->children.getLast()->nextSibling = child.get();
                        }
                        current->children.append(Ark::move(child));
                    }
                    continue;
                }

                char const c = data[cursor++];
                if (c == '&')
                {
                    usize const entityStart = cursor;
                    while (cursor < length && data[cursor] != ';')
                    {
                        ++cursor;
                    }
                    if (cursor >= length)
                    {
                        return false;
                    }
                    StringSlice const entity(data + entityStart, cursor - entityStart);
                    ++cursor;
                    xmlArkDecodeEntity(text, entity);
                }
                else
                {
                    text.append(c);
                }
            }

            if (!text.isEmpty())
            {
                current->text = Ark::move(text);
                current->hasText = true;
            }
            element = Ark::move(current);
            return true;
        }

    public:
        XmlArkParser(char const* data, usize length)
            : data(data)
            , length(length)
            , cursor(0)
        {
        }

        bool parse(XmlArkDocument& document)
        {
            document.root.reset();
            document.error = String();

            skipWhitespace();
            if (startsWith("<?xml"))
            {
                if (!skipUntil("?>"))
                {
                    document.error = String("Invalid XML declaration.");
                    return false;
                }
                skipWhitespace();
            }

            if (!parseElement(document.root, nullptr) || !document.root)
            {
                document.error = String("Invalid XML document.");
                return false;
            }

            skipWhitespace();
            if (cursor != length)
            {
                document.error = String("Unexpected trailing characters in XML.");
                return false;
            }
            return true;
        }
    };

    inline bool parseXmlArkDocument(XmlArkDocument& document, char const* data, usize length)
    {
        XmlArkParser parser(data, length);
        return parser.parse(document);
    }

    inline bool loadXmlArkDocument(XmlArkDocument& document, Storage::Path const& path)
    {
        auto textResult = Storage::File::readAllText(path);
        if (!textResult)
        {
            document.error = String("Unable to open XML file.");
            return false;
        }

        String const& text = *textResult;
        return parseXmlArkDocument(document, text.asPointer(), text.getLength());
    }
}
