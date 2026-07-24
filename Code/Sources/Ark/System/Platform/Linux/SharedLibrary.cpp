#include "Ark/System/SharedLibrary.hpp"

#include <dlfcn.h>

namespace Ark::System
{
    Result<SharedLibraryHandle, String> loadSharedLibrary(StringSlice const path)
    {
        void* const handle = dlopen(path.asPointer(), RTLD_LAZY | RTLD_LOCAL);

        if (handle == nullptr)
        {
            char const* const error = dlerror();
            return Result<SharedLibraryHandle, String>(unexpectedResult, error);
        }

        return handle;
    }

    void unloadSharedLibrary(SharedLibraryHandle handle)
    {
        if (handle != nullptr)
        {
            dlclose(handle);
        }
    }

    void* getSharedLibrarySymbol(SharedLibraryHandle handle, StringSlice const symbolName)
    {
        if (handle == nullptr)
        {
            return nullptr;
        }

        dlerror();

        void* symbol = dlsym(handle, symbolName.asPointer());

        char const* error = dlerror();
        if (error != nullptr)
        {
            return nullptr;
        }

        return symbol;
    }

    String getSharedLibraryExtension()
    {
        return "so";
    }
}
