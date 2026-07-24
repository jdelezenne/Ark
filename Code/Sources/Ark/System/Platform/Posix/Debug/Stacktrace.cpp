#include "Ark/System/Debug/Stacktrace.hpp"

#if (defined(ARK_PLATFORM_MACOS) || defined(ARK_PLATFORM_LINUX)) && !defined(ARK_PLATFORM_WEBASSEMBLY)
#include <cstring>
#include <cxxabi.h>
#include <dlfcn.h>
#include <execinfo.h>

namespace Ark
{
    Ark::Collections::Array<StacktraceEntry> Stacktrace::capture(uint32 skip, uint32 maxDepth)
    {
        Ark::Collections::Array<StacktraceEntry> results;

        void* callstack[256];
        int captured = ::backtrace(callstack, static_cast<int>(Ark::min<uint32>(maxDepth + skip + 1, 256)));

        for (int i = static_cast<int>(skip + 1); i < captured; ++i)
        {
            Dl_info info;
            std::memset(&info, 0, sizeof(info));

            StringSlice functionName;
            StringSlice fileName;

            if (::dladdr(callstack[i], &info) != 0)
            {
                if (info.dli_sname != nullptr)
                {
                    int status = 0;
                    char* demangled = abi::__cxa_demangle(info.dli_sname, nullptr, nullptr, &status);
                    if (status == 0 && demangled != nullptr)
                    {
                        functionName = StringSlice(demangled);
                        std::free(demangled);
                    }
                    else
                    {
                        functionName = StringSlice(info.dli_sname);
                    }
                }

                if (info.dli_fname != nullptr)
                {
                    fileName = StringSlice(info.dli_fname);
                }
            }

            UIntPtr addr = reinterpret_cast<UIntPtr>(callstack[i]);
            results.append(StacktraceEntry(functionName, fileName, 0, addr));
        }

        return results;
    }
}

#endif
