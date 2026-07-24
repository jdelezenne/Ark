#pragma once

#include "Ark/Core/Functions.hpp"
#include "Ark/Core/Macros.hpp"
#include "Ark/Core/Types.hpp"
#include "Ark/Strings/Format.hpp"
#include "Ark/Strings/StringSlice.hpp"
#include <cstdarg>
#include <cstdio>

namespace Ark::System
{
    struct Console final
    {
        ARK_STATIC_STRUCT(Console)

    public:
        enum class Color
        {
            Default,
            Black,
            Red,
            Green,
            Yellow,
            Blue,
            Magenta,
            Cyan,
            White,
            BrightBlack,
            BrightRed,
            BrightGreen,
            BrightYellow,
            BrightBlue,
            BrightMagenta,
            BrightCyan,
            BrightWhite,
        };

        constexpr static usize ColorCount = static_cast<usize>(Color::BrightWhite) + 1;

        enum class Style
        {
            Default,
            Bold,
            Italic,
            Underline,
        };

        constexpr static usize StyleCount = static_cast<usize>(Style::Underline) + 1;

    public:
        static void print(StringSlice text);
        static void println(StringSlice text);

        /// Prints using stdio-style printf formatting.
        static void printf(char const* format, ...);

        /// Prints using stdio-style printf formatting and appends a newline.
        static void printlnf(char const* format, ...);

        /// Prints a formatted message.
        template <typename... Args>
        static void printFormat(Ark::FormatString<Args...> format, Args&&... args)
        {
            auto formatted = Ark::format(format, Ark::forward<Args>(args)...);
            print(formatted);
        }

        /// Prints a formatted message followed by a newline.
        template <typename... Args>
        static void printlnFormat(Ark::FormatString<Args...> format, Args&&... args)
        {
            auto formatted = Ark::format(format, Ark::forward<Args>(args)...);
            println(formatted);
        }

        static void printColored(StringSlice text, Color foreground, Color background = Color::Default);
        static void printlnColored(StringSlice text, Color foreground, Color background = Color::Default);

        /// Prints a colored message using stdio-style printf formatting.
        static void printfColored(Color foreground, Color background, char const* format, ...);

        /// Prints a colored message using stdio-style printf formatting (default background).
        static void printfColored(Color foreground, char const* format, ...);

        /// Prints a colored formatted message.
        template <typename... Args>
        static void printFormatColored(Color foreground, Color background, Ark::FormatString<Args...> format, Args&&... args)
        {
            auto formatted = Ark::format(format, Ark::forward<Args>(args)...);
            printColored(formatted, foreground, background);
        }

        /// Prints a colored formatted message (with default background).
        template <typename... Args>
        static void printFormatColored(Color foreground, Ark::FormatString<Args...> format, Args&&... args)
        {
            printFormatColored(foreground, Color::Default, format, Ark::forward<Args>(args)...);
        }

        /// Prints a colored formatted message followed by a newline.
        template <typename... Args>
        static void printlnFormatColored(Color foreground, Color background, Ark::FormatString<Args...> format, Args&&... args)
        {
            auto formatted = Ark::format(format, Ark::forward<Args>(args)...);
            printlnColored(formatted, foreground, background);
        }

        /// Prints a colored formatted message (with default background) followed by a newline.
        template <typename... Args>
        static void printlnFormatColored(Color foreground, Ark::FormatString<Args...> format, Args&&... args)
        {
            printlnFormatColored(foreground, Color::Default, format, Ark::forward<Args>(args)...);
        }

        static void printStyled(StringSlice text, Style style = Style::Default);
        static void printlnStyled(StringSlice text, Style style = Style::Default);

        /// Prints a styled formatted message.
        template <typename... Args>
        static void printFormatStyled(Style style, Ark::FormatString<Args...> format, Args&&... args)
        {
            auto formatted = Ark::format(format, Ark::forward<Args>(args)...);
            printStyled(formatted, style);
        }

        /// Prints a styled formatted message followed by a newline.
        template <typename... Args>
        static void printlnFormatStyled(Style style, Ark::FormatString<Args...> format, Args&&... args)
        {
            auto formatted = Ark::format(format, Ark::forward<Args>(args)...);
            printlnStyled(formatted, style);
        }

        static void printFormatted(StringSlice text, Color foreground, Color background = Color::Default, Style style = Style::Default);
        static void printlnFormatted(StringSlice text, Color foreground, Color background = Color::Default, Style style = Style::Default);

        /// Prints a formatted message (color/style) using stdio-style printf formatting.
        static void printfFormatted(Color foreground, Color background, Style style, char const* format, ...);

        /// Prints a formatted message (color, default style) using stdio-style printf formatting.
        static void printfFormatted(Color foreground, Color background, char const* format, ...);

        /// Prints a formatted message (color only) using stdio-style printf formatting.
        static void printfFormatted(Color foreground, char const* format, ...);

        /// Prints a formatted message with color and style.
        template <typename... Args>
        static void printFormatColoredStyled(Color foreground, Color background, Style style, Ark::FormatString<Args...> format, Args&&... args)
        {
            auto formatted = Ark::format(format, Ark::forward<Args>(args)...);
            printFormatted(formatted, foreground, background, style);
        }

        /// Prints a formatted message with color and default style.
        template <typename... Args>
        static void printFormatColoredStyled(Color foreground, Color background, Ark::FormatString<Args...> format, Args&&... args)
        {
            printFormatColoredStyled(foreground, background, Style::Default, format, Ark::forward<Args>(args)...);
        }

        /// Prints a formatted message with default background and style.
        template <typename... Args>
        static void printFormatColoredStyled(Color foreground, Ark::FormatString<Args...> format, Args&&... args)
        {
            printFormatColoredStyled(foreground, Color::Default, Style::Default, format, Ark::forward<Args>(args)...);
        }

        /// Prints a formatted message with color and style followed by a newline.
        template <typename... Args>
        static void printlnFormatColoredStyled(Color foreground, Color background, Style style, Ark::FormatString<Args...> format, Args&&... args)
        {
            auto formatted = Ark::format(format, Ark::forward<Args>(args)...);
            printlnFormatted(formatted, foreground, background, style);
        }

        /// Prints a formatted message with color and default style followed by a newline.
        template <typename... Args>
        static void printlnFormatColoredStyled(Color foreground, Color background, Ark::FormatString<Args...> format, Args&&... args)
        {
            printlnFormatColoredStyled(foreground, background, Style::Default, format, Ark::forward<Args>(args)...);
        }

        /// Prints a formatted message with default background and style followed by a newline.
        template <typename... Args>
        static void printlnFormatColoredStyled(Color foreground, Ark::FormatString<Args...> format, Args&&... args)
        {
            printlnFormatColoredStyled(foreground, Color::Default, Style::Default, format, Ark::forward<Args>(args)...);
        }

        static void flush();

        static void moveCursor(int x, int y);
        static void saveCursorPosition();
        static void restoreCursorPosition();

        static void clearScreen();
        static void clearLine();

        static void scrollUp();
        static void scrollDown();

        static void waitForKeyPress();
    };
}
