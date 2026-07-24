#if defined(ARK_PLATFORM_WEBASSEMBLY)

#include "Ark/System/Debug/Stacktrace.hpp"

namespace Ark
{
    Collections::Array<StacktraceEntry> Stacktrace::capture(uint32 /*skip*/, uint32 /*maxDepth*/)
    {
        // Browsers do not expose a portable native backtrace API to C++.
        return {};
    }
}

#endif
