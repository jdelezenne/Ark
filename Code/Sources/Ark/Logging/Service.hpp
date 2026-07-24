#pragma once

#include "Ark/Collections/HashMap.hpp"
#include "Ark/Concurrency/Mutex.hpp"
#include "Ark/Concurrency/Thread.hpp"
#include "Ark/Core/Types.hpp"
#include "Ark/Logging/Level.hpp"
#include "Ark/Logging/Message.hpp"
#include "Ark/Memory/SharedPointer.hpp"
#include "Ark/Memory/UniquePointer.hpp"
#include "Ark/Strings/Format.hpp"
#include "Ark/Strings/StringSlice.hpp"
#include "Ark/System/Time.hpp"

namespace Ark::Logging
{
    class Logger;
    class Filter;

    /// Central logging service interface.
    /// Manages logger instances and processes log messages.
    class ServiceInterface
    {
    private:
        /// Global service instance (singleton).
        static UniquePointer<ServiceInterface> instance;

    public:
        /// Gets the global logging service instance.
        /// @return Service instance pointer (may be nullptr if not initialized).
        static ServiceInterface* getInstance()
        {
            return instance.get();
        }

        /// Sets the global logging service instance.
        /// @param value New service instance.
        static void setInstance(UniquePointer<ServiceInterface> value)
        {
            instance = Ark::move(value);
        }

        /// Destroys the service.
        virtual ~ServiceInterface() = default;

        /// Gets an existing logger by name.
        /// @param name Logger name.
        /// @return Shared pointer to logger (nullptr if not found).
        virtual SharedPointer<Logger> getLogger(StringSlice name) const = 0;

        /// Creates and registers a new logger.
        /// @param name Logger name.
        /// @return Pointer to the created logger.
        virtual Logger* addLogger(StringSlice name) = 0;

        /// Removes a logger by name.
        /// @param name Logger name to remove.
        virtual void removeLogger(StringSlice name) = 0;

        /// Removes all loggers.
        virtual void removeAll() = 0;

        /// Flushes all registered loggers.
        virtual void flushAll() = 0;

        /// Logs a structured message.
        /// @param message Complete log message with context.
        virtual void log(const Message& message) = 0;

        /// Logs a simple message at a severity level.
        /// @param level Log severity level.
        /// @param message Message text.
        virtual void log(Level level, StringSlice message) = 0;

        /// Logs a categorized message with full context.
        /// @param level Log severity level.
        /// @param category Log category/subsystem name.
        /// @param message Message text.
        /// @param timestamp Message timestamp (0 = auto-fill current time).
        /// @param threadId Thread ID (0 = auto-fill current thread).
        /// @param location Source code location.
        virtual void log(
            Level level,
            StringSlice category,
            StringSlice message,
            uint64 timestamp = 0,
            uint64 threadId = 0,
            SourceLocation location = {}) = 0;

        /// Logs a formatted message with variadic arguments.
        /// Uses Ark::format for message formatting.
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
            uint64 effectiveTimestamp = timestamp;
            if (effectiveTimestamp == 0)
            {
                Result<uint64> currentTime = System::Time::getCurrentTimeMilliseconds();
                effectiveTimestamp = currentTime.getValueOr(0);
            }

            uint64 effectiveThreadId = threadId;
            if (effectiveThreadId == 0)
            {
                effectiveThreadId = Ark::Concurrency::Thread::getCurrentId();
            }

            log(Message{
                level,
                category,
                Ark::format(format, Ark::forward<Args>(args)...),
                effectiveTimestamp,
                effectiveThreadId,
                location,
            });
        }
    };

    class Service final : public ServiceInterface
    {
    private:
        mutable Concurrency::Mutex mutex;
        Collections::HashMap<String, SharedPointer<Logger>> loggers;

    public:
        static Service* createService()
        {
            auto instance = makeUnique<Service>();
            auto instancePtr = instance.get();
            ServiceInterface::setInstance(Ark::move(instance));
            return instancePtr;
        }

        Service() = default;
        ~Service() = default;

        SharedPointer<Logger> getLogger(StringSlice name) const override;

        Logger* addLogger(StringSlice name) override;

        void removeLogger(StringSlice name) override;

        void removeAll() override;

        void flushAll() override;

        void log(const Message& message) override;

        void log(Level level, StringSlice message) override;

        void log(
            Level level,
            StringSlice category,
            StringSlice message,
            uint64 timestamp,
            uint64 threadId,
            SourceLocation location) override;
    };
}
