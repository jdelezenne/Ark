#pragma once

#include "Ark/Collections/Array.hpp"
#include "Ark/Core/Types.hpp"
#include "Ark/Strings/String.hpp"
#include "Ark/Strings/StringSlice.hpp"

namespace Ark
{
    struct StacktraceEntry final
    {
    public:
        StacktraceEntry(StringSlice functionName, StringSlice fileName, uint32 lineNumber, UIntPtr address);

        StringSlice getFunctionName() const;
        StringSlice getFileName() const;
        uint32 getLineNumber() const;
        UIntPtr getAddress() const;

    private:
        Ark::String functionName;
        Ark::String fileName;
        uint32 lineNumber;
        UIntPtr address;
    };

    struct Stacktrace final
    {
    public:
        static Ark::Collections::Array<StacktraceEntry> capture(uint32 skip = 1, uint32 maxDepth = 32);
    };
}
