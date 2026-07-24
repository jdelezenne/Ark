#pragma once

#include "Ark/Core/Result.hpp"
#include "Ark/Storage/Path.hpp"
#include "Ark/Strings/StringSlice.hpp"

namespace Ark::System
{
    using SharedLibraryHandle = void*;

    /// Load a shared library from the given path.
    /// @param path The path to the shared library file
    /// @return A handle to the library on success, nullptr on failure
    Result<SharedLibraryHandle, String> loadSharedLibrary(StringSlice const path);

    inline Result<SharedLibraryHandle, String> loadSharedLibrary(Storage::Path const& path)
    {
        return loadSharedLibrary(StringSlice(path.string()));
    }

    /// Free a previously loaded shared library.
    /// @param handle The library handle to free (safe to call with nullptr)
    void unloadSharedLibrary(SharedLibraryHandle handle);

    /// Get the address of a symbol in a loaded shared library.
    /// @param handle The library handle
    /// @param symbolName The name of the symbol to find
    /// @return Symbol address on success, nullptr if symbol not found or handle invalid
    void* getSharedLibrarySymbol(SharedLibraryHandle handle, StringSlice const symbolName);

    /// Get the platform-specific shared library file extension (without leading dot).
    /// @return "dll" on Windows, "dylib" on macOS, "so" on Linux
    String getSharedLibraryExtension();
}
