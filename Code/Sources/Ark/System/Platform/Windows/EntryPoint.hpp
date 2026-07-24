#pragma once

#if defined(ARK_PLATFORM_WINDOWS)

#include "Ark/Core/Platform/Windows/Platform.hpp"

#include <shellapi.h>
#include <stringapiset.h>

int WinMain(
    [[maybe_unused]] HINSTANCE hInstance,
    [[maybe_unused]] HINSTANCE hPrevInstance,
    [[maybe_unused]] LPSTR lpCmdLine,
    [[maybe_unused]] int nCmdShow)
{
    using namespace Ark;

    int argc;
    LPWSTR* wideArgs = CommandLineToArgvW(GetCommandLineW(), &argc);

    Collections::Array<String> arguments;

    if (wideArgs != nullptr)
    {
        for (int i = 0; i < argc; ++i)
        {
            int size = WideCharToMultiByte(CP_UTF8, 0, wideArgs[i], -1, nullptr, 0, nullptr, nullptr);

            if (size > 0)
            {
                Collections::Array<char> buffer(static_cast<uint>(size));
                WideCharToMultiByte(CP_UTF8, 0, wideArgs[i], -1, buffer.asMutablePointer(), size, nullptr, nullptr);

                // Remove null terminator
                buffer.removeLast();
                arguments.append(String(buffer.asSlice()));
            }
        }

        LocalFree(wideArgs);
    }

    return arkMain(arguments) ? 0 : 1;
}

#endif
