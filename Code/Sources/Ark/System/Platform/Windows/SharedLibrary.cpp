#include "Ark/System/SharedLibrary.hpp"
#include "Ark/Strings/Unicode.hpp"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace Ark::System
{
    Result<SharedLibraryHandle, String> loadSharedLibrary(StringSlice const path)
    {
        auto widePath = Unicode::toWide(path);
        HMODULE const handle = LoadLibraryW(widePath.asPointer());

        if (handle == nullptr)
        {
            DWORD const error = GetLastError();
            return Result<SharedLibraryHandle, String>(unexpectedResult, String::format("Failed to load library: {}", static_cast<int>(error)));
        }

        return static_cast<SharedLibraryHandle>(handle);
    }

    void unloadSharedLibrary(SharedLibraryHandle handle)
    {
        if (handle != nullptr)
        {
            HMODULE const hmodule = static_cast<HMODULE>(handle);
            FreeLibrary(hmodule);
        }
    }

    void* getSharedLibrarySymbol(SharedLibraryHandle handle, StringSlice const symbolName)
    {
        if (handle == nullptr)
        {
            return nullptr;
        }

        HMODULE const hmodule = static_cast<HMODULE>(handle);
        return GetProcAddress(hmodule, symbolName.asPointer());
    }

    String getSharedLibraryExtension()
    {
        return "dll";
    }
}
