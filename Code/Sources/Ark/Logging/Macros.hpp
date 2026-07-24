#pragma once

#include "Ark/Logging/Service.hpp"

/// @defgroup LoggingLocationMacros Source Location Capture
/// @{
/// Captures the current source code location (file, line, function).
/// @{

/// Macro to create a SourceLocation
/// Automatically captures __FILE__, __LINE__, and __FUNCTION__.
#define ARK_SOURCE_LOCATION              \
    Ark::Logging::SourceLocation         \
    {                                    \
        __FILE__, __LINE__, __FUNCTION__ \
    }

/// @}
/// @}

/// @defgroup LoggerInstanceMacros Logger Instance Logging
/// Logging macros for explicit logger instances.
/// @{

/// Logs a message with a specific logger at a given level.
/// Respects category-based level filtering.
/// @param logger Logger instance (nullptr-safe).
/// @param level Log severity level.
/// @param category Category name (string).
/// @param message Message text.
/// @note Uses Ark::format internally; message must be a format-compatible string.
#define ARK_LOGGER(logger, level, category, message) \
    ARK_LOGGER_FORMAT(logger,                        \
                      level,                         \
                      category,                      \
                      "{}",                          \
                      message)

/// Logs a debug message to a logger.
/// @param logger Logger instance.
/// @param category Category name.
/// @param message Message text.
#define ARK_LOGGER_DEBUG(logger, category, message) ARK_LOGGER(logger, Ark::Logging::Level::Debug, category, message)

/// Logs an info message to a logger.
#define ARK_LOGGER_INFO(logger, category, message) ARK_LOGGER(logger, Ark::Logging::Level::Info, category, message)

/// Logs a warning message to a logger.
#define ARK_LOGGER_WARNING(logger, category, message) ARK_LOGGER(logger, Ark::Logging::Level::Warning, category, message)

/// Logs an error message to a logger.
#define ARK_LOGGER_ERROR(logger, category, message) ARK_LOGGER(logger, Ark::Logging::Level::Error, category, message)

/// Logs a fatal message to a logger.
#define ARK_LOGGER_FATAL(logger, category, message) ARK_LOGGER(logger, Ark::Logging::Level::Fatal, category, message)

#define ARK_LOGGER_MESSAGE_FORMAT(logger, level, category, format, ...)

/// Logs a formatted message to a logger.
/// @param logger Logger instance.
/// @param level Log severity level.
/// @param category Category name.
/// @param format Ark::format-compatible format string.
/// @param ... Format arguments.
#define ARK_LOGGER_FORMAT(logger, level, category, format, ...) \
    do                                                          \
    {                                                           \
        if (ARK_LOG_GET_CATEGORY(category).shouldLog(level))    \
        {                                                       \
            if ((logger) != nullptr)                            \
            {                                                   \
                (logger)->logFormat(                            \
                    level,                                      \
                    ARK_LOG_GET_CATEGORY(category).name,        \
                    0,                                          \
                    0,                                          \
                    ARK_SOURCE_LOCATION,                        \
                    format,                                     \
                    ##__VA_ARGS__);                             \
            }                                                   \
        }                                                       \
    }                                                           \
    while (0)

/// Logs a debug formatted message to a logger.
/// @param logger Logger instance.
/// @param category Category name.
/// @param format Format string.
/// @param ... Format arguments.
#define ARK_LOGGER_DEBUG_FORMAT(logger, category, format, ...) ARK_LOGGER_FORMAT(logger, Ark::Logging::Level::Debug, category, format, __VA_ARGS__)

/// Logs an info formatted message to a logger.
#define ARK_LOGGER_INFO_FORMAT(logger, category, format, ...) ARK_LOGGER_FORMAT(logger, Ark::Logging::Level::Info, category, format, __VA_ARGS__)

/// Logs a warning formatted message to a logger.
#define ARK_LOGGER_WARNING_FORMAT(logger, category, format, ...) ARK_LOGGER_FORMAT(logger, Ark::Logging::Level::Warning, category, format, __VA_ARGS__)

/// Logs an error formatted message to a logger.
#define ARK_LOGGER_ERROR_FORMAT(logger, category, format, ...) ARK_LOGGER_FORMAT(logger, Ark::Logging::Level::Error, category, format, __VA_ARGS__)

/// Logs a fatal formatted message to a logger.
#define ARK_LOGGER_FATAL_FORMAT(logger, category, format, ...) ARK_LOGGER_FORMAT(logger, Ark::Logging::Level::Fatal, category, format, __VA_ARGS__)

/// @}

/// @defgroup ServiceLoggingMacros Global Service Logging
/// Logging macros using the global logging service instance.
/// @{

/// Logs a categorized message to the global service.
/// @param level Log severity level (Debug, Info, Warning, Error, Fatal).
/// @param CategoryName Category name.
/// @param format Format string.
/// @param ... Format arguments.

// Un-categorized formatted variant
#define ARK_LOG(level, format, ...)                          \
    do                                                       \
    {                                                        \
        if (Ark::Logging::Service::getInstance() != nullptr) \
        {                                                    \
            Ark::Logging::Service::getInstance()->logFormat( \
                Ark::Logging::Level::level,                  \
                "",                                          \
                0,                                           \
                0,                                           \
                ARK_SOURCE_LOCATION,                         \
                format,                                      \
                ##__VA_ARGS__);                              \
        }                                                    \
    }                                                        \
    while (0)

#define ARK_LOG_FORMAT(level, category, format, ...)        \
    ARK_LOGGER_FORMAT(Ark::Logging::Service::getInstance(), \
                      level,                                \
                      category,                             \
                      format,                               \
                      ##__VA_ARGS__);

#define ARK_LOG_DEBUG_FORMAT(category, format, ...) ARK_LOG_FORMAT(Ark::Logging::Level::Debug, category, format, ##__VA_ARGS__)
#define ARK_LOG_INFO_FORMAT(category, format, ...) ARK_LOG_FORMAT(Ark::Logging::Level::Info, category, format, ##__VA_ARGS__)
#define ARK_LOG_WARNING_FORMAT(category, format, ...) ARK_LOG_FORMAT(Ark::Logging::Level::Warning, category, format, ##__VA_ARGS__)
#define ARK_LOG_ERROR_FORMAT(category, format, ...) ARK_LOG_FORMAT(Ark::Logging::Level::Error, category, format, ##__VA_ARGS__)
#define ARK_LOG_FATAL_FORMAT(category, format, ...) ARK_LOG_FORMAT(Ark::Logging::Level::Fatal, category, format, ##__VA_ARGS__)

#define ARK_LOG_DECLARE_CATEGORY(CategoryName) \
    extern const ::Ark::Logging::Category ArkLogCategory##CategoryName;

#define ARK_LOG_DEFINE_CATEGORY(CategoryName) \
    const Ark::Logging::Category ArkLogCategory##CategoryName{#CategoryName};

#define ARK_LOG_DEFINE_CATEGORY_LEVEL(CategoryName, DefaultLevel) \
    const Ark::Logging::Category ArkLogCategory##CategoryName{#CategoryName, Ark::Logging::Level::DefaultLevel};

#define ARK_LOG_DEFINE_CATEGORY_STATIC(CategoryName) \
    static const Ark::Logging::Category ArkLogCategory##CategoryName{#CategoryName};

#define ARK_LOG_DEFINE_CATEGORY_LEVEL_STATIC(CategoryName, DefaultLevel) \
    static const Ark::Logging::Category ArkLogCategory##CategoryName{#CategoryName, Ark::Logging::Level::DefaultLevel};

#define ARK_LOG_GET_CATEGORY(CategoryName) ArkLogCategory##CategoryName

// Parent variants
#define ARK_LOG_DEFINE_CATEGORY_PARENT(CategoryName, ParentCategoryName) \
    const Ark::Logging::Category ArkLogCategory##CategoryName{#CategoryName, Ark::Logging::Level::None, &ArkLogCategory##ParentCategoryName};

#define ARK_LOG_DEFINE_CATEGORY_LEVEL_PARENT(CategoryName, DefaultLevel, ParentCategoryName) \
    const Ark::Logging::Category ArkLogCategory##CategoryName{#CategoryName, Ark::Logging::Level::DefaultLevel, &ArkLogCategory##ParentCategoryName};

#define ARK_LOG_DEFINE_CATEGORY_PARENT_STATIC(CategoryName, ParentCategoryName) \
    static const Ark::Logging::Category ArkLogCategory##CategoryName{#CategoryName, Ark::Logging::Level::None, &ArkLogCategory##ParentCategoryName};

#define ARK_LOG_DEFINE_CATEGORY_LEVEL_PARENT_STATIC(CategoryName, DefaultLevel, ParentCategoryName) \
    static const Ark::Logging::Category ArkLogCategory##CategoryName{#CategoryName, Ark::Logging::Level::DefaultLevel, &ArkLogCategory##ParentCategoryName};
