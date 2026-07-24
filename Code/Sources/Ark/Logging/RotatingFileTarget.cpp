#include "Ark/Logging/RotatingFileTarget.hpp"
#include "Ark/Collections/Array.hpp"
#include "Ark/Core/Debug/Assert.hpp"
#include "Ark/Logging/FormatUtilities.hpp"
#include "Ark/Storage/FileSystem.hpp"
#include "Ark/Storage/Types.hpp"
#include "Ark/Strings/AnsiCharacters.hpp"
#include "Ark/Strings/AnsiConversions.hpp"
#include "Ark/System/Time.hpp"

namespace Ark::Logging
{
    namespace
    {
        StringSlice removeNumericSuffix(StringSlice fileName)
        {
            usize lastDotPos = String::InvalidIndex;
            for (usize i = 0; i < fileName.getLength(); ++i)
            {
                if (fileName[i] == '.')
                {
                    lastDotPos = i;
                }
            }

            if (lastDotPos == String::InvalidIndex)
            {
                return fileName;
            }

            auto suffix = fileName.subslice(lastDotPos + 1);
            for (usize i = 0; i < suffix.getLength(); ++i)
            {
                if (!Characters::isDigit(suffix[i]))
                {
                    return fileName;
                }
            }

            return fileName.subslice(0, lastDotPos);
        }

        uint64 getModificationComparable(Storage::Path const& path)
        {
            auto modificationTime = Storage::FileSystem::getLastModificationTime(path);
            if (!modificationTime.isOk())
            {
                return 0;
            }

            return Internal::dateTimeToComparable(modificationTime.getValue());
        }
    }

    RotatingFileTarget::RotatingFileTarget(Storage::Path path, usize maxSize, uint32 maxCount)
        : baseFilePath{Ark::move(path)}
        , maxFileSize{maxSize}
        , maxFiles{maxCount}
    {
        openNewFile();
    }

    void RotatingFileTarget::write(const Entry& entry)
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

        auto size = fileStream.getSize();
        if (size.isOk() && size.getValue() >= maxFileSize)
        {
            rotateFiles();
        }
    }

    void RotatingFileTarget::flush()
    {
        if (fileStream.isOpen())
        {
            fileStream.flush();
        }
    }

    void RotatingFileTarget::openNewFile()
    {
        uint64 const nowMs = System::Time::getCurrentTimeMilliseconds().getValueOr(0);
        String fileName = baseFilePath.stem();
        fileName.append('_');
        fileName.append(Internal::formatCompactDateTime(nowMs));

        String const extension = baseFilePath.extension();
        if (!extension.isEmpty())
        {
            fileName.append('.');
            fileName.append(extension);
        }

        Storage::Path directory = baseFilePath.parent();
        if (directory.isEmpty())
        {
            directory = Storage::Path(".");
        }

        Storage::Path const newPath = directory / fileName;
        Storage::FileDescriptor descriptor{Storage::FileAccess::Write, Storage::FileShare::Read};
        Outcome const opened = fileStream.open(newPath, Storage::FileMode::OpenAlways, descriptor);
        ARK_ASSERT_MSG(opened.isOk() && fileStream.isOpen(), "RotatingFileTarget failed to open log file");

        if (fileStream.isOpen())
        {
            fileStream.setPosition(0, Storage::SeekOrigin::End);
        }
    }

    void RotatingFileTarget::rotateFiles()
    {
        fileStream.close();

        Storage::Path directory = baseFilePath.parent();
        if (directory.isEmpty())
        {
            directory = Storage::Path(".");
        }

        String const filePrefix = baseFilePath.stem();
        String const extension = baseFilePath.extension();

        Collections::Array<Storage::Path> logFiles;
        Storage::FileSystem::enumerateDirectory(directory, [&](Storage::Path const& relative, bool isFile)
                                                {
                                                    if (!isFile)
                                                    {
                                                        return true;
                                                    }

                                                    Storage::Path const fullPath = directory / relative;
                                                    String const stem = fullPath.stem();
                                                    if (stem.startsWith(filePrefix) && fullPath.extension() == extension)
                                                    {
                                                        logFiles.append(fullPath);
                                                    }
                                                    return true;
                                                });

        for (usize i = 0; i < logFiles.getCount(); ++i)
        {
            for (usize j = i + 1; j < logFiles.getCount(); ++j)
            {
                if (getModificationComparable(logFiles[j]) < getModificationComparable(logFiles[i]))
                {
                    auto tmp = logFiles[i];
                    logFiles[i] = logFiles[j];
                    logFiles[j] = tmp;
                }
            }
        }

        if (maxFiles != 0)
        {
            while (logFiles.getCount() > static_cast<usize>(maxFiles))
            {
                Storage::FileSystem::deleteFile(logFiles.getFirst());
                logFiles.removeFirst();
            }
        }

        auto appendExtension = [&](String& name)
        {
            if (!extension.isEmpty())
            {
                name.append('.');
                name.append(extension);
            }
        };

        for (int i = static_cast<int>(logFiles.getCount()) - 1; i > 0; --i)
        {
            String newName = String(removeNumericSuffix(logFiles[i].stem()));
            newName.append('.');
            newName.append(Ark::Strings::fromInt(i + 1).getValueOr("0"));
            appendExtension(newName);
            Storage::FileSystem::renameFile(logFiles[i], logFiles[i].parent() / newName);
        }

        if (!logFiles.isEmpty())
        {
            String newName = String(removeNumericSuffix(logFiles[0].stem()));
            newName.append(".1");
            appendExtension(newName);
            Storage::FileSystem::renameFile(logFiles[0], logFiles[0].parent() / newName);
        }

        openNewFile();
    }
}
