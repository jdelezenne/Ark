#include "Ark/Documents/CsvWriter.hpp"

namespace Ark::Documents
{
    CsvWriter::CsvWriter(Storage::Stream& stream)
        : stream{stream}
        , options{}
    {
    }

    CsvWriter::CsvWriter(Storage::Stream& stream, Options const& options)
        : stream{stream}
        , options{options}
    {
    }

    CsvWriter::~CsvWriter() = default;

    Outcome CsvWriter::write(CsvDocument const& document)
    {
        if (document.isEmpty())
        {
            return makeOutcome();
        }

        Collections::Array<CsvDocument::Row> const& allRows = document.getRows();

        for (usize i = 0; i < allRows.getCount(); ++i)
        {
            writeRow(allRows[i]);

            if (i < allRows.getCount() - 1)
            {
                writeLineEnding();
            }
        }

        return makeOutcome();
    }

    Storage::Stream& CsvWriter::getStream() const
    {
        return stream;
    }

    CsvWriter::Options const& CsvWriter::getOptions() const
    {
        return options;
    }

    Outcome CsvWriter::writeString(StringSlice str)
    {
        if (str.isEmpty())
        {
            return makeOutcome();
        }

        stream.write(str.asPointer(), str.getLength());
        return makeOutcome();
    }

    Outcome CsvWriter::writeLineEnding()
    {
        if (options.lineEnding == Options::LineEnding::Windows)
        {
            return writeString("\r\n");
        }

        return writeString("\n");
    }

    Outcome CsvWriter::writeField(StringSlice field)
    {
        if (options.alwaysQuote || needsQuoting(field))
        {
            char const quote = options.quoteChar;
            writeString(StringSlice{&quote, 1});

            usize start = 0;
            for (usize i = 0; i < field.getLength(); ++i)
            {
                if (field[i] == options.quoteChar)
                {
                    writeString(field.subslice(start, i + 1));
                    writeString(StringSlice{&quote, 1});
                    start = i + 1;
                }
            }

            if (start < field.getLength())
            {
                writeString(field.subslice(start));
            }

            writeString(StringSlice{&quote, 1});
        }
        else
        {
            writeString(field);
        }

        return makeOutcome();
    }

    Outcome CsvWriter::writeRow(CsvDocument::Row const& row)
    {
        for (usize i = 0; i < row.getCount(); ++i)
        {
            writeField(row[i]);

            if (i < row.getCount() - 1)
            {
                char const delim = options.delimiter;
                writeString(StringSlice{&delim, 1});
            }
        }

        return makeOutcome();
    }

    bool CsvWriter::needsQuoting(StringSlice field) const
    {
        for (usize i = 0; i < field.getLength(); ++i)
        {
            char const c = field[i];

            if (c == options.delimiter || c == options.quoteChar || c == '\n' || c == '\r')
            {
                return true;
            }
        }

        return false;
    }
}
