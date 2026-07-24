#include "Ark/Strings/String.hpp"
#include "Ark/System/Console.hpp"
#include "Ark/System/CpuInfo.hpp"
#include "Ark/System/EntryPoint.hpp"

using Ark::System::Console;
namespace CpuInfo = Ark::System::CpuInfo;

static void printSectionHeader(Ark::StringSlice title)
{
    Console::println("");
    Console::printlnColored(title, Console::Color::BrightCyan);
}

static void printKvp(Ark::StringSlice key, Ark::StringSlice value)
{
    Console::printColored(key, Console::Color::BrightYellow);
    Console::print(": ");
    Console::println(value);
}

template <typename T>
static void printKvpNumber(Ark::StringSlice key, T value)
{
    auto text = Ark::String::format("{}", value);
    printKvp(key, text);
}

bool arkMain(Ark::Collections::Array<Ark::String> const&)
{
    printSectionHeader("CPU");
    {
        printKvpNumber("Logical Cores", CpuInfo::getNumLogicalCpuCores());
        printKvpNumber("L1 Cache Line Size", CpuInfo::getCpuCacheLineSize());

        auto yesNo = [](bool v)
        {
            return Ark::String(v ? "Yes" : "No");
        };

        printKvp("MMX", yesNo(CpuInfo::hasMmx()));
        printKvp("SSE", yesNo(CpuInfo::hasSse()));
        printKvp("SSE2", yesNo(CpuInfo::hasSse2()));
        printKvp("SSE3", yesNo(CpuInfo::hasSse3()));
        printKvp("SSE4.1", yesNo(CpuInfo::hasSse41()));
        printKvp("SSE4.2", yesNo(CpuInfo::hasSse42()));
        printKvp("AVX", yesNo(CpuInfo::hasAvx()));
        printKvp("AVX2", yesNo(CpuInfo::hasAvx2()));
        printKvp("AVX-512F", yesNo(CpuInfo::hasAvx512F()));
        printKvp("AltiVec", yesNo(CpuInfo::hasAltiVec()));
        printKvp("ARM SIMD", yesNo(CpuInfo::hasArmSimd()));
        printKvp("NEON", yesNo(CpuInfo::hasNeon()));
        printKvp("LSX", yesNo(CpuInfo::hasLsx()));
        printKvp("LASX", yesNo(CpuInfo::hasLasx()));
        printKvpNumber("SIMD Alignment", CpuInfo::getSimdAlignment());
        printKvpNumber("System Page Size", CpuInfo::getSystemPageSize());
        printKvpNumber("System RAM (MiB)", CpuInfo::getSystemRamMiB());
    }

    Console::flush();
    return true;
}
