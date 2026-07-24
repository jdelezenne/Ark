#include "Ark/Core/Debug/Assert.hpp"

#include <cstdlib>
#include <iostream>

namespace Ark
{
    namespace
    {
        AssertHandler assertHandler = nullptr;
    }

    AssertHandler getAssertHandler()
    {
        return assertHandler;
    }

    void setAssertHandler(AssertHandler value)
    {
        assertHandler = value;
    }

    AssertAction defaultAssertHandler(const char* expression, const char* message, const char* fileName, uint lineNumber)
    {
        std::cerr << "ARK Assertion failed: " << ((expression != nullptr) ? expression : "Unconditional") << "\n"
                  << "Message: " << ((message != nullptr) ? message : "No message") << "\n"
                  << "File: " << fileName << "\n"
                  << "Line: " << lineNumber << "\n";

        std::cerr << "Choose action:\n"
                  << "1. Abort\n"
                  << "2. Break (attach debugger)\n"
                  << "3. Continue\n"
                  << "Enter choice (1-3): ";

        int choice;
        std::cin >> choice;

        switch (choice)
        {
            case 1:
                return AssertAction::Abort;

            case 2:
                return AssertAction::Break;

            case 3:
                return AssertAction::Continue;

            default:
                return AssertAction::Abort;
        }
    }

    [[noreturn]]
    void fatalError(const char* message, const char* fileName, uint lineNumber)
    {
        std::cerr << "ARK Fatal error: " << message << "\n"
                  << "File: " << fileName << "\n"
                  << "Line: " << lineNumber << "\n";

        ARK_ABORT();
    }

    AssertAction handleAssert(bool condition, char const* expression, char const* message, char const* fileName, uint lineNumber)
    {
        if (condition)
        {
            return AssertAction::Continue;
        }

        AssertAction action = AssertAction::Abort;

        if (assertHandler != nullptr)
        {
            return assertHandler(expression, message, fileName, lineNumber);
        }
        else
        {
            return defaultAssertHandler(expression, message, fileName, lineNumber);
        }
    }
}
