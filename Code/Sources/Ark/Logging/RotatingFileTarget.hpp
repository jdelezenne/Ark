#pragma once

#include "Ark/Logging/Interfaces.hpp"
#include "Ark/Logging/Message.hpp"
#include "Ark/Storage/FileStream.hpp"
#include "Ark/Storage/Path.hpp"

namespace Ark::Logging
{
    /// Rotating file logging target.
    /// Writes log entries to a file and rotates (archives) the file when it exceeds a size limit.
    /// Older rotated files are deleted when the maximum file count is exceeded.
    class RotatingFileTarget final : public Target
    {
    private:
        Storage::Path baseFilePath;
        Storage::FileStream fileStream;
        usize maxFileSize;
        uint32 maxFiles;

    public:
        /// Constructs a rotating file target.
        /// @param path Base file path (e.g., "logs/app.log").
        /// @param maxSize Maximum file size before rotation (in bytes).
        /// @param maxCount Maximum number of rotated files to retain (0 = unlimited).
        RotatingFileTarget(Storage::Path path, usize maxSize, uint32 maxCount);

        /// Writes a log entry to the current file.
        /// If the file size exceeds maxFileSize, performs rotation.
        /// @param entry Log entry to write.
        void write(const Entry& entry) override;

        /// Flushes the current file stream.
        void flush() override;

    private:
        void openNewFile();
        void rotateFiles();
    };
}
