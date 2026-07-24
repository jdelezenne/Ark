#include "Ark/Documents/IniWriter.hpp"

namespace Ark::Documents
{
    IniWriter::IniWriter(Storage::Stream& stream)
        : stream{stream}
        , options{}
    {
    }

    IniWriter::IniWriter(Storage::Stream& stream, Options const& options)
        : stream{stream}
        , options{options}
    {
    }

    IniWriter::~IniWriter() = default;

    Outcome IniWriter::write(IniDocument const& document)
    {
        if (document.isEmpty())
        {
            return makeOutcome();
        }

        auto const globalSection = document.getSection("");
        if (globalSection != nullptr)
        {
            writeSection("", *globalSection);

            if (document.getSectionCount() > 0 && options.blankLinesBetweenSections)
            {
                writeLineEnding();
            }
        }

        Collections::Array<String> const sectionNames = document.getSectionNames();
        for (usize i = 0; i < sectionNames.getCount(); ++i)
        {
            String const& sectionName = sectionNames[i];

            if (options.writeComments)
            {
                auto const comment = document.getComment(sectionName);
                if (comment.hasValue())
                {
                    writeComment(comment.getValue());
                }
            }

            writeString("[");
            writeString(sectionName);
            writeString("]");
            writeLineEnding();

            auto const section = document.getSection(sectionName);
            if (section != nullptr)
            {
                writeSection(sectionName, *section);
            }

            if (i < sectionNames.getCount() - 1 && options.blankLinesBetweenSections)
            {
                writeLineEnding();
            }
        }

        return makeOutcome();
    }

    Storage::Stream& IniWriter::getStream() const
    {
        return stream;
    }

    IniWriter::Options const& IniWriter::getOptions() const
    {
        return options;
    }

    Outcome IniWriter::writeString(StringSlice str)
    {
        if (str.isEmpty())
        {
            return makeOutcome();
        }

        stream.write(str.asPointer(), str.getLength());
        return makeOutcome();
    }

    Outcome IniWriter::writeLineEnding()
    {
        if (options.lineEnding == Options::LineEnding::Windows)
        {
            return writeString("\r\n");
        }

        return writeString("\n");
    }

    Outcome IniWriter::writeComment(StringSlice comment)
    {
        if (comment.isEmpty())
        {
            return makeOutcome();
        }

        usize lineStart = 0;
        for (usize i = 0; i <= comment.getLength(); ++i)
        {
            if (i == comment.getLength() || comment[i] == '\n')
            {
                char const commentChars[2] = {options.commentChar, ' '};
                writeString(StringSlice{commentChars, 2});

                StringSlice const line = comment.subslice(lineStart, i);
                writeString(line);

                writeLineEnding();

                lineStart = i + 1;
            }
        }

        return makeOutcome();
    }

    Outcome IniWriter::writeSection(StringSlice sectionName, IniDocument::PropertyMap const& properties)
    {
        [[maybe_unused]] StringSlice const section = sectionName;

        for (auto const& [key, value] : properties)
        {
            writeString(key);
            writeString("=");
            writeString(value);
            writeLineEnding();
        }

        return makeOutcome();
    }
}
