#include "Ark/System/Console.hpp"
#include "Ark/System/EntryPoint.hpp"

using namespace Ark;
using namespace Ark::System;

bool arkMain(Ark::Collections::Array<Ark::String> const&)
{
    Console::print("Hello, ");
    Console::println("System Console!");

    Console::printlnColored("Info message", Console::Color::BrightCyan);
    Console::printlnColored("Warning message", Console::Color::Yellow);
    Console::printlnColored("Error message", Console::Color::BrightRed);

    // Formatted printing examples
    int numAssets = 5;
    float64 elapsedMs = 12.345;
    Console::printlnFormat("Loaded {} assets in {:.2f} ms", numAssets, elapsedMs);

    float fps = 144.0f;
    Console::printlnFormatColored(Console::Color::BrightGreen, "FPS: {:.1f}", fps);

    Console::printlnFormatStyled(Console::Style::Bold, "User {} logged in", "admin");
    Console::printlnFormatColoredStyled(Console::Color::Yellow, Console::Color::Default, Console::Style::Italic, "Level {}: {}", 3, "Forest");

    // Alignment and width
    Console::printlnFormat("|{:>8}|{:^8}|{:<8}|", "right", "center", "left");
    Console::printlnFormatColored(Console::Color::Black, Console::Color::BrightYellow, "Foreground {} on {}", "black", "bright yellow");

    Console::printlnFormatted(
        "Styled text (italic blue on white)",
        Console::Color::Blue,
        Console::Color::White,
        Console::Style::Italic);

    Console::printlnStyled("Bold line", Console::Style::Bold);
    Console::printlnStyled("Underlined line", Console::Style::Underline);

    Console::println("");
    Console::println("Clearing screen and moving cursor...");
    Console::flush();

    Console::waitForKeyPress();

    Console::clearScreen();
    Console::moveCursor(1, 1);
    Console::println("Top-left after clear");

    Console::saveCursorPosition();
    Console::println("This line will be replaced");

    Console::waitForKeyPress();

    Console::restoreCursorPosition();
    Console::clearLine();
    Console::println("Replaced line");

    Console::flush();
    return true;
}
