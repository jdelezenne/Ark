#include "Ark/Logging/Logger.hpp"
#include "Ark/Concurrency/Thread.hpp"
#include "Ark/Core/Debug/Assert.hpp"
#include "Ark/Logging/Interfaces.hpp"
#include "Ark/System/Time.hpp"

namespace Ark::Logging
{
    namespace
    {
        uint64 getCurrentTimestampMilliseconds()
        {
            Result<uint64> currentTime = System::Time::getCurrentTimeMilliseconds();
            ARK_ASSERT(currentTime.isOk());
            return currentTime.getValueOr(0);
        }
    }

    const Collections::Array<SharedPointer<Filter>>& Logger::getFilters() const
    {
        return filters;
    }

    SharedPointer<Formatter> Logger::getFormatter() const
    {
        return formatter;
    }

    const Collections::Array<SharedPointer<Target>>& Logger::getTargets() const
    {
        return targets;
    }

    void Logger::addFilter(SharedPointer<Filter> filter)
    {
        ARK_ASSERT(filter.get() != nullptr);
        filters.append(Ark::move(filter));
    }

    void Logger::setFormatter(SharedPointer<Formatter> newFormatter)
    {
        formatter = Ark::move(newFormatter);
    }

    void Logger::addTarget(SharedPointer<Target> target)
    {
        ARK_ASSERT(target.get() != nullptr);
        targets.append(Ark::move(target));
    }

    void Logger::log(const Message& message)
    {
        Message normalizedMessage = message;

        if (normalizedMessage.timestamp == 0)
        {
            normalizedMessage.timestamp = getCurrentTimestampMilliseconds();
        }

        if (normalizedMessage.threadId == 0)
        {
            normalizedMessage.threadId = Ark::Concurrency::Thread::getCurrentId();
        }

        bool shouldWrite = filters.isEmpty();
        if (!shouldWrite)
        {
            shouldWrite = true;
            for (auto iter = filters.getStartIterator(); iter != filters.getEndIterator(); ++iter)
            {
                ARK_ASSERT(iter->get() != nullptr);
                if (!(*iter)->shouldLog(normalizedMessage))
                {
                    shouldWrite = false;
                    break;
                }
            }
        }

        if (!shouldWrite)
        {
            return;
        }

        String formattedMessage = normalizedMessage.message;

        if (formatter.get() != nullptr)
        {
            formattedMessage = formatter->format(normalizedMessage);
        }

        Entry entry{normalizedMessage, Ark::move(formattedMessage)};
        for (const auto& target : targets)
        {
            ARK_ASSERT(target.get() != nullptr);
            target->write(entry);
        }
    }

    void Logger::log(Level level, StringSlice text)
    {
        Message message{};
        message.level = level;
        message.message = text;
        log(message);
    }

    void Logger::log(
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
