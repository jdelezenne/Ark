#include "Ark/Documents/IniReader.hpp"
#include "Ark/Strings/Cp437.hpp"

namespace Ark::Documents
{
    namespace
    {
        // Removes an inline comment starting with the configured comment characters.
        // Honors simple quoted strings (single or float64 quotes) so comment markers inside quotes are preserved.
        static String stripInlineComment(String const& input, IniReader::Options const& options)
        {
            if (input.isEmpty())
            {
                return input;
            }

            bool inQuotes = false;
            char quoteChar = '\0';

            for (usize i = 0; i < input.getLength(); ++i)
            {
                char const c = input[i];

                if (c == '"' || c == '\'')
                {
                    if (!inQuotes)
                    {
                        inQuotes = true;
                        quoteChar = c;
                    }
                    else if (quoteChar == c)
                    {
                        inQuotes = false;
                    }
                    continue;
                }

                if (!inQuotes && (c == options.commentChar || c == options.altCommentChar))
                {
                    StringSlice const head{input.asPointer(), i};
                    return String{head.trimmed()};
                }
            }

            return input;
        }
    }

    IniReader::IniReader(Storage::Stream& stream)
        : stream{stream}
        , options{}
    {
    }

    IniReader::IniReader(Storage::Stream& stream, Options const& options)
        : stream{stream}
        , options{options}
    {
    }

    IniReader::~IniReader() = default;

    Result<IniDocument, IniReader::Error> IniReader::parse()
    {
        IniDocument document;

        auto const sizeResult = stream.getSize();
        if (sizeResult.isError())
        {
            return Ark::Error{Error::StreamError};
        }

        usize const size = sizeResult.getValue();
        if (size == 0)
        {
            return document;
        }

        Collections::Array<uint8> buffer;
        buffer.resize(size);

        auto const readResult = stream.read(buffer.asMutablePointer(), size);
        if (readResult.isError())
        {
            return Ark::Error{Error::StreamError};
        }

        String content;
        if (options.encoding == IniEncoding::Cp437)
        {
            content = Strings::convertCp437ToUtf8(buffer);
        }
        else
        {
            StringSlice slice{reinterpret_cast<char const*>(buffer.asPointer()), buffer.getCount()};
            content = String{slice};
        }

        Collections::Array<StringSlice> lines;

        usize lineStart = 0;
        for (usize i = 0; i < content.getLength(); ++i)
        {
            if (content[i] == '\n')
            {
                StringSlice line{content.asPointer() + lineStart, i - lineStart};

                if (!line.isEmpty() && line[line.getLength() - 1] == '\r')
                {
                    line = StringSlice{line.asPointer(), line.getLength() - 1};
                }

                lines.append(line);
                lineStart = i + 1;
            }
        }

        if (lineStart < content.getLength())
        {
            lines.append(StringSlice{content.asPointer() + lineStart, content.getLength() - lineStart});
        }

        String currentSection;
        String currentComment;

        for (StringSlice line : lines)
        {
            if (options.trimWhitespace)
            {
                line = line.trimmed();
            }

            if (line.isEmpty())
            {
                continue;
            }

            if (!line.isEmpty() && isComment(line[0], options))
            {
                if (options.preserveComments)
                {
                    StringSlice comment = line.subslice(1);
                    if (options.trimWhitespace)
                    {
                        comment = comment.trimmed();
                    }

                    if (!currentComment.isEmpty())
                    {
                        currentComment.append('\n');
                    }

                    currentComment.append(comment);
                }

                continue;
            }

            if (line[0] == '[')
            {
                auto const endPosOpt = findFirst(line, "]");
                if (!endPosOpt.hasValue())
                {
                    return Ark::Error{Error::InvalidSectionHeader};
                }

                usize const endPos = endPosOpt.getValue();

                String sectionName;
                if (options.trimWhitespace)
                {
                    sectionName = String{line.subslice(1, endPos).trimmed()};
                }
                else
                {
                    sectionName = String{line.subslice(1, endPos)};
                }

                currentSection = sectionName;

                if (!currentComment.isEmpty())
                {
                    document.setComment(currentSection, currentComment);
                    currentComment.clear();
                }

                continue;
            }

            auto const equalPosOpt = findFirst(line, "=");
            if (!equalPosOpt.hasValue())
            {
                continue;
            }

            usize const equalPos = equalPosOpt.getValue();

            String key;
            String value;

            if (options.trimWhitespace)
            {
                key = String{line.subslice(0, equalPos).trimmed()};
                value = String{line.subslice(equalPos + 1).trimmed()};
            }
            else
            {
                key = String{line.subslice(0, equalPos)};
                value = String{line.subslice(equalPos + 1)};
            }

            // Remove inline comments from the value if present
            value = stripInlineComment(value, options);

            if (key.isEmpty())
            {
                continue;
            }

            if (!options.allowDuplicateKeys && document.hasKey(currentSection, key))
            {
                continue;
            }

            document.setValue(currentSection, key, value);
        }

        return document;
    }

    Storage::Stream& IniReader::getStream() const
    {
        return stream;
    }

    IniReader::Options const& IniReader::getOptions() const
    {
        return options;
    }

    bool IniReader::isComment(char c, Options const& options)
    {
        return c == options.commentChar || c == options.altCommentChar;
    }
}
