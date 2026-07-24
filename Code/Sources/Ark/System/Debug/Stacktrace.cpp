#include "Ark/System/Debug/Stacktrace.hpp"

#include "Ark/Strings/String.hpp"

namespace Ark
{
    StacktraceEntry::StacktraceEntry(StringSlice functionName, StringSlice fileName, uint32 lineNumber, UIntPtr address)
        : functionName{}
        , fileName{}
        , lineNumber(lineNumber)
        , address(address)
    {
        this->functionName = functionName.isEmpty() ? Ark::String() : Ark::String(functionName.asPointer(), functionName.getLength());
        this->fileName = fileName.isEmpty() ? Ark::String() : Ark::String(fileName.asPointer(), fileName.getLength());
    }

    StringSlice StacktraceEntry::getFunctionName() const
    {
        return functionName;
    }

    StringSlice StacktraceEntry::getFileName() const
    {
        return fileName;
    }

    uint32 StacktraceEntry::getLineNumber() const
    {
        return lineNumber;
    }

    UIntPtr StacktraceEntry::getAddress() const
    {
        return address;
    }
}
