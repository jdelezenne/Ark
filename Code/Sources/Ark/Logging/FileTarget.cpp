#include "Ark/Logging/FileTarget.hpp"
#include "Ark/Core/Debug/Assert.hpp"
#include "Ark/Storage/Types.hpp"

namespace Ark::Logging
{
    FileTarget::FileTarget(Storage::Path path, bool appendModeValue)
        : filePath{Ark::move(path)}
        , appendMode{appendModeValue}
    {
        openFile();
    }

    void FileTarget::write(const Entry& entry)
    {
        if (!fileStream.isOpen())
        {
            return;
        }

        StringSlice text = entry.formattedMessage;
        auto written = fileStream.write(text.asPointer(), text.getLength());
        ARK_ASSERT(written.isOk());

        char const newline = '\n';
        written = fileStream.write(&newline, 1);
        ARK_ASSERT(written.isOk());
    }

    void FileTarget::flush()
    {
        if (fileStream.isOpen())
        {
            fileStream.flush();
        }
    }

    void FileTarget::openFile()
    {
        Storage::FileMode const mode = appendMode ? Storage::FileMode::OpenAlways : Storage::FileMode::CreateAlways;
        Storage::FileDescriptor descriptor{Storage::FileAccess::Write, Storage::FileShare::Read};
        Outcome const opened = fileStream.open(filePath, mode, descriptor);
        ARK_ASSERT_MSG(opened.isOk() && fileStream.isOpen(), "FileTarget failed to open log file");

        if (appendMode && fileStream.isOpen())
        {
            fileStream.setPosition(0, Storage::SeekOrigin::End);
        }
    }
}
