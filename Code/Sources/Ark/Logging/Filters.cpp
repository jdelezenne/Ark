#include "Ark/Logging/Filters.hpp"
#include "Ark/Logging/Message.hpp"

namespace Ark::Logging
{
    bool LevelFilter::shouldLog(const Message& message) const
    {
        // Threshold semantics: allow messages at or above configured level
        return static_cast<int>(message.level) >= static_cast<int>(level);
    }

    void CategoryFilter::addCategory(StringSlice category)
    {
        categories.append({String(category)});
    }

    bool CategoryFilter::shouldLog(const Message& message) const
    {
        for (auto it = categories.getStartIterator(); it != categories.getEndIterator(); ++it)
        {
            if (String(message.category) == *it)
            {
                return true;
            }
        }
        return false;
    }

#if (ARK_USE_STANDARD_LIBRARY == ARK_STANDARD_LIBRARY_CPP)
    bool RegexFilter::shouldLog(const Message& message) const
    {
        return std::regex_search(message.message.asPointer(), regex);
    }
#endif

    bool TimestampFilter::shouldLog(const Message& message) const
    {
        return (message.timestamp >= startTime) && (message.timestamp <= endTime);
    }
}
