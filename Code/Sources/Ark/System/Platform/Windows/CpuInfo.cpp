#if defined(ARK_PLATFORM_WINDOWS)

#include "Ark/System/CpuInfo.hpp"

#include <Windows.h>
#include <intrin.h>

namespace Ark::System::CpuInfo
{
    static void cpuid(int info[4], int functionId, int subFunction)
    {
        __cpuidex(info, functionId, subFunction);
    }

    int getNumLogicalCpuCores()
    {
        SYSTEM_INFO si{};
        GetSystemInfo(&si);
        return static_cast<int>(si.dwNumberOfProcessors);
    }

    int getCpuCacheLineSize()
    {
        DWORD len = 0;
        GetLogicalProcessorInformation(nullptr, &len);
        if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
        {
            return 0;
        }

        SYSTEM_LOGICAL_PROCESSOR_INFORMATION* buffer = reinterpret_cast<SYSTEM_LOGICAL_PROCESSOR_INFORMATION*>(::HeapAlloc(GetProcessHeap(), 0, len));
        if (buffer == nullptr)
        {
            return 0;
        }

        DWORD count = len / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
        int lineSize = 0;
        if (GetLogicalProcessorInformation(buffer, &len))
        {
            for (DWORD i = 0; i < count; ++i)
            {
                if (buffer[i].Relationship == RelationCache && buffer[i].Cache.Level == 1)
                {
                    lineSize = static_cast<int>(buffer[i].Cache.LineSize);
                    break;
                }
            }
        }
        ::HeapFree(GetProcessHeap(), 0, buffer);
        return lineSize;
    }

    static bool xgetbvAvxEnabled()
    {
        int info[4] = {};
        cpuid(info, 1, 0);
        bool const osUsesXsaveXrstor = (info[2] & (1 << 27)) != 0; // OSXSAVE
        bool const avxSupport = (info[2] & (1 << 28)) != 0;        // AVX
        if (!(osUsesXsaveXrstor && avxSupport))
        {
            return false;
        }
        unsigned long long const xcr0 = _xgetbv(0);
        return (xcr0 & 0x6) == 0x6; // XMM and YMM state
    }

    bool hasAltiVec()
    {
        return false;
    }
    bool hasMmx()
    {
        int info[4] = {};
        cpuid(info, 1, 0);
        return (info[3] & (1 << 23)) != 0; // MMX
    }
    bool hasSse()
    {
        int info[4] = {};
        cpuid(info, 1, 0);
        return (info[3] & (1 << 25)) != 0; // SSE
    }
    bool hasSse2()
    {
        int info[4] = {};
        cpuid(info, 1, 0);
        return (info[3] & (1 << 26)) != 0; // SSE2
    }
    bool hasSse3()
    {
        int info[4] = {};
        cpuid(info, 1, 0);
        return (info[2] & (1 << 0)) != 0; // SSE3
    }
    bool hasSse41()
    {
        int info[4] = {};
        cpuid(info, 1, 0);
        return (info[2] & (1 << 19)) != 0; // SSE4.1
    }
    bool hasSse42()
    {
        int info[4] = {};
        cpuid(info, 1, 0);
        return (info[2] & (1 << 20)) != 0; // SSE4.2
    }
    bool hasAvx()
    {
        return xgetbvAvxEnabled();
    }
    bool hasAvx2()
    {
        if (!xgetbvAvxEnabled())
        {
            return false;
        }
        int info[4] = {};
        cpuid(info, 7, 0);
        return (info[1] & (1 << 5)) != 0; // AVX2
    }
    bool hasAvx512F()
    {
        if (!xgetbvAvxEnabled())
        {
            return false;
        }
        // Check OPMASK, ZMM_Hi256, Hi16_ZMM enabled in XCR0 (bits 5,6,7)
        unsigned long long const xcr0 = _xgetbv(0);
        if ((xcr0 & 0xE0) != 0xE0)
        {
            return false;
        }
        int info[4] = {};
        cpuid(info, 7, 0);
        return (info[1] & (1 << 16)) != 0; // AVX-512F
    }
    bool hasArmSimd()
    {
        return false;
    }
    bool hasNeon()
    {
        return false;
    }
    bool hasLsx()
    {
        return false;
    }
    bool hasLasx()
    {
        return false;
    }

    int getSystemRamMiB()
    {
        MEMORYSTATUSEX msx{};
        msx.dwLength = sizeof(msx);
        if (GlobalMemoryStatusEx(&msx))
        {
            return static_cast<int>(msx.ullTotalPhys / (1024 * 1024));
        }
        return 0;
    }

    size_t getSimdAlignment()
    {
        if (hasAvx512F())
        {
            return 64;
        }
        if (hasAvx2() || hasAvx())
        {
            return 32;
        }
        if (hasSse())
        {
            return 16;
        }
        return sizeof(void*);
    }

    int getSystemPageSize()
    {
        SYSTEM_INFO si{};
        GetSystemInfo(&si);
        return static_cast<int>(si.dwPageSize);
    }
}

#endif
