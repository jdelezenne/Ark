#include "Ark/Memory/VirtualMemory.hpp"
#include "Ark/Core/Configuration.hpp"

#if ARK_PLATFORM_WINDOWS

#include <windows.h>

namespace Ark::Memory
{
    static DWORD toWinProt(Protect value)
    {
        switch (value)
        {
            case Protect::NoAccess:
                return PAGE_NOACCESS;

            case Protect::Read:
                return PAGE_READONLY;

            case Protect::Write:
                return PAGE_READWRITE; // Windows has no write-only; use RW

            case Protect::Execute:
                return PAGE_EXECUTE;

            case Protect::ReadWrite:
                return PAGE_READWRITE;

            case Protect::ReadExecute:
                return PAGE_EXECUTE_READ;

            case Protect::ReadWriteExecute:
                return PAGE_EXECUTE_READWRITE;

            default:
                return PAGE_READWRITE;
        }
    }

    void* reserve(usize size, [[maybe_unused]] usize alignment)
    {
        return VirtualAlloc(nullptr, size, MEM_RESERVE, PAGE_NOACCESS);
    }

    bool commit(void* base, usize size, Protect protection)
    {
        return VirtualAlloc(base, size, MEM_COMMIT, toWinProt(protection)) != nullptr;
    }

    bool decommit(void* base, usize size)
    {
        return VirtualFree(base, size, MEM_DECOMMIT) != 0;
    }

    bool release(void* base, usize)
    {
        return VirtualFree(base, 0, MEM_RELEASE) != 0;
    }

    bool protect(void* base, usize size, Protect protection)
    {
        DWORD oldProt;
        return VirtualProtect(base, size, toWinProt(protection), &oldProt) != 0;
    }
}

#endif
