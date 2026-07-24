#include "Ark/Logging/PatternFormatter.hpp"
#include "Ark/Concurrency/Locks.hpp"
#include "Ark/Concurrency/Mutex.hpp"
#include "Ark/Logging/FormatUtilities.hpp"
#include "Ark/Logging/Message.hpp"
#include "Ark/Strings/AnsiCharacters.hpp"

namespace Ark::Logging
{
    Collections::HashMap<String, PatternFormatter::ReplaceCallback> PatternFormatter::patternMap;

    PatternFormatter::PatternFormatter(StringSlice pattern)
        : pattern{String(pattern)}
    {
        initialize();
    }

    String PatternFormatter::format(const Message& message)
    {
        StringSlice const input = pattern;
        String output;
        output.reserve(input.getLength() + 64);

        usize index = 0;
        while (index < input.getLength())
        {
            char const ch = input[index];
            if (ch == '{')
            {
                usize end = index + 1;
                while (end < input.getLength() && Characters::isAlphabetic(input[end]))
                {
                    ++end;
                }

                if (end < input.getLength() && input[end] == '}' && end > index + 1)
                {
                    StringSlice const key = input.subslice(index + 1, end);
                    auto mapIt = patternMap.find(String(key));
                    if (mapIt != patternMap.end())
                    {
                        output.append(mapIt->second(message));
                    }
                    else
                    {
                        output.append(input.subslice(index, end + 1));
                    }
                    index = end + 1;
                    continue;
                }
            }
            else if (ch == '%')
            {
                usize end = index + 1;
                while (end < input.getLength() && Characters::isAlphabetic(input[end]))
                {
                    ++end;
                }

                if (end > index + 1)
                {
                    StringSlice const key = input.subslice(index + 1, end);
                    auto mapIt = patternMap.find(String(key));
                    if (mapIt != patternMap.end())
                    {
                        output.append(mapIt->second(message));
                        index = end;
                        continue;
                    }
                }
            }

            output.append(ch);
            ++index;
        }

        return output;
    }

    void PatternFormatter::initialize()
    {
        static Concurrency::Mutex initMutex;
        Concurrency::LockGuard<Concurrency::Mutex> lock(initMutex);

        if (!patternMap.isEmpty())
        {
            return;
        }

        patternMap[String("level")] = [](const Message& e)
        {
            return String(levelToString(e.level));
        };
        patternMap[String("category")] = [](const Message& e)
        {
            return String(e.category);
        };
        patternMap[String("message")] = [](const Message& e)
        {
            return e.message;
        };
        patternMap[String("timestamp")] = [](const Message& e)
        {
            return formatTimestamp(e.timestamp);
        };
        patternMap[String("threadId")] = [](const Message& e)
        {
            return Internal::formatUnsigned(e.threadId);
        };
        patternMap[String("file")] = [](const Message& e)
        {
            return String(e.location.fileName);
        };
        patternMap[String("line")] = [](const Message& e)
        {
            return Internal::formatUnsigned(e.location.lineNumber);
        };
        patternMap[String("function")] = [](const Message& e)
        {
            return String(e.location.functionName);
        };
        patternMap[String("datetime")] = [](const Message& e)
        {
            return formatDateTime(e.timestamp);
        };
        patternMap[String("date")] = [](const Message& e)
        {
            return formatDate(e.timestamp);
        };
        patternMap[String("time")] = [](const Message& e)
        {
            return formatTime(e.timestamp);
        };

        patternMap[String("l")] = [](const Message& e)
        {
            return String(levelToString(e.level));
        };
        patternMap[String("c")] = [](const Message& e)
        {
            return String(e.category);
        };
        patternMap[String("m")] = [](const Message& e)
        {
            return e.message;
        };
        patternMap[String("t")] = [](const Message& e)
        {
            return formatTime(e.timestamp);
        };
    }

    String PatternFormatter::formatTimestamp(uint64 timestamp)
    {
        return Internal::formatUnsigned(timestamp);
    }

    String PatternFormatter::formatDateTime(uint64 timestamp)
    {
        return Internal::formatDateTime(timestamp, '-', ' ', true);
    }

    String PatternFormatter::formatDate(uint64 timestamp)
    {
        return Internal::formatDateTime(timestamp, '-', ' ', false);
    }

    String PatternFormatter::formatTime(uint64 timestamp)
    {
        Result<DateTime> dateTime = System::Time::unixMillisecondsToDateTime(timestamp, true);
        if (!dateTime.isOk())
        {
            return Internal::formatUnsigned(timestamp);
        }

        DateTime const& value = dateTime.getValue();
        String output;
        Internal::appendPadded(output, value.Time.Hour, 2);
        output.append(':');
        Internal::appendPadded(output, value.Time.Minute, 2);
        output.append(':');
        Internal::appendPadded(output, value.Time.Second, 2);
        return output;
    }
}
