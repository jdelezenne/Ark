#include "Ark/System/Console.hpp"
#include "Ark/Collections/InlineArray.hpp"
#include "Ark/Core/Macros.hpp"

#include <cstdarg>
#include <iostream>
#include <termios.h>
#include <unistd.h>

namespace Ark::System
{
    namespace
    {
        static const Collections::InlineArray<const char*, static_cast<usize>(Console::ColorCount)> colorCodes{
            "39", // Default
            "30", // Black
            "31", // Red
            "32", // Green
            "33", // Yellow
            "34", // Blue
            "35", // Magenta
            "36", // Cyan
            "37", // White
            "90", // Bright Black (Gray)
            "91", // Bright Red
            "92", // Bright Green
            "93", // Bright Yellow
            "94", // Bright Blue
            "95", // Bright Magenta
            "96", // Bright Cyan
            "97", // Bright White
        };

        const char* getColorCode(Console::Color color, bool isBackground = false)
        {
            static char buffer[5];
            const char* baseCode = colorCodes[static_cast<usize>(color)];
            std::snprintf(buffer, sizeof(buffer), isBackground ? "4%s" : "3%s", baseCode + 1);
            return buffer;
        }

        const char* getStyleCode(Console::Style style)
        {
            switch (style)
            {
                case Console::Style::Bold:
                    return "1";
                case Console::Style::Underline:
                    return "4";
                case Console::Style::Italic:
                    return "3";
                default:
                    return "0";
            }
        }

        void writeEscapeSequence(const char* sequence)
        {
            std::cout << "\033[" << sequence;
        }
    }

    void Console::print(StringSlice text)
    {
        std::cout << text.asPointer();
    }

    void Console::println(StringSlice text)
    {
        std::cout << text.asPointer() << std::endl;
    }

    void Console::printf(char const* format, ...)
    {
        char buffer[2048];
        va_list args;
        va_start(args, format);
        int n = vsnprintf(buffer, sizeof(buffer), format, args);
        va_end(args);
        if (n > 0)
        {
            std::cout << StringSlice(buffer).asPointer();
        }
    }

    void Console::printlnf(char const* format, ...)
    {
        char buffer[2048];
        va_list args;
        va_start(args, format);
        int n = vsnprintf(buffer, sizeof(buffer), format, args);
        va_end(args);
        if (n > 0)
        {
            std::cout << StringSlice(buffer).asPointer();
        }
        std::cout << std::endl;
    }

    void Console::printColored(StringSlice text, Color foreground, Color background)
    {
        printFormatted(text, foreground, background);
    }

    void Console::printlnColored(StringSlice text, Color foreground, Color background)
    {
        printlnFormatted(text, foreground, background);
    }

    void Console::printfColored(Color foreground, Color background, char const* format, ...)
    {
        char buffer[2048];
        va_list args;
        va_start(args, format);
        int n = vsnprintf(buffer, sizeof(buffer), format, args);
        va_end(args);
        if (n > 0)
        {
            printFormatted(StringSlice(buffer), foreground, background);
        }
    }

    void Console::printfColored(Color foreground, char const* format, ...)
    {
        char buffer[2048];
        va_list args;
        va_start(args, format);
        int n = vsnprintf(buffer, sizeof(buffer), format, args);
        va_end(args);
        if (n > 0)
        {
            printFormatted(StringSlice(buffer), foreground, Color::Default);
        }
    }

    void Console::printStyled(StringSlice text, Style style)
    {
        printFormatted(text, Color::Default, Color::Default, style);
    }

    void Console::printlnStyled(StringSlice text, Style style)
    {
        printlnFormatted(text, Color::Default, Color::Default, style);
    }

    void Console::printFormatted(StringSlice text, Color foreground, Color background, Style style)
    {
        std::cout << "\033[" << getStyleCode(style) << ";"
                  << getColorCode(foreground) << ";"
                  << getColorCode(background, true) << "m"
                  << text.asPointer() << "\033[0m";
    }

    void Console::printlnFormatted(StringSlice text, Color foreground, Color background, Style style)
    {
        printFormatted(text, foreground, background, style);
        std::cout << std::endl;
    }

    void Console::printfFormatted(Color foreground, Color background, Style style, char const* format, ...)
    {
        char buffer[2048];
        va_list args;
        va_start(args, format);
        int n = vsnprintf(buffer, sizeof(buffer), format, args);
        va_end(args);
        if (n > 0)
        {
            printFormatted(StringSlice(buffer), foreground, background, style);
        }
    }

    void Console::printfFormatted(Color foreground, Color background, char const* format, ...)
    {
        char buffer[2048];
        va_list args;
        va_start(args, format);
        int n = vsnprintf(buffer, sizeof(buffer), format, args);
        va_end(args);
        if (n > 0)
        {
            printFormatted(StringSlice(buffer), foreground, background);
        }
    }

    void Console::printfFormatted(Color foreground, char const* format, ...)
    {
        char buffer[2048];
        va_list args;
        va_start(args, format);
        int n = vsnprintf(buffer, sizeof(buffer), format, args);
        va_end(args);
        if (n > 0)
        {
            printFormatted(StringSlice(buffer), foreground, Color::Default);
        }
    }

    void Console::flush()
    {
        std::cout.flush();
    }

    void Console::moveCursor(int x, int y)
    {
        std::cout << "\033[" << y << ';' << x << 'H';
    }

    void Console::saveCursorPosition()
    {
        std::cout << "\0337"; // DECSC
    }

    void Console::restoreCursorPosition()
    {
        std::cout << "\0338"; // DECRC
    }

    void Console::clearScreen()
    {
        writeEscapeSequence("2J");
        writeEscapeSequence("H");
    }

    void Console::clearLine()
    {
        writeEscapeSequence("2K");
    }

    void Console::scrollUp()
    {
        // ANSI escape code for scrolling up
        std::cout << "\033[" << 1 << "S";

        // Move cursor up
        std::cout << "\033[" << 1 << "A";

        std::cout.flush();
    }

    void Console::scrollDown()
    {
        // ANSI escape code for scrolling down
        std::cout << "\033[" << 1 << "T";

        // Move cursor down
        std::cout << "\033[" << 1 << "B";

        std::cout.flush();
    }

    void Console::waitForKeyPress()
    {
        if (::isatty(STDIN_FILENO) <= 0)
        {
            return;
        }

        termios oldt{};
        if (tcgetattr(STDIN_FILENO, &oldt) != 0)
        {
            return;
        }

        termios newt = oldt;
        newt.c_lflag &= static_cast<unsigned>(~(ICANON | ECHO));
        if (tcsetattr(STDIN_FILENO, TCSANOW, &newt) != 0)
        {
            return;
        }

        char ch;
        ARK_UNUSED(::read(STDIN_FILENO, &ch, 1));
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    }
}
