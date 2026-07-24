#pragma once

#include "Ark/Documents/CsvDocument.hpp"
#include "Ark/Storage/Stream.hpp"

namespace Ark::Documents
{
    /// Reader for CSV format documents
    /// @details Parses CSV files from a stream into a CsvDocument.
    /// Supports quoted fields, custom delimiters, and header rows.
    class CsvReader final
    {
    public:
        /// Error types for CSV parsing
        enum class Error
        {
            InvalidFormat,
            UnexpectedEndOfFile,
            UnclosedQuote,
            InvalidEscape,
            StreamError,
        };

        /// Configuration options for CSV parsing
        struct Options final
        {
            /// Field delimiter character (default: ',')
            char delimiter = ',';

            /// Quote character for fields containing delimiters or newlines (default: '"')
            char quoteChar = '"';

            /// Escape character within quoted fields (default: '"' for float64d quotes)
            char escapeChar = '"';

            /// Whether the first row contains column headers
            bool hasHeaders = false;

            /// Whether to trim whitespace from unquoted fields
            bool trimWhitespace = false;

            /// Whether to allow empty rows (skip them if false)
            bool allowEmptyRows = false;
        };

    private:
        Storage::Stream& stream;
        Options options;

    public:
        /// Constructs a CSV reader with default options
        /// @param stream The input stream to read from
        explicit CsvReader(Storage::Stream& stream);

        /// Constructs a CSV reader with custom options
        /// @param stream The input stream to read from
        /// @param options Parser configuration options
        CsvReader(Storage::Stream& stream, Options const& options);

        ~CsvReader();

        /// Parses the CSV file into a document
        Result<CsvDocument, Error> parse();

        /// Gets the input stream
        Storage::Stream& getStream() const;

        /// Gets the parser options
        Options const& getOptions() const;

    private:
        static StringSlice trimWhitespace(StringSlice str);

        static bool isWhitespace(char c);
    };
}
