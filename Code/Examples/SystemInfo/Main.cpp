#include "Ark/Strings/String.hpp"
#include "Ark/System/Console.hpp"
#include "Ark/System/EntryPoint.hpp"
#include "Ark/System/Environment.hpp"
#include "Ark/System/KnownFolders.hpp"
#include "Ark/System/Process.hpp"
#include "Ark/System/SystemInfo.hpp"

using namespace Ark;
using namespace Ark::System;

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

static Ark::String formatBytes(Ark::uint64 bytes)
{
    const char* units[] = {"B", "KiB", "MiB", "GiB", "TiB"};
    float64 size = static_cast<float64>(bytes);
    int unitIndex = 0;
    while (size >= 1024.0 && unitIndex < 4)
    {
        size /= 1024.0;
        ++unitIndex;
    }
    return Ark::String::format("{:.2f} {}", size, units[unitIndex]);
}

static void printEnvPath()
{
    auto pathResult = getEnvironmentVariable("PATH");
    if (pathResult)
    {
        auto path = pathResult.getValue();
        printKvp("PATH", path);
    }
    else
    {
        printKvp("PATH", Ark::String("<unavailable>"));
    }
}

static void printKnownFolders()
{
    using Ark::System::KnownFolder;
    struct Item
    {
        KnownFolder folder;
        const char* name;
    } items[] = {
        {KnownFolder::Home, "Home"},
        {KnownFolder::Desktop, "Desktop"},
        {KnownFolder::Documents, "Documents"},
        {KnownFolder::Downloads, "Downloads"},
        {KnownFolder::Music, "Music"},
        {KnownFolder::Pictures, "Pictures"},
        {KnownFolder::Videos, "Videos"},
        {KnownFolder::AppDataLocal, "AppDataLocal"},
        {KnownFolder::AppDataRoaming, "AppDataRoaming"},
        {KnownFolder::Temp, "Temp"},
    };
    for (auto const& it : items)
    {
        auto p = Ark::System::getKnownFolderPath(it.folder);
        printKvp(it.name, p.string());
    }
}

bool arkMain(Ark::Collections::Array<Ark::String> const& args)
{
    printSectionHeader("System");
    {
        auto osName = SystemInfo::getOsName();
        auto osVersion = SystemInfo::getOsVersion();
        auto computerName = SystemInfo::getComputerName();
        auto userName = SystemInfo::getUserName();
        auto processName = SystemInfo::getCurrentProcessName();
        auto dt = SystemInfo::getLocalDateTime();
        auto locale = SystemInfo::getLocale();
        auto theme = SystemInfo::getAppTheme();
        auto accent = SystemInfo::getAccentColor();
        auto power = SystemInfo::getPowerStatus();

        printKvp("OS", Ark::String::format("{} {}", osName, osVersion));
        printKvp("Computer", computerName);
        printKvp("User", userName);
        printKvp("Process", processName);
        printKvp("Local Time", Ark::String::format("{:04}-{:02}-{:02} {:02}:{:02}:{:02}.{:03}", dt.Date.Year, static_cast<int>(dt.Date.Month), dt.Date.Day, dt.Time.Hour, dt.Time.Minute, dt.Time.Second, dt.Time.Millisecond));

        auto mem = SystemInfo::getMemoryInfo();
        printKvp("RAM Total", formatBytes(mem.totalPhysicalBytes));
        printKvp("RAM Available", formatBytes(mem.availablePhysicalBytes));
        printKvp("Virtual Total", formatBytes(mem.totalVirtualBytes));
        printKvp("Virtual Available", formatBytes(mem.availableVirtualBytes));
        printKvp("Process Resident", formatBytes(mem.processResidentBytes));
        printKvp("Process Virtual", formatBytes(mem.processVirtualBytes));

        printKvp("Locale", locale);
        const char* themeStr = "Unknown";
        switch (theme)
        {
            case SystemInfo::Theme::Light:
                themeStr = "Light";
                break;
            case SystemInfo::Theme::Dark:
                themeStr = "Dark";
                break;
            default:
                break;
        }
        printKvp("Theme", Ark::String(themeStr));

        printKvp(
            "Accent Color",
            Ark::String::format("{},{},{},{}", static_cast<int>(accent.r), static_cast<int>(accent.g), static_cast<int>(accent.b), static_cast<int>(accent.a)));

        printKvp("Battery Present", power.batteryPresent ? Ark::String("Yes") : Ark::String("No"));
        printKvp("Charging", power.charging ? Ark::String("Yes") : Ark::String("No"));
        printKvp("On Battery", power.onBattery ? Ark::String("Yes") : Ark::String("No"));
        if (power.batteryPercent >= 0)
        {
            printKvpNumber("Battery Percent", power.batteryPercent);
        }
        else
        {
            printKvp("Battery Percent", Ark::String("<unknown>"));
        }

        if (power.minutesRemaining >= 0)
        {
            printKvpNumber("Minutes Remaining", power.minutesRemaining);
        }
        else
        {
            printKvp("Minutes Remaining", Ark::String("<unknown>"));
        }
    }

    printSectionHeader("Environment");
    printEnvPath();

    printSectionHeader("Known Folders");
    printKnownFolders();

    printSectionHeader("Process");
    {
        auto pid = getCurrentProcessId();
        auto processPath = getCurrentProcessPath();
        auto cwd = getCurrentWorkingDirectory();
        printKvpNumber("PID", pid);
        printKvp("Path", processPath.string());
        printKvp("Working Dir", cwd.string());
    }

    printSectionHeader("Arguments");
    if (args.getCount() > 0)
    {
        for (Ark::usize i = 0; i < args.getCount(); ++i)
        {
            printKvp(Ark::String::format("arg[{}]", i), args[i]);
        }
    }

    Console::flush();
    return true;
}
