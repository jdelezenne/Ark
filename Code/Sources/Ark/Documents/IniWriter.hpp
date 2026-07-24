#pragma once

#include "Ark/Documents/IniDocument.hpp"
#include "Ark/Storage/Stream.hpp"

namespace Ark::Documents
{
    /// Writer for INI format documents
    /// @details Writes an IniDocument to a stream in standard INI format.
    class IniWriter final
    {
    public:
        /// Error types for INI writing
        enum class Error
        {
            StreamError,
            InvalidDocument,
        };

        /// Configuration options for INI writing
        struct Options final
        {
            /// Whether to write comments
            bool writeComments = true;

            /// Whether to add blank lines between sections
            bool blankLinesBetweenSections = true;

            /// Comment character (default: ';')
            char commentChar = ';';

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
        /// Constructs an INI writer with default options
        /// @param stream The output stream to write to
        explicit IniWriter(Storage::Stream& stream);

        /// Constructs an INI writer with custom options
        /// @param stream The output stream to write to
        /// @param options Writer configuration options
        IniWriter(Storage::Stream& stream, Options const& options);

        ~IniWriter();

        /// Writes the document to the stream
        /// @param document The INI document to write
        Outcome write(IniDocument const& document);

        /// Gets the output stream
        Storage::Stream& getStream() const;

        /// Gets the writer options
        Options const& getOptions() const;

    private:
        Outcome writeString(StringSlice str);

        Outcome writeLineEnding();

        Outcome writeComment(StringSlice comment);

        Outcome writeSection(StringSlice sectionName, IniDocument::PropertyMap const& properties);
    };
}
