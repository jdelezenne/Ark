#include "Ark/System/Console.hpp"
#include "Ark/Collections/InlineArray.hpp"
#include "Ark/Core/Debug/Assert.hpp"
#include "Ark/Core/Platform/Windows/Platform.hpp"

#include <conio.h>
#include <cstdarg>
#include <cstdio>
#include <utility>

namespace Ark::System
{
    static HANDLE consoleHandle{INVALID_HANDLE_VALUE};
    static COORD savedPosition{};

    static constexpr Ark::Collections::InlineArray<WORD, static_cast<usize>(Console::ColorCount)> colorAttributes = {
        FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,                        // Default
        0,                                                                          // Black
        FOREGROUND_RED,                                                             // Red
        FOREGROUND_GREEN,                                                           // Green
        FOREGROUND_RED | FOREGROUND_GREEN,                                          // Yellow
        FOREGROUND_BLUE,                                                            // Blue
        FOREGROUND_RED | FOREGROUND_BLUE,                                           // Magenta
        FOREGROUND_GREEN | FOREGROUND_BLUE,                                         // Cyan
        FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,                        // White
        FOREGROUND_INTENSITY,                                                       // BrightBlack
        FOREGROUND_INTENSITY | FOREGROUND_RED,                                      // BrightRed
        FOREGROUND_INTENSITY | FOREGROUND_GREEN,                                    // BrightGreen
        FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN,                   // BrightYellow
        FOREGROUND_INTENSITY | FOREGROUND_BLUE,                                     // BrightBlue
        FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE,                    // BrightMagenta
        FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE,                  // BrightCyan
        FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE, // BrightWhite
    };

    static HANDLE getConsoleHandle()
    {
        if (consoleHandle == INVALID_HANDLE_VALUE)
        {
            // Try to get existing console handle first
            consoleHandle = ::GetStdHandle(STD_OUTPUT_HANDLE);

            // Check if we have a valid console handle
            if (consoleHandle != INVALID_HANDLE_VALUE && consoleHandle != nullptr)
            {
                // Verify it's actually a console (not redirected file/pipe)
                DWORD mode;
                if (::GetConsoleMode(consoleHandle, &mode))
                {
                    // We have a valid console handle - use it
                    return consoleHandle;
                }
            }

            // No valid console, try to attach to parent or allocate new
            // First try to attach to parent console (e.g., when run from terminal)
            if (::AttachConsole(ATTACH_PARENT_PROCESS))
            {
                // After attaching, we MUST reopen stdout/stderr so streams work correctly
                // This is necessary for the shell to track cursor position
                // Use Win32 CreateFile to open console handles directly
                HANDLE hOut = ::CreateFileW(L"CONOUT$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);
                HANDLE hErr = ::CreateFileW(L"CONOUT$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);
                HANDLE hIn = ::CreateFileW(L"CONIN$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);

                if (hOut != INVALID_HANDLE_VALUE)
                {
                    ::SetStdHandle(STD_OUTPUT_HANDLE, hOut);
                    consoleHandle = hOut;
                }
                if (hErr != INVALID_HANDLE_VALUE)
                {
                    ::SetStdHandle(STD_ERROR_HANDLE, hErr);
                }
                if (hIn != INVALID_HANDLE_VALUE)
                {
                    ::SetStdHandle(STD_INPUT_HANDLE, hIn);
                }

                // Verify we have a valid console handle
                if (consoleHandle != INVALID_HANDLE_VALUE && consoleHandle != nullptr)
                {
                    DWORD mode;
                    if (::GetConsoleMode(consoleHandle, &mode))
                    {
                        // Write a newline using WriteFile (Win32) so shell tracks cursor position
                        DWORD written;
                        const char* newline = "\n";
                        ::WriteFile(consoleHandle, newline, 1, &written, nullptr);
                    }
                }
            }
            else
            {
                // No parent console, allocate a new one
                if (::AllocConsole())
                {
                    // Use Win32 CreateFile to open console handles
                    HANDLE hOut = ::CreateFileW(L"CONOUT$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);
                    HANDLE hErr = ::CreateFileW(L"CONOUT$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);
                    HANDLE hIn = ::CreateFileW(L"CONIN$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);

                    if (hOut != INVALID_HANDLE_VALUE)
                    {
                        ::SetStdHandle(STD_OUTPUT_HANDLE, hOut);
                        consoleHandle = hOut;
                    }
                    if (hErr != INVALID_HANDLE_VALUE)
                    {
                        ::SetStdHandle(STD_ERROR_HANDLE, hErr);
                    }
                    if (hIn != INVALID_HANDLE_VALUE)
                    {
                        ::SetStdHandle(STD_INPUT_HANDLE, hIn);
                    }
                }
            }

            // Verify we have a valid console handle now
            if (consoleHandle == INVALID_HANDLE_VALUE || consoleHandle == nullptr)
            {
                // Still no valid console - mark as invalid
                consoleHandle = INVALID_HANDLE_VALUE;
            }
        }

        return consoleHandle;
    }

    static WORD getColorAttribute(Console::Color color, bool isBackground = false)
    {
        const usize index = static_cast<usize>(color);
        ARK_ASSERT_MSG(index < colorAttributes.getCount(), "Color index out of bounds");

        const WORD attribute = colorAttributes[index];
        return isBackground ? attribute << 4 : attribute;
    }

    static void setTextAttribute(WORD attributes)
    {
        ::SetConsoleTextAttribute(consoleHandle, attributes);
    }

    void Console::print(StringSlice text)
    {
        const HANDLE handle = getConsoleHandle();
        if (handle == INVALID_HANDLE_VALUE)
        {
            return;
        }

        DWORD written;
        ::WriteConsoleA(
            handle,
            text.asPointer(),
            static_cast<DWORD>(text.getCount()),
            &written,
            nullptr);
    }

    void Console::println(StringSlice text)
    {
        print(text);
        print("\n");
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
            print(StringSlice(buffer));
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
            print(StringSlice(buffer));
        }
        print("\n");
    }

    void Console::printColored(StringSlice text, Color foreground, Color background)
    {
        printFormatted(text, foreground, background, Style::Default);
    }

    void Console::printlnColored(StringSlice text, Color foreground, Color background)
    {
        printlnFormatted(text, foreground, background, Style::Default);
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
            printFormatted(StringSlice(buffer), foreground, background, Style::Default);
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
            printFormatted(StringSlice(buffer), foreground, Color::Default, Style::Default);
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
        const HANDLE handle = getConsoleHandle();
        if (handle == INVALID_HANDLE_VALUE)
        {
            return;
        }

        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(handle, &csbi);
        WORD oldAttributes = csbi.wAttributes;

        WORD foregroundAttribute = getColorAttribute(foreground);
        WORD backgroundAttribute = (background == Color::Default) ? 0 : getColorAttribute(background, true);

        WORD newAttributes = foregroundAttribute | backgroundAttribute;
        switch (style)
        {
            case Style::Bold:
                newAttributes |= FOREGROUND_INTENSITY;
                break;

            case Style::Underline:
                newAttributes |= COMMON_LVB_UNDERSCORE;
                break;

            case Style::Italic:
                // Windows console doesn't support italic
            default:
                break;
        }

        setTextAttribute(newAttributes);
        print(text);
        setTextAttribute(oldAttributes);
    }

    void Console::printlnFormatted(StringSlice text, Color foreground, Color background, Style style)
    {
        printFormatted(text, foreground, background, style);
        print("\n");
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
            printFormatted(StringSlice(buffer), foreground, background, Style::Default);
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
            printFormatted(StringSlice(buffer), foreground, Color::Default, Style::Default);
        }
    }

    void Console::flush()
    {
        const HANDLE handle = getConsoleHandle();
        if (handle == INVALID_HANDLE_VALUE)
        {
            return;
        }

        // Windows console is automatically flushed, but we can force it
        ::FlushConsoleInputBuffer(handle);
    }

    void Console::moveCursor(int x, int y)
    {
        const HANDLE handle = getConsoleHandle();
        if (handle == INVALID_HANDLE_VALUE)
        {
            return;
        }

        COORD position = {static_cast<SHORT>(x), static_cast<SHORT>(y)};
        ::SetConsoleCursorPosition(handle, position);
    }

    void Console::saveCursorPosition()
    {
        const HANDLE handle = getConsoleHandle();
        if (handle == INVALID_HANDLE_VALUE)
        {
            return;
        }

        CONSOLE_SCREEN_BUFFER_INFO csbi;
        ::GetConsoleScreenBufferInfo(handle, &csbi);

        savedPosition = csbi.dwCursorPosition;
    }

    void Console::restoreCursorPosition()
    {
        const HANDLE handle = getConsoleHandle();
        if (handle == INVALID_HANDLE_VALUE)
        {
            return;
        }

        ::SetConsoleCursorPosition(handle, savedPosition);
    }

    void Console::clearScreen()
    {
        const HANDLE handle = getConsoleHandle();
        if (handle == INVALID_HANDLE_VALUE)
        {
            return;
        }

        CONSOLE_SCREEN_BUFFER_INFO csbi;
        ::GetConsoleScreenBufferInfo(consoleHandle, &csbi);

        DWORD cellCount = csbi.dwSize.X * csbi.dwSize.Y;
        DWORD count;
        COORD homeCoords = {0, 0};

        ::FillConsoleOutputCharacter(handle, ' ', cellCount, homeCoords, &count);
        ::FillConsoleOutputAttribute(handle, csbi.wAttributes, cellCount, homeCoords, &count);
        moveCursor(0, 0);
    }

    void Console::clearLine()
    {
        const HANDLE handle = getConsoleHandle();
        if (handle == INVALID_HANDLE_VALUE)
        {
            return;
        }

        CONSOLE_SCREEN_BUFFER_INFO csbi;
        ::GetConsoleScreenBufferInfo(handle, &csbi);

        DWORD count;
        COORD lineStart = {0, csbi.dwCursorPosition.Y};

        ::FillConsoleOutputCharacter(handle, ' ', csbi.dwSize.X, lineStart, &count);
        ::FillConsoleOutputAttribute(handle, csbi.wAttributes, csbi.dwSize.X, lineStart, &count);

        csbi.dwCursorPosition.X = 0;
        ::SetConsoleCursorPosition(handle, csbi.dwCursorPosition);
    }

    static void scrollConsole(int lines)
    {
        const HANDLE handle = getConsoleHandle();
        if (handle == INVALID_HANDLE_VALUE)
        {
            return;
        }

        CONSOLE_SCREEN_BUFFER_INFO csbi;
        ::GetConsoleScreenBufferInfo(handle, &csbi);

        SMALL_RECT scrollRect;
        SMALL_RECT clipRect;
        COORD destOrigin;
        CHAR_INFO fill;

        if (lines != 0)
        {
            // Set up the scrolling area
            scrollRect.Left = 0;
            scrollRect.Right = csbi.dwSize.X - 1;

            if (lines > 0)
            {
                scrollRect.Top = lines;
                scrollRect.Bottom = csbi.dwSize.Y - 1;
                destOrigin.X = 0;
                destOrigin.Y = 0;
            }
            else
            {
                scrollRect.Top = 0;
                scrollRect.Bottom = csbi.dwSize.Y - 1 + lines;
                destOrigin.X = 0;
                destOrigin.Y = -lines;
            }

            // Fill the vacated area with blank lines
            clipRect = scrollRect;
            fill.Attributes = csbi.wAttributes;
            fill.Char.AsciiChar = ' ';

            // Do the scroll
            ScrollConsoleScreenBuffer(handle, &scrollRect, &clipRect, destOrigin, &fill);

            // Adjust cursor position
            csbi.dwCursorPosition.Y = csbi.dwCursorPosition.Y - static_cast<SHORT>(lines);
            csbi.dwCursorPosition.Y = std::max<SHORT>(0, std::min<SHORT>(csbi.dwCursorPosition.Y, csbi.dwSize.Y - 1));
            ::SetConsoleCursorPosition(handle, csbi.dwCursorPosition);
        }
    }

    void Console::scrollUp()
    {
        scrollConsole(-1);
    }

    void Console::scrollDown()
    {
        scrollConsole(1);
    }

    void Console::waitForKeyPress()
    {
        _getch();
    }
}
