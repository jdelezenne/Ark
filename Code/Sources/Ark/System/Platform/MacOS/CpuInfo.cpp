#include "Ark/System/CpuInfo.hpp"

#include <sys/sysctl.h>
#include <unistd.h>

namespace Ark::System::CpuInfo
{
    int getNumLogicalCpuCores()
    {
        int count = 0;
        size_t size = sizeof(count);
        if (sysctlbyname("hw.logicalcpu", &count, &size, nullptr, 0) == 0 && count > 0)
        {
            return count;
        }
        long n = sysconf(_SC_NPROCESSORS_ONLN);
        return static_cast<int>(n > 0 ? n : 0);
    }

    int getCpuCacheLineSize()
    {
        size_t lineSize = 0;
        size_t size = sizeof(lineSize);
        if (sysctlbyname("hw.cachelinesize", &lineSize, &size, nullptr, 0) == 0)
        {
            return static_cast<int>(lineSize);
        }
        return 0;
    }

    bool hasAltiVec()
    {
        int altivec = 0;
        size_t size = sizeof(altivec);
        if (sysctlbyname("hw.optional.altivec", &altivec, &size, nullptr, 0) == 0)
        {
            return altivec != 0;
        }
        return false;
    }

    bool hasMmx()
    {
        return false;
    }
    bool hasSse()
    {
        int sse = 0;
        size_t size = sizeof(sse);
        return sysctlbyname("hw.optional.sse", &sse, &size, nullptr, 0) == 0 && sse != 0;
    }
    bool hasSse2()
    {
        int v = 0;
        size_t size = sizeof(v);
        return sysctlbyname("hw.optional.sse2", &v, &size, nullptr, 0) == 0 && v != 0;
    }
    bool hasSse3()
    {
        int v = 0;
        size_t size = sizeof(v);
        return sysctlbyname("hw.optional.sse3", &v, &size, nullptr, 0) == 0 && v != 0;
    }
    bool hasSse41()
    {
        int v = 0;
        size_t size = sizeof(v);
        return sysctlbyname("hw.optional.sse4_1", &v, &size, nullptr, 0) == 0 && v != 0;
    }
    bool hasSse42()
    {
        int v = 0;
        size_t size = sizeof(v);
        return sysctlbyname("hw.optional.sse4_2", &v, &size, nullptr, 0) == 0 && v != 0;
    }
    bool hasAvx()
    {
        int v = 0;
        size_t size = sizeof(v);
        return sysctlbyname("hw.optional.avx1_0", &v, &size, nullptr, 0) == 0 && v != 0;
    }
    bool hasAvx2()
    {
        int v = 0;
        size_t size = sizeof(v);
        return sysctlbyname("hw.optional.avx2_0", &v, &size, nullptr, 0) == 0 && v != 0;
    }
    bool hasAvx512F()
    {
        int v = 0;
        size_t size = sizeof(v);
        return sysctlbyname("hw.optional.avx512f", &v, &size, nullptr, 0) == 0 && v != 0;
    }
    bool hasArmSimd()
    {
        int v = 0;
        size_t size = sizeof(v);
        return sysctlbyname("hw.optional.neon", &v, &size, nullptr, 0) == 0 && v != 0;
    }
    bool hasNeon()
    {
        int v = 0;
        size_t size = sizeof(v);
        return sysctlbyname("hw.optional.neon", &v, &size, nullptr, 0) == 0 && v != 0;
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
        long long mem = 0;
        size_t size = sizeof(mem);
        if (sysctlbyname("hw.memsize", &mem, &size, nullptr, 0) == 0 && mem > 0)
        {
            return static_cast<int>(mem / (1024LL * 1024LL));
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
        if (hasNeon())
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
