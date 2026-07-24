#pragma once

#include "Ark/Documents/CsvDocument.hpp"
#include "Ark/Storage/Stream.hpp"

namespace Ark::Documents
{
    /// Writer for CSV format documents
    /// @details Writes a CsvDocument to a stream in standard CSV format.
    class CsvWriter final
    {
    public:
        /// Error types for CSV writing
        enum class Error
        {
            StreamError,
            InvalidDocument,
        };

        /// Configuration options for CSV writing
        struct Options final
        {
            /// Field delimiter character (default: ',')
            char delimiter = ',';

            /// Quote character for fields containing delimiters or newlines (default: '"')
            char quoteChar = '"';

            /// Whether to always quote fields (default: false, only quote when necessary)
            bool alwaysQuote = false;

            /// Line ending style
            enum class LineEnding
            {
                Unix,
                Windows,
            };

            /// Line ending to use
            LineEnding lineEnding = LineEnding::Unix;
        };

    private:
        Storage::Stream& stream;
        Options options;

    public:
        /// Constructs a CSV writer with default options
        /// @param stream The output stream to write to
        explicit CsvWriter(Storage::Stream& stream);

        /// Constructs a CSV writer with custom options
        /// @param stream The output stream to write to
        /// @param options Writer configuration options
        CsvWriter(Storage::Stream& stream, Options const& options);

        ~CsvWriter();

        /// Writes the document to the stream
        /// @param document The CSV document to write
        Outcome write(CsvDocument const& document);

        /// Gets the output stream
        Storage::Stream& getStream() const;

        /// Gets the writer options
        Options const& getOptions() const;

    private:
        Outcome writeString(StringSlice str);

        Outcome writeLineEnding();

        Outcome writeField(StringSlice field);

        Outcome writeRow(CsvDocument::Row const& row);

        bool needsQuoting(StringSlice field) const;
    };
}
