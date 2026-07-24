#include "Ark/Documents/CsvReader.hpp"

namespace Ark::Documents
{
    namespace
    {
        static bool isWs(char c)
        {
            return c == ' ' || c == '\t' || c == '\r' || c == '\n';
        }

        static StringSlice trimWs(StringSlice str)
        {
            if (str.isEmpty())
            {
                return StringSlice{};
            }

            usize start = 0;
            while (start < str.getLength() && isWs(str[start]))
            {
                ++start;
            }

            if (start >= str.getLength())
            {
                return StringSlice{};
            }

            usize end = str.getLength();
            while (end > start && isWs(str[end - 1]))
            {
                --end;
            }

            return str.subslice(start, end);
        }
    }

    CsvReader::CsvReader(Storage::Stream& stream)
        : stream{stream}
        , options{}
    {
    }

    CsvReader::CsvReader(Storage::Stream& stream, Options const& options)
        : stream{stream}
        , options{options}
    {
    }

    CsvReader::~CsvReader() = default;

    Result<CsvDocument, CsvReader::Error> CsvReader::parse()
    {
        CsvDocument document{options.hasHeaders};

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

        Collections::Array<char> buffer;
        buffer.resize(size);

        auto const readResult = stream.read(buffer.asMutablePointer(), size);
        if (readResult.isError())
        {
            return Ark::Error{Error::StreamError};
        }

        String const content{buffer.asPointer(), size};
        usize position = 0;

        while (position < content.getLength())
        {
            CsvDocument::Row row;
            bool isEmptyRow = true;

            while (position < content.getLength())
            {
                String field;
                bool inQuotes = false;
                bool fieldComplete = false;
                usize fieldStart = position;

                if (position < content.getLength() && content[position] == options.quoteChar)
                {
                    inQuotes = true;
                    ++position;
                    fieldStart = position;
                }

                while (position < content.getLength())
                {
                    char const c = content[position];

                    if (inQuotes)
                    {
                        if (c == options.escapeChar && position + 1 < content.getLength() && content[position + 1] == options.quoteChar)
                        {
                            field.append(StringSlice{content.asPointer() + fieldStart, position - fieldStart});
                            field.append(options.quoteChar);
                            position += 2;
                            fieldStart = position;
                            continue;
                        }

                        if (c == options.quoteChar)
                        {
                            field.append(StringSlice{content.asPointer() + fieldStart, position - fieldStart});
                            inQuotes = false;
                            fieldComplete = true;
                            ++position;

                            if (position < content.getLength() && content[position] == options.delimiter)
                            {
                                ++position;
                                break;
                            }

                            if (position < content.getLength() && (content[position] == '\r' || content[position] == '\n'))
                            {
                                if (content[position] == '\r')
                                {
                                    ++position;
                                }

                                if (position < content.getLength() && content[position] == '\n')
                                {
                                    ++position;
                                }

                                break;
                            }

                            break;
                        }

                        ++position;
                    }
                    else
                    {
                        if (c == options.delimiter)
                        {
                            StringSlice fieldSlice{content.asPointer() + fieldStart, position - fieldStart};
                            if (options.trimWhitespace)
                            {
                                fieldSlice = trimWhitespace(fieldSlice);
                            }

                            field = String{fieldSlice};
                            fieldComplete = true;
                            ++position;
                            break;
                        }

                        if (c == '\n' || c == '\r')
                        {
                            StringSlice fieldSlice{content.asPointer() + fieldStart, position - fieldStart};
                            if (options.trimWhitespace)
                            {
                                fieldSlice = trimWhitespace(fieldSlice);
                            }

                            field = String{fieldSlice};
                            fieldComplete = true;

                            if (c == '\r')
                            {
                                ++position;
                            }

                            if (position < content.getLength() && content[position] == '\n')
                            {
                                ++position;
                            }

                            break;
                        }

                        ++position;
                    }
                }

                if (inQuotes)
                {
                    return Ark::Error{Error::UnclosedQuote};
                }

                if (!fieldComplete && position > fieldStart)
                {
                    StringSlice fieldSlice{content.asPointer() + fieldStart, position - fieldStart};
                    if (options.trimWhitespace)
                    {
                        fieldSlice = trimWhitespace(fieldSlice);
                    }

                    field = String{fieldSlice};
                }

                if (!field.isEmpty())
                {
                    isEmptyRow = false;
                }

                row.append(field);

                if (position >= content.getLength())
                {
                    break;
                }

                char const current = position > 0 ? content[position - 1] : '\0';
                if (current == '\n' || current == '\r')
                {
                    break;
                }
            }

            if (!isEmptyRow || options.allowEmptyRows)
            {
                document.appendRow(row);
            }
        }

        return document;
    }

    Storage::Stream& CsvReader::getStream() const
    {
        return stream;
    }

    CsvReader::Options const& CsvReader::getOptions() const
    {
        return options;
    }

    StringSlice CsvReader::trimWhitespace(StringSlice str)
    {
        if (str.isEmpty())
        {
            return StringSlice{};
        }

        usize start = 0;
        while (start < str.getLength() && isWhitespace(str[start]))
        {
            ++start;
        }

        if (start >= str.getLength())
        {
            return StringSlice{};
        }

        usize end = str.getLength();
        while (end > start && isWhitespace(str[end - 1]))
        {
            --end;
        }

        return str.subslice(start, end);
    }

    bool CsvReader::isWhitespace(char c)
    {
        return c == ' ' || c == '\t' || c == '\r' || c == '\n';
    }
}
