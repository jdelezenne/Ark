#if defined(ARK_PLATFORM_LINUX)

#include "Ark/System/CpuInfo.hpp"

#include <cstdio>
#include <cstring>
#include <sys/auxv.h>
#include <sys/sysinfo.h>
#include <unistd.h>

namespace Ark::System::CpuInfo
{
    static bool readFlagFromProcCpuInfo(char const* flag)
    {
        FILE* f = fopen("/proc/cpuinfo", "r");
        if (f == nullptr)
        {
            return false;
        }
        char line[2048] = {};
        bool found = false;
        while (fgets(line, sizeof(line), f) != nullptr)
        {
            if (strncmp(line, "flags", 5) == 0 || strncmp(line, "Features", 8) == 0)
            {
                if (strstr(line, flag) != nullptr)
                {
                    found = true;
                    break;
                }
            }
        }
        fclose(f);
        return found;
    }

    int getNumLogicalCpuCores()
    {
        long n = sysconf(_SC_NPROCESSORS_ONLN);
        return static_cast<int>(n > 0 ? n : 0);
    }

    int getCpuCacheLineSize()
    {
        long size = sysconf(_SC_LEVEL1_DCACHE_LINESIZE);
        return static_cast<int>(size > 0 ? size : 0);
    }

    bool hasAltiVec()
    {
        return readFlagFromProcCpuInfo("altivec");
    }
    bool hasMmx()
    {
        return readFlagFromProcCpuInfo("mmx");
    }
    bool hasSse()
    {
        return readFlagFromProcCpuInfo("sse ") || readFlagFromProcCpuInfo(" sse");
    }
    bool hasSse2()
    {
        return readFlagFromProcCpuInfo("sse2");
    }
    bool hasSse3()
    {
        return readFlagFromProcCpuInfo("sse3");
    }
    bool hasSse41()
    {
        return readFlagFromProcCpuInfo("sse4_1");
    }
    bool hasSse42()
    {
        return readFlagFromProcCpuInfo("sse4_2");
    }
    bool hasAvx()
    {
        return readFlagFromProcCpuInfo("avx");
    }
    bool hasAvx2()
    {
        return readFlagFromProcCpuInfo("avx2");
    }
    bool hasAvx512F()
    {
        return readFlagFromProcCpuInfo("avx512f");
    }
    bool hasArmSimd()
    {
        return readFlagFromProcCpuInfo("asimd") || readFlagFromProcCpuInfo("edsp");
    }
    bool hasNeon()
    {
        return readFlagFromProcCpuInfo("neon") || readFlagFromProcCpuInfo("asimd");
    }
    bool hasLsx()
    {
        return readFlagFromProcCpuInfo("lsx");
    }
    bool hasLasx()
    {
        return readFlagFromProcCpuInfo("lasx");
    }

    int getSystemRamMiB()
    {
        struct sysinfo si{};
        if (sysinfo(&si) == 0)
        {
            return static_cast<int>((si.totalram * si.mem_unit) / (1024 * 1024));
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
        if (hasSse() || hasNeon())
        {
            return 16;
        }
        return sizeof(void*);
    }

    int getSystemPageSize()
    {
        long ps = sysconf(_SC_PAGESIZE);
        return static_cast<int>(ps > 0 ? ps : 0);
    }
}

#endif
