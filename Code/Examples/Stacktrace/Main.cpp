#include "Ark/Collections/Array.hpp"
#include "Ark/Strings/StringSlice.hpp"
#include "Ark/System/Console.hpp"
#include "Ark/System/Debug/Stacktrace.hpp"
#include "Ark/System/EntryPoint.hpp"

namespace Example
{
    static void inner(Ark::Collections::Array<Ark::StacktraceEntry>& out)
    {
        auto frames = Ark::Stacktrace::capture(1, 32);
        out = Ark::move(frames);
    }

    static void middle(Ark::Collections::Array<Ark::StacktraceEntry>& out)
    {
        inner(out);
    }
}

bool arkMain(Ark::Collections::Array<Ark::String> const&)
{
    Ark::Collections::Array<Ark::StacktraceEntry> frames;
    Example::middle(frames);

    Ark::System::Console::printlnf("Captured %zu frames", static_cast<size_t>(frames.getCount()));

    for (Ark::usize i = 0; i < frames.getCount(); ++i)
    {
        Ark::StacktraceEntry const& entry = frames.get(i);
        Ark::StringSlice fn = entry.getFunctionName();
        Ark::StringSlice file = entry.getFileName();
        unsigned line = entry.getLineNumber();
        auto addr = static_cast<unsigned long long>(entry.getAddress());

        Ark::System::Console::printlnf("#%zu %s (%s:%u) [0x%llx]",
                                       static_cast<size_t>(i),
                                       fn.asPointer(),
                                       file.asPointer(),
                                       line,
                                       addr);
    }

    return true;
}
