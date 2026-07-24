#pragma once

#include <cstddef>

namespace Ark::System::CpuInfo
{
    /// A conservative cache line size for padding purposes.
    constexpr int CpuCacheLineSize = 128;

    /// Get the number of logical CPU cores available.
    int getNumLogicalCpuCores();

    /// Determine the L1 cache line size of the CPU, in bytes.
    int getCpuCacheLineSize();

    /// CPU feature detection queries (return false on architectures where not applicable).
    /// @{
    /// @return True if the CPU supports the feature.

    bool hasAltiVec(); ///< AltiVec SIMD extension
    bool hasMmx();     ///< MMX extension
    bool hasSse();     ///< SSE extension
    bool hasSse2();    ///< SSE2 extension
    bool hasSse3();    ///< SSE3 extension
    bool hasSse41();   ///< SSE4.1 extension
    bool hasSse42();   ///< SSE4.2 extension
    bool hasAvx();     ///< AVX extension
    bool hasAvx2();    ///< AVX2 extension
    bool hasAvx512F(); ///< AVX-512 Foundation extension
    bool hasArmSimd(); ///< ARM SIMD extension
    bool hasNeon();    ///< ARM NEON extension
    bool hasLsx();     ///< LoongArch SX SIMD extension
    bool hasLasx();    ///< LoongArch ASX SIMD extension
    /// @}

    /// Get the amount of RAM configured in the system in MiB.
    int getSystemRamMiB();

    /// Report the alignment, in bytes, needed for SIMD allocations.
    size_t getSimdAlignment();

    /// Report the size of a page of memory in bytes, or 0 if unknown.
    int getSystemPageSize();
}
