#pragma once

#include "Ark/Collections/HashMap.hpp"
#include "Ark/Logging/Interfaces.hpp"
#include "Ark/Logging/Level.hpp"
#include "Ark/Logging/Message.hpp"
#include "Ark/System/Console.hpp"

#include <functional>

namespace Ark::Logging
{
    using namespace Ark::System;

    /// Formatted console output target with color and style support.
    /// Writes log entries to console with level-based and category-based coloring.
    class FormattedConsoleTarget final : public Target
    {
    private:
        /// Format specification (foreground, background, style).
        struct Format
        {
            /// Foreground color.
            Console::Color foreground;
            /// Background color.
            Console::Color background;
            /// Text styling.
            Console::Style style;
        };

        /// Foreground colors per log level.
        Collections::HashMap<Level, Format> levelFormats;
        /// Foreground colors per category name.
        Collections::HashMap<String, Format> categoryFormats;

        /// Default foreground color.
        Console::Color defaultForeground;
        /// Default background color.
        Console::Color defaultBackground;
        /// Default text style.
        Console::Style defaultStyle;

    public:
        /// Constructs a formatted console target with default colors.
        /// Initializes with level-based colors: Debug=Cyan, Info=Green, Warning=Yellow, Error=Red, Fatal=BrightRed+Bold.
        FormattedConsoleTarget()
            : defaultForeground{Console::Color::Default}
            , defaultBackground{Console::Color::Default}
            , defaultStyle{Console::Style::Default}
        {
            initializeDefaults();
        }

        /// Writes a log entry to console with applied formatting.
        /// Applies level-based or category-based colors, falls back to defaults.
        /// @param entry Log entry with formatted message.
        void write(const Entry& entry) override
        {
            Console::Color foreground = defaultForeground;
            Console::Color background = defaultBackground;
            Console::Style style = defaultStyle;

            for (const auto& [level, format] : levelFormats)
            {
                if (entry.message.level == level)
                {
                    foreground = (format.foreground == Console::Color::Default) ? defaultForeground : format.foreground;
                    background = (format.background == Console::Color::Default) ? defaultBackground : format.background;
                    style = (format.style == Console::Style::Default) ? defaultStyle : format.style;
                    break;
                }
            }

            for (const auto& [category, format] : categoryFormats)
            {
                if (entry.message.category.compare(category) == Ordering::Equal)
                {
                    foreground = (format.foreground == Console::Color::Default) ? defaultForeground : format.foreground;
                    background = (format.background == Console::Color::Default) ? defaultBackground : format.background;
                    style = (format.style == Console::Style::Default) ? defaultStyle : format.style;
                    break;
                }
            }

            Console::printlnFormatted(entry.formattedMessage, foreground, background, style);
        }

        /// Flushes output buffer (no-op for console).
        void flush() override
        {
        }

        /// Clears all level-based format mappings.
        void clearLevelFormats()
        {
            levelFormats.removeAll();
        }

        /// Clears all category-based format mappings.
        void clearCategoryFormats()
        {
            categoryFormats.removeAll();
        }

        /// Sets the console format for a specific log level.
        /// @param level Log severity level.
        /// @param foreground Foreground color (required).
        /// @param background Background color (defaults to none).
        /// @param style Text styling (defaults to normal).
        void setLevelFormat(Level level, Console::Color foreground, Console::Color background = Console::Color::Default, Console::Style style = Console::Style::Default)
        {
            levelFormats[level] = {foreground, background, style};
        }

        /// Sets the console format for a specific log category.
        /// @param category Category name.
        /// @param foreground Foreground color (required).
        /// @param background Background color (defaults to none).
        /// @param style Text styling (defaults to normal).
        void setCategoryFormat(StringSlice category, Console::Color foreground, Console::Color background = Console::Color::Default, Console::Style style = Console::Style::Default)
        {
            categoryFormats[String(category)] = {foreground, background, style};
        }

        /// Sets the default formatting for all messages.
        /// @param foreground Default foreground color (required).
        /// @param background Default background color (defaults to none).
        /// @param style Default text styling (defaults to normal).
        void setDefaultFormatting(Console::Color foreground, Console::Color background = Console::Color::Default, Console::Style style = Console::Style::Default)
        {
            defaultForeground = foreground;
            defaultBackground = background;
            defaultStyle = style;
        }

    private:
        /// Initializes default level-based colors.
        void initializeDefaults()
        {
            setLevelFormat(Level::Debug, Console::Color::Cyan);
            setLevelFormat(Level::Info, Console::Color::Green);
            setLevelFormat(Level::Warning, Console::Color::Yellow);
            setLevelFormat(Level::Error, Console::Color::Red);
            setLevelFormat(Level::Fatal, Console::Color::BrightRed, Console::Color::Default, Console::Style::Bold);
        }
    };
}
