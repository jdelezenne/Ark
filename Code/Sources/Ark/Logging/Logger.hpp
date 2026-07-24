#pragma once

#include "Ark/Collections/Array.hpp"
#include "Ark/Core/Types.hpp"
#include "Ark/Core/Utilities.hpp"
#include "Ark/Logging/Interfaces.hpp"
#include "Ark/Logging/Message.hpp"
#include "Ark/Memory/SharedPointer.hpp"
#include "Ark/Strings/Format.hpp"

namespace Ark::Logging
{
    /// Main logger class.
    class Logger final
    {
        ARK_CLASS(Logger)

    private:
        Collections::Array<SharedPointer<Filter>> filters;
        SharedPointer<Formatter> formatter;
        Collections::Array<SharedPointer<Target>> targets;

    public:
        Logger() = default;

        /// Gets the filters.
        Collections::Array<SharedPointer<Filter>> const& getFilters() const;

        /// Gets the formatter.
        SharedPointer<Formatter> getFormatter() const;

        /// Gets the targets.
        Collections::Array<SharedPointer<Target>> const& getTargets() const;

        /// Adds a filter to the logger.
        /// @param filter The filter to add.
        void addFilter(SharedPointer<Filter> filter);

        /// Sets the formatter for the logger.
        /// @param formatter The formatter to use.
        void setFormatter(SharedPointer<Formatter> formatter);

        /// Adds an output target to the logger.
        /// @param target The target to add.
        void addTarget(SharedPointer<Target> target);

        /// Logs a message.
        /// @param message The log message to process.
        void log(Message const& message);

        void log(Level level, StringSlice message);

        void log(
            Level level,
            StringSlice category,
            StringSlice message,
            uint64 timestamp = 0,
            uint64 threadId = 0,
            SourceLocation location = {});

        template <typename... Args>
        void logFormat(
            Level level,
            StringSlice category,
            uint64 timestamp,
            uint64 threadId,
            SourceLocation location,
            FormatString<Args...> format,
            Args&&... args)
        {
            log(Message{
                level,
                category,
                Ark::format(format, Ark::forward<Args>(args)...),
                timestamp,
                threadId,
                location,
            });
        }

        template <typename... Args>
        void logFormatMessage(
            Level level,
            FormatString<Args...> format,
            Args&&... args)
        {
            Message message{};
            message.level = level;
            message.message = Ark::format(format, Ark::forward<Args>(args)...);
            log(message);
        }
    };
}
