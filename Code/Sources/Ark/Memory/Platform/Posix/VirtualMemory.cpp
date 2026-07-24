#include "Ark/Memory/VirtualMemory.hpp"

#include <sys/mman.h>
#include <unistd.h>

namespace Ark::Memory
{
    static usize getPageSize()
    {
        long const pageSize = ::sysconf(_SC_PAGESIZE);
        return pageSize > 0 ? static_cast<usize>(pageSize) : static_cast<usize>(4096);
    }

    static usize alignToPage(usize size)
    {
        usize const pageSize = getPageSize();
        return (size + pageSize - 1) & ~(pageSize - 1);
    }

    static int toPosixProt(Protect p)
    {
        switch (p)
        {
            case Protect::NoAccess:
                return PROT_NONE;
            case Protect::Read:
                return PROT_READ;
            case Protect::Write:
                return PROT_WRITE;
            case Protect::Execute:
                return PROT_EXEC;
            case Protect::ReadWrite:
                return PROT_READ | PROT_WRITE;
            case Protect::ReadExecute:
                return PROT_READ | PROT_EXEC;
            case Protect::ReadWriteExecute:
                return PROT_READ | PROT_WRITE | PROT_EXEC;
        }
        return PROT_READ | PROT_WRITE;
    }

    void* reserve(usize size, usize alignment)
    {
        if (size == 0)
        {
            return nullptr;
        }

        usize const pageSize = getPageSize();
        if (alignment < pageSize)
        {
            alignment = pageSize;
        }

        // mmap is page-granular. Request enough space to satisfy alignment, then keep an aligned window.
        usize const mappedSize = alignToPage(size + alignment);
        if (mappedSize < size)
        {
            return nullptr;
        }

        void* memory = ::mmap(nullptr, mappedSize, PROT_NONE, MAP_PRIVATE | MAP_ANON, -1, 0);
        if (memory == MAP_FAILED)
        {
            return nullptr;
        }

        uintptr const base = reinterpret_cast<uintptr>(memory);
        uintptr const aligned = (base + alignment - 1) & ~(static_cast<uintptr>(alignment) - 1);
        usize const leading = static_cast<usize>(aligned - base);
        usize const kept = alignToPage(size);
        usize const trailing = mappedSize - leading - kept;

        if (leading != 0)
        {
            ::munmap(memory, leading);
        }
        if (trailing != 0)
        {
            ::munmap(reinterpret_cast<void*>(aligned + kept), trailing);
        }

        return reinterpret_cast<void*>(aligned);
    }

    bool commit(void* base, usize size, Protect protection)
    {
        if (base == nullptr || size == 0)
        {
            return false;
        }

        return ::mprotect(base, alignToPage(size), toPosixProt(protection)) == 0;
    }

    bool decommit(void* base, usize size)
    {
        if (base == nullptr || size == 0)
        {
            return false;
        }

        size = alignToPage(size);

        // Approximate Windows MEM_DECOMMIT: drop pages and fault on access until recommit.
        if (::mprotect(base, size, PROT_NONE) != 0)
        {
            return false;
        }

#if defined(MADV_DONTNEED)
        return ::madvise(base, size, MADV_DONTNEED) == 0;
#else
        return true;
#endif
    }

    bool release(void* base, usize size)
    {
        if (base == nullptr || size == 0)
        {
            return false;
        }

        return ::munmap(base, alignToPage(size)) == 0;
    }

    bool protect(void* base, usize size, Protect protection)
    {
        if (base == nullptr || size == 0)
        {
            return false;
        }

        return ::mprotect(base, alignToPage(size), toPosixProt(protection)) == 0;
    }
}
