#include "Ark/System/Debug/Stacktrace.hpp"

#if defined(ARK_PLATFORM_WINDOWS)
#include <dbghelp.h>
#include <windows.h>
#pragma comment(lib, "dbghelp.lib")

namespace Ark
{
    Ark::Collections::Array<StacktraceEntry> Stacktrace::capture(uint32 skip, uint32 maxDepth)
    {
        Ark::Collections::Array<StacktraceEntry> results;

        HANDLE process = ::GetCurrentProcess();
        HANDLE thread = ::GetCurrentThread();

        CONTEXT context;
        std::memset(&context, 0, sizeof(CONTEXT));
        context.ContextFlags = CONTEXT_FULL;
        ::RtlCaptureContext(&context);

        ::SymSetOptions(SYMOPT_DEFERRED_LOADS | SYMOPT_UNDNAME | SYMOPT_LOAD_LINES);
        ::SymInitialize(process, nullptr, TRUE);

        STACKFRAME64 frame;
        std::memset(&frame, 0, sizeof(STACKFRAME64));

#if defined(ARK_ARCHITECTURE_X64)
        DWORD machineType = IMAGE_FILE_MACHINE_AMD64;
        frame.AddrPC.Offset = context.Rip;
        frame.AddrPC.Mode = AddrModeFlat;
        frame.AddrFrame.Offset = context.Rbp;
        frame.AddrFrame.Mode = AddrModeFlat;
        frame.AddrStack.Offset = context.Rsp;
        frame.AddrStack.Mode = AddrModeFlat;
#elif defined(ARK_ARCHITECTURE_X86)
        DWORD machineType = IMAGE_FILE_MACHINE_I386;
        frame.AddrPC.Offset = context.Eip;
        frame.AddrPC.Mode = AddrModeFlat;
        frame.AddrFrame.Offset = context.Ebp;
        frame.AddrFrame.Mode = AddrModeFlat;
        frame.AddrStack.Offset = context.Esp;
        frame.AddrStack.Mode = AddrModeFlat;
#elif defined(ARK_ARCHITECTURE_ARM64)
        DWORD machineType = IMAGE_FILE_MACHINE_ARM64;
        frame.AddrPC.Offset = context.Pc;
        frame.AddrPC.Mode = AddrModeFlat;
        frame.AddrFrame.Offset = context.Fp;
        frame.AddrFrame.Mode = AddrModeFlat;
        frame.AddrStack.Offset = context.Sp;
        frame.AddrStack.Mode = AddrModeFlat;
#else
        DWORD machineType = 0;
#endif

        for (uint32 i = 0; i < maxDepth + skip; ++i)
        {
            BOOL ok = ::StackWalk64(
                machineType,
                process,
                thread,
                &frame,
                &context,
                nullptr,
                ::SymFunctionTableAccess64,
                ::SymGetModuleBase64,
                nullptr);

            if (ok == FALSE)
            {
                break;
            }

            if (i >= skip)
            {
                char symbolBuffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
                PSYMBOL_INFO symbol = reinterpret_cast<PSYMBOL_INFO>(symbolBuffer);
                symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
                symbol->MaxNameLen = MAX_SYM_NAME;

                DWORD64 displacement = 0;
                BOOL symOk = ::SymFromAddr(process, frame.AddrPC.Offset, &displacement, symbol);

                if (symOk == TRUE)
                {
                    IMAGEHLP_LINE64 line;
                    std::memset(&line, 0, sizeof(line));
                    line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
                    DWORD displacementLine = 0;

                    BOOL lineOk = ::SymGetLineFromAddr64(process, frame.AddrPC.Offset, &displacementLine, &line);

                    char undecorated[MAX_SYM_NAME];
                    std::memset(undecorated, 0, sizeof(undecorated));
                    ::UnDecorateSymbolName(symbol->Name, undecorated, static_cast<DWORD>(sizeof(undecorated)), UNDNAME_COMPLETE);

                    char const* function = (undecorated[0] != '\0') ? undecorated : symbol->Name;

                    if (lineOk == TRUE)
                    {
                        results.append(StacktraceEntry(StringSlice(function), StringSlice(line.FileName), static_cast<uint32>(line.LineNumber), static_cast<UIntPtr>(frame.AddrPC.Offset)));
                    }
                    else
                    {
                        results.append(StacktraceEntry(StringSlice(function), StringSlice(), 0, static_cast<UIntPtr>(frame.AddrPC.Offset)));
                    }
                }
            }
        }

        ::SymCleanup(process);
        return results;
    }
}

#endif
