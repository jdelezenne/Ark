#if defined(ARK_PLATFORM_WINDOWS)

#include "Ark/System/SystemInfo.hpp"

#include "Ark/Strings/Unicode.hpp"
#include <Lmcons.h>
#include <Psapi.h>
#include <Windows.h>
#include <cstring>
#include <powrprof.h>
#include <shellapi.h>
#include <winternl.h>
#pragma comment(lib, "PowrProf.lib")

namespace Ark::System::SystemInfo
{
    using Ark::Unicode::fromWide;
    String getOsName()
    {
        return "Windows";
    }

    String getOsVersion()
    {
        using RtlGetVersionPtr = LONG(WINAPI*)(PRTL_OSVERSIONINFOW);

        HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
        if (ntdll == nullptr)
        {
            return {};
        }

        auto rtlGetVersion = reinterpret_cast<RtlGetVersionPtr>(GetProcAddress(ntdll, "RtlGetVersion"));
        if (rtlGetVersion == nullptr)
        {
            return {};
        }

        RTL_OSVERSIONINFOEXW ver{};
        ver.dwOSVersionInfoSize = sizeof(ver);
        if (rtlGetVersion(reinterpret_cast<PRTL_OSVERSIONINFOW>(&ver)) != 0)
        {
            return {};
        }

        return String::format(
            "{}.{}.{}",
            static_cast<int>(ver.dwMajorVersion),
            static_cast<int>(ver.dwMinorVersion),
            static_cast<int>(ver.dwBuildNumber));
    }

    String getComputerName()
    {
        wchar_t buffer[MAX_COMPUTERNAME_LENGTH + 1] = {};
        DWORD size = MAX_COMPUTERNAME_LENGTH + 1;
        if (GetComputerNameW(buffer, &size))
        {
            return fromWide(buffer);
        }
        return {};
    }

    String getUserName()
    {
        wchar_t buffer[UNLEN + 1] = {};
        DWORD size = UNLEN + 1;
        if (GetUserNameW(buffer, &size))
        {
            return fromWide(buffer);
        }
        return {};
    }

    String getCurrentProcessName()
    {
        wchar_t buffer[MAX_PATH] = {};
        DWORD const len = GetModuleFileNameW(nullptr, buffer, MAX_PATH);
        if (len == 0)
        {
            return {};
        }
        wchar_t const* lastSlash = wcsrchr(buffer, L'\\');
        if (lastSlash != nullptr && *(lastSlash + 1) != L'\0')
        {
            return fromWide(lastSlash + 1);
        }
        return fromWide(buffer);
    }

    Collections::Array<String> getCommandLineArguments()
    {
        int argc = 0;
        wchar_t** argv = CommandLineToArgvW(GetCommandLineW(), &argc);
        if (argv == nullptr || argc <= 0)
        {
            return {};
        }

        Collections::Array<String> out;
        out.resize(static_cast<usize>(argc));
        for (int i = 0; i < argc; ++i)
        {
            out[i] = fromWide(argv[i]);
        }
        LocalFree(argv);
        return out;
    }

    DateTime getLocalDateTime()
    {
        SYSTEMTIME st{};
        GetLocalTime(&st);

        DateTime dt{};
        dt.Date.Year = st.wYear;
        dt.Date.Month = static_cast<Month>(st.wMonth);
        dt.Date.Day = st.wDay;
        dt.Date.Weekday = static_cast<Weekday>((st.wDayOfWeek + 0) % 7);

        dt.Time.Hour = st.wHour;
        dt.Time.Minute = st.wMinute;
        dt.Time.Second = st.wSecond;
        dt.Time.Millisecond = st.wMilliseconds;
        return dt;
    }

    MemoryInfo getMemoryInfo()
    {
        MemoryInfo info{};

        MEMORYSTATUSEX msx{};
        msx.dwLength = sizeof(msx);
        if (GlobalMemoryStatusEx(&msx))
        {
            info.totalPhysicalBytes = static_cast<uint64>(msx.ullTotalPhys);
            info.availablePhysicalBytes = static_cast<uint64>(msx.ullAvailPhys);
            info.totalVirtualBytes = static_cast<uint64>(msx.ullTotalVirtual);
            info.availableVirtualBytes = static_cast<uint64>(msx.ullAvailVirtual);
        }

        PROCESS_MEMORY_COUNTERS_EX pmc{};
        if (GetProcessMemoryInfo(GetCurrentProcess(), reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&pmc), sizeof(pmc)))
        {
            info.processResidentBytes = static_cast<uint64>(pmc.WorkingSetSize);
            info.processVirtualBytes = static_cast<uint64>(pmc.PrivateUsage);
        }

        return info;
    }

    String getLocale()
    {
        wchar_t buffer[LOCALE_NAME_MAX_LENGTH] = {};
        int n = GetUserDefaultLocaleName(buffer, LOCALE_NAME_MAX_LENGTH);
        if (n > 0)
        {
            return fromWide(buffer);
        }
        return {};
    }

    Theme getAppTheme()
    {
        // Windows 10 1903+: AppsUseLightTheme (0=dark,1=light) under HKCU\Software\Microsoft\Windows\CurrentVersion\Themes\Personalize
        HKEY key = nullptr;
        LONG res = RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", 0, KEY_READ, &key);
        if (res == ERROR_SUCCESS)
        {
            DWORD value = 1;
            DWORD size = sizeof(value);
            DWORD type = 0;
            if (RegQueryValueExW(key, L"AppsUseLightTheme", nullptr, &type, reinterpret_cast<LPBYTE>(&value), &size) == ERROR_SUCCESS && type == REG_DWORD)
            {
                RegCloseKey(key);
                return (value == 0) ? Theme::Dark : Theme::Light;
            }
            RegCloseKey(key);
        }
        return Theme::Unknown;
    }

    RgbaColor getAccentColor()
    {
        RgbaColor color{};
        // Windows accent color stored in HKCU\Software\Microsoft\Windows\DWM -> ColorizationColor (DWORD ARGB)
        HKEY key = nullptr;
        if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\DWM", 0, KEY_READ, &key) == ERROR_SUCCESS)
        {
            DWORD value = 0;
            DWORD size = sizeof(value);
            DWORD type = 0;
            if (RegQueryValueExW(key, L"ColorizationColor", nullptr, &type, reinterpret_cast<LPBYTE>(&value), &size) == ERROR_SUCCESS && type == REG_DWORD)
            {
                // Value is 0xAARRGGBB
                color.r = static_cast<uint8>((value >> 16) & 0xFF);
                color.g = static_cast<uint8>((value >> 8) & 0xFF);
                color.b = static_cast<uint8>(value & 0xFF);
                color.a = static_cast<uint8>((value >> 24) & 0xFF);
            }
            RegCloseKey(key);
        }
        return color;
    }

    PowerStatus getPowerStatus()
    {
        PowerStatus ps{};
        SYSTEM_POWER_STATUS sps{};
        if (GetSystemPowerStatus(&sps))
        {
            ps.onBattery = (sps.ACLineStatus == 0);
            ps.charging = (sps.BatteryFlag & 8) != 0; // Charging flag
            if (sps.BatteryLifePercent <= 100)
            {
                ps.batteryPercent = static_cast<int32>(sps.BatteryLifePercent);
            }
            if (sps.BatteryLifeTime != static_cast<DWORD>(-1))
            {
                ps.minutesRemaining = static_cast<int32>(sps.BatteryLifeTime / 60);
            }
            ps.batteryPresent = (sps.BatteryFlag & 128) == 0; // 128 means no system battery
        }
        return ps;
    }
}

#endif
