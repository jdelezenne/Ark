#pragma once

#include "Ark/Logging/Interfaces.hpp"
#include "Ark/System/Console.hpp"

namespace Ark::Logging
{
    using namespace Ark::System;

    /// Console output logging target.
    /// Writes formatted log entries to stdout via System::Console.
    class ConsoleTarget final : public Target
    {
        /// Writes a log entry to console (stdout).
        /// @param entry Log entry with formatted message.
        void write(const Entry& entry) override
        {
            Console::println(entry.formattedMessage);
        }

        /// Flushes output buffer (no-op for console).
        void flush() override
        {
        }
    };
}
