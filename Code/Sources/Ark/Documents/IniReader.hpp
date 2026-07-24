#pragma once

#include "Ark/Documents/IniDocument.hpp"
#include "Ark/Storage/Stream.hpp"

namespace Ark::Documents
{
    /// Source encoding for INI file parsing
    enum class IniEncoding
    {
        Utf8,
        Cp437,
    };

    /// Reader for INI format documents
    /// @details Parses INI files from a stream into an IniDocument.
    /// Supports sections, comments, and standard INI syntax.
    class IniReader final
    {
    public:
        /// Error types for INI parsing
        enum class Error
        {
            InvalidFormat,
            UnexpectedEndOfFile,
            InvalidSectionHeader,
            InvalidKeyValuePair,
            StreamError,
        };

        /// Configuration options for INI parsing
        struct Options final
        {
            /// Whether to preserve comments in the document
            bool preserveComments = true;

            /// Whether to trim whitespace from keys and values
            bool trimWhitespace = true;

            /// Whether to allow duplicate keys (last value wins)
            bool allowDuplicateKeys = true;

            /// Comment character (default: ';')
            char commentChar = ';';

            /// Alternative comment character (default: '#')
            char altCommentChar = '#';

            /// Source encoding of the INI file (default: UTF-8)
            IniEncoding encoding = IniEncoding::Utf8;
        };

    private:
        Storage::Stream& stream;
        Options options;

    public:
        /// Constructs an INI reader with default options
        /// @param stream The input stream to read from
        explicit IniReader(Storage::Stream& stream);

        /// Constructs an INI reader with custom options
        /// @param stream The input stream to read from
        /// @param options Parser configuration options
        IniReader(Storage::Stream& stream, Options const& options);

        ~IniReader();

        /// Parses the INI file into a document
        Result<IniDocument, Error> parse();

        /// Gets the input stream
        Storage::Stream& getStream() const;

        /// Gets the parser options
        Options const& getOptions() const;

    private:
        static bool isComment(char c, Options const& options);
    };
}
