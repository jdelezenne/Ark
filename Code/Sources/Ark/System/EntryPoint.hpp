#pragma once

#include "Ark/Collections/Array.hpp"
#include "Ark/Core/Types.hpp"
#include "Ark/Strings/String.hpp"

/// Application entry function implemented by the program.
/// @param arguments Command-line arguments as UTF-8 strings.
/// @return True on success, false on failure.
bool arkMain(Ark::Collections::Array<Ark::String> const& arguments);

#if defined(ARK_PLATFORM_WINDOWS)

#include "Ark/System/Platform/Windows/EntryPoint.hpp"

#endif

#if !defined(ARK_PLATFORM_WINDOWS) || !defined(ARK_PLATFORM_WINDOWS_SUBSYSTEM)

/// Platform entry point forwarding to arkMain.
int main(int argc, char* argv[])
{
    using namespace Ark;
    Collections::Array<String> arguments(argv, argv + argc);
    return arkMain(arguments) ? 0 : 1;
}

#endif
