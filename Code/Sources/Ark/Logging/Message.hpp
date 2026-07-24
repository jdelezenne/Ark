#pragma once

#include "Ark/Core/Types.hpp"
#include "Ark/Logging/Level.hpp"
#include "Ark/Strings/String.hpp"
#include "Ark/Strings/StringSlice.hpp"

namespace Ark::Logging
{
    /// Source Location
    /// @details Represents the location where a log was created.
    struct SourceLocation final
    {
        StringSlice fileName{};     ///< The file where the log was created.
        uint32 lineNumber{};        ///< The line number where the log was created.
        StringSlice functionName{}; ///< The name of the function where the log was created.
    };

    /// Log message category with hierarchical level defaults.
    /// Supports parent-child relationships for log filtering and inheritance.
    struct Category final
    {
        /// Category name (e.g., "Engine", "Renderer").
        StringSlice name;
        /// Default severity level for this category.
        Level defaultLevel;
        /// Parent category (for inheritance).
        Category const* parent = nullptr;

        /// Constructs a top-level category with explicit level.
        /// @param name Category identifier.
        /// @param defaultLevel Minimum log level for this category.
        constexpr Category(StringSlice name, Level defaultLevel = Level::None)
            : name{name}
            , defaultLevel{defaultLevel}
            , parent{nullptr}
        {
        }

        /// Constructs a child category with explicit level.
        /// @param name Category identifier.
        /// @param defaultLevel Minimum log level for this category.
        /// @param parent Parent category (for level inheritance).
        constexpr Category(StringSlice name, Level defaultLevel, Category const* parent)
            : name{name}
            , defaultLevel{defaultLevel}
            , parent{parent}
        {
        }

        /// Computes effective default level (respects inheritance).
        /// Returns explicit level if set, otherwise checks parent hierarchy.
        constexpr Level effectiveDefaultLevel() const
        {
            // Inherit from parent when not explicitly set
            if (defaultLevel == Level::None)
            {
                if (parent != nullptr)
                {
                    return parent->effectiveDefaultLevel();
                }
                return Level::Info;
            }

            // Explicit level set on this category takes precedence
            return defaultLevel;
        }

        constexpr bool shouldLog(Level level) const
        {
            return static_cast<int>(level) >= static_cast<int>(effectiveDefaultLevel());
        }
    };

    /// Complete log message with context and metadata.
    struct Message final
    {
        /// Severity level of the message.
        Level level = Level::None;
        /// Category or subsystem name.
        StringSlice category{};
        /// Log message text.
        String message{};
        /// Timestamp in milliseconds since Unix epoch (UTC).
        uint64 timestamp = 0;
        /// ID of the thread that logged this message.
        uint64 threadId = 0;
        /// Source code location (file, line, function).
        SourceLocation location{};
    };
}
