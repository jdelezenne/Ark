#pragma once

#include "Ark/Logging/Interfaces.hpp"
#include "Ark/Logging/Message.hpp"
#include "Ark/Storage/FileStream.hpp"
#include "Ark/Storage/Path.hpp"

namespace Ark::Logging
{
    /// File logging target.
    /// Writes log entries to a specified file.
    class FileTarget final : public Target
    {
    private:
        Storage::Path filePath;
        Storage::FileStream fileStream;
        bool appendMode;

    public:
        /// Constructs a file target.
        /// @param path File path for log output.
        /// @param appendMode If true, append to file; if false, overwrite on open (default: false).
        explicit FileTarget(Storage::Path path, bool appendMode = false);

        /// Writes a log entry to the file.
        /// @param entry Log entry to write.
        void write(const Entry& entry) override;

        /// Flushes the file stream to disk.
        void flush() override;

    private:
        void openFile();
    };
}
