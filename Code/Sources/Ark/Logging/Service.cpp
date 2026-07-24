#include "Ark/Logging/Service.hpp"
#include "Ark/Concurrency/Locks.hpp"
#include "Ark/Core/Functions.hpp"
#include "Ark/Logging/Interfaces.hpp"
#include "Ark/Logging/Logger.hpp"

namespace Ark::Logging
{
    UniquePointer<ServiceInterface> ServiceInterface::instance = UniquePointer<ServiceInterface>(nullptr);

    SharedPointer<Logger> Service::getLogger(StringSlice name) const
    {
        Concurrency::LockGuard<Concurrency::Mutex> lock(mutex);
        auto it = loggers.find(String(name));
        if (it != loggers.end())
        {
            return it->second;
        }

        return SharedPointer<Logger>{nullptr};
    }

    void Service::flushAll()
    {
        Collections::Array<SharedPointer<Logger>> snapshot;
        {
            Concurrency::LockGuard<Concurrency::Mutex> lock(mutex);
            snapshot.reserve(loggers.getCount());
            for (const auto& [name, logger] : loggers)
            {
                snapshot.append(logger);
            }
        }

        for (const auto& logger : snapshot)
        {
            for (const auto& target : logger->getTargets())
            {
                target->flush();
            }
        }
    }

    Logger* Service::addLogger(StringSlice name)
    {
        SharedPointer<Logger> logger = makeShared<Logger>();
        Logger* loggerPtr = logger.get();

        Concurrency::LockGuard<Concurrency::Mutex> lock(mutex);
        loggers[String(name)] = Ark::move(logger);

        return loggerPtr;
    }

    void Service::removeLogger(StringSlice name)
    {
        Concurrency::LockGuard<Concurrency::Mutex> lock(mutex);
        loggers.remove(String(name));
    }

    void Service::removeAll()
    {
        Concurrency::LockGuard<Concurrency::Mutex> lock(mutex);
        loggers = {};
    }

    void Service::log(const Message& message)
    {
        Collections::Array<SharedPointer<Logger>> snapshot;
        {
            Concurrency::LockGuard<Concurrency::Mutex> lock(mutex);
            snapshot.reserve(loggers.getCount());
            for (const auto& [_, logger] : loggers)
            {
                snapshot.append(logger);
            }
        }

        for (const auto& logger : snapshot)
        {
            logger->log(message);
        }
    }

    void Service::log(Level level, StringSlice text)
    {
        Message message{};
        message.level = level;
        message.message = text;
        log(message);
    }

    void Service::log(
        Level level,
        StringSlice category,
        StringSlice text,
        uint64 timestamp,
        uint64 threadId,
        SourceLocation location)
    {
        log(Message{
            level,
            category,
            String(text),
            timestamp,
            threadId,
            location,
        });
    }
}
