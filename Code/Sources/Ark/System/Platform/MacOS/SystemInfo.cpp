#include "Ark/System/SystemInfo.hpp"

#include <crt_externs.h>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <libproc.h>
#include <mach/mach.h>
#include <pwd.h>
#include <sys/sysctl.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <unistd.h>

#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/ps/IOPowerSources.h>

namespace Ark::System::SystemInfo
{
    static String readSysctlString(char const* name)
    {
        usize size = 0;
        if (sysctlbyname(name, nullptr, &size, nullptr, 0) != 0 || size == 0)
        {
            return {};
        }

        String result;
        result.resize(static_cast<String::SizeType>(size > 0 ? size - 1 : 0));
        if (sysctlbyname(name, result.asPointer(), &size, nullptr, 0) != 0)
        {
            return {};
        }
        return result;
    }

    String getOsName()
    {
        return "macOS";
    }

    String getOsVersion()
    {
        String version = readSysctlString("kern.osproductversion");
        if (!version.isEmpty())
        {
            return version;
        }

        // Fallback to Darwin kernel release when product version is unavailable
        return readSysctlString("kern.osrelease");
    }

    String getComputerName()
    {
        return readSysctlString("kern.hostname");
    }

    String getUserName()
    {
        uid_t uid = geteuid();
        passwd* pw = getpwuid(uid);
        if (pw != nullptr && pw->pw_name != nullptr)
        {
            return pw->pw_name;
        }
        return {};
    }

    String getCurrentProcessName()
    {
        char pathbuf[PROC_PIDPATHINFO_MAXSIZE] = {};
        pid_t pid = getpid();
        int ret = proc_pidpath(pid, pathbuf, sizeof(pathbuf));
        if (ret <= 0)
        {
            return {};
        }

        // Extract basename
        char const* lastSlash = strrchr(pathbuf, '/');
        if (lastSlash != nullptr && *(lastSlash + 1) != '\0')
        {
            return lastSlash + 1;
        }
        return pathbuf;
    }

    Collections::Array<String> getCommandLineArguments()
    {
        int argc = *_NSGetArgc();
        char** argv = *_NSGetArgv();

        Collections::Array<String> out;
        out.resize(static_cast<usize>(argc));
        for (int i = 0; i < argc; ++i)
        {
            out[i] = String(argv[i]);
        }
        return out;
    }

    DateTime getLocalDateTime()
    {
        DateTime dateTime{};

        time_t now = time(nullptr);
        tm localTm{};
        localtime_r(&now, &localTm);

        dateTime.Date.Year = static_cast<uint32>(localTm.tm_year + 1900);
        dateTime.Date.Month = static_cast<Month>(localTm.tm_mon + 1);
        dateTime.Date.Day = static_cast<uint32>(localTm.tm_mday);
        dateTime.Date.Weekday = static_cast<Weekday>(localTm.tm_wday);

        dateTime.Time.Hour = static_cast<uint32>(localTm.tm_hour);
        dateTime.Time.Minute = static_cast<uint32>(localTm.tm_min);
        dateTime.Time.Second = static_cast<uint32>(localTm.tm_sec);
        dateTime.Time.Millisecond = 0;
        return dateTime;
    }

    MemoryInfo getMemoryInfo()
    {
        MemoryInfo info{};

        // System memory via sysctl
        int mib[2] = {CTL_HW, HW_MEMSIZE};
        uint64_t memsize = 0;
        usize len = sizeof(memsize);
        if (sysctl(mib, 2, &memsize, &len, nullptr, 0) == 0)
        {
            info.totalPhysicalBytes = static_cast<uint64>(memsize);
        }

        // Available memory (approx) via vm statistics
        mach_msg_type_number_t count = HOST_VM_INFO_COUNT;
        vm_statistics_data_t vmstat{};
        mach_port_t hostPort = mach_host_self();
        if (host_statistics(hostPort, HOST_VM_INFO, reinterpret_cast<host_info_t>(&vmstat), &count) == KERN_SUCCESS)
        {
            vm_size_t pageSize = 0;
            host_page_size(hostPort, &pageSize);

            uint64 freeBytes = static_cast<uint64>(vmstat.free_count) * static_cast<uint64>(pageSize);
            uint64 inactiveBytes = static_cast<uint64>(vmstat.inactive_count) * static_cast<uint64>(pageSize);
            info.availablePhysicalBytes = freeBytes + inactiveBytes;
        }

        // Process memory via task_info
        task_basic_info_data_t tinfo{};
        mach_msg_type_number_t tcount = TASK_BASIC_INFO_COUNT;
        if (task_info(mach_task_self(), TASK_BASIC_INFO, reinterpret_cast<task_info_t>(&tinfo), &tcount) == KERN_SUCCESS)
        {
            info.processResidentBytes = static_cast<uint64>(tinfo.resident_size);
            info.processVirtualBytes = static_cast<uint64>(tinfo.virtual_size);
        }

        // Virtual memory totals not trivially available; leave zeros on macOS.
        return info;
    }

    String getLocale()
    {
        String result;
        CFLocaleRef locale = CFLocaleCopyCurrent();
        if (locale == nullptr)
        {
            return result;
        }

        CFStringRef identifier = CFLocaleGetIdentifier(locale);
        if (identifier != nullptr)
        {
            char buffer[256] = {};
            Boolean ok = CFStringGetCString(identifier, buffer, sizeof(buffer), kCFStringEncodingUTF8);
            if (ok != 0)
            {
                result = buffer;
            }
        }

        CFRelease(locale);
        return result;
    }

    Theme getAppTheme()
    {
        // Read global domain key "AppleInterfaceStyle"; if present and equals "Dark", return Dark.
        CFStringRef key = CFSTR("AppleInterfaceStyle");
        CFStringRef domain = kCFPreferencesAnyApplication;
        CFStringRef value = static_cast<CFStringRef>(CFPreferencesCopyValue(key, domain, kCFPreferencesCurrentUser, kCFPreferencesAnyHost));
        if (value != nullptr)
        {
            char buffer[64] = {};
            Boolean ok = CFStringGetCString(value, buffer, sizeof(buffer), kCFStringEncodingUTF8);
            CFRelease(value);
            if (ok != 0)
            {
                if (std::strcmp(buffer, "Dark") == 0 || std::strcmp(buffer, "dark") == 0)
                {
                    return Theme::Dark;
                }
                return Theme::Light;
            }
        }
        return Theme::Light;
    }

    RgbaColor getAccentColor()
    {
        RgbaColor color{0, 122, 255, 255};

        // Try AppleHighlightColor first (space-separated floats 0..1)
        CFStringRef highlightKey = CFSTR("AppleHighlightColor");
        CFStringRef str = static_cast<CFStringRef>(CFPreferencesCopyValue(highlightKey, kCFPreferencesAnyApplication, kCFPreferencesCurrentUser, kCFPreferencesAnyHost));
        if (str != nullptr)
        {
            char buffer[128] = {};
            if (CFStringGetCString(str, buffer, sizeof(buffer), kCFStringEncodingUTF8) != 0)
            {
                float r = 0.0f, g = 0.0f, b = 0.0f;
                if (std::sscanf(buffer, "%f %f %f", &r, &g, &b) == 3)
                {
                    color.r = static_cast<uint8>(r * 255.0f);
                    color.g = static_cast<uint8>(g * 255.0f);
                    color.b = static_cast<uint8>(b * 255.0f);
                    color.a = 255;
                    CFRelease(str);
                    return color;
                }
            }
            CFRelease(str);
        }

        // Fallback known accent indices mapping (AppleAccentColor)
        CFStringRef accentKey = CFSTR("AppleAccentColor");
        CFNumberRef num = static_cast<CFNumberRef>(CFPreferencesCopyValue(accentKey, kCFPreferencesAnyApplication, kCFPreferencesCurrentUser, kCFPreferencesAnyHost));
        if (num != nullptr)
        {
            int32 idx = -1;
            if (CFNumberGetValue(num, kCFNumberSInt32Type, &idx))
            {
                // Based on macOS mapping (approximate): 0=Graphite,1=Red,2=Orange,3=Yellow,4=Green,5=Blue,6=Purple,7=Pink,-1=Multicolor
                switch (idx)
                {
                    case -1:
                        color = {0, 122, 255, 255};
                        break; // Multicolor → default system blue
                    case 1:
                        color = {255, 59, 48, 255};
                        break; // Red
                    case 2:
                        color = {255, 149, 0, 255};
                        break; // Orange
                    case 3:
                        color = {255, 204, 0, 255};
                        break; // Yellow
                    case 4:
                        color = {52, 199, 89, 255};
                        break; // Green
                    case 5:
                        color = {0, 122, 255, 255};
                        break; // Blue
                    case 6:
                        color = {175, 82, 222, 255};
                        break; // Purple
                    case 7:
                        color = {255, 45, 85, 255};
                        break; // Pink
                    case 0:
                        color = {142, 142, 147, 255};
                        break; // Graphite
                    default:
                        color = {0, 122, 255, 255};
                        break; // Default Blue
                }
            }
            CFRelease(num);
        }
        return color;
    }

    PowerStatus getPowerStatus()
    {
        PowerStatus ps{};

        CFTypeRef blob = IOPSCopyPowerSourcesInfo();
        if (blob == nullptr)
        {
            return ps;
        }

        CFArrayRef list = IOPSCopyPowerSourcesList(blob);
        if (list == nullptr)
        {
            CFRelease(blob);
            return ps;
        }

        CFDictionaryRef details = IOPSCopyExternalPowerAdapterDetails();
        bool onAc = (details != nullptr);
        if (details != nullptr)
        {
            CFRelease(details);
        }
        ps.onBattery = !onAc;

        CFIndex count = CFArrayGetCount(list);
        for (CFIndex i = 0; i < count; ++i)
        {
            CFTypeRef psRef = CFArrayGetValueAtIndex(list, i);
            CFDictionaryRef desc = IOPSGetPowerSourceDescription(blob, psRef);
            if (desc == nullptr)
            {
                continue;
            }

            CFStringRef type = static_cast<CFStringRef>(CFDictionaryGetValue(desc, CFSTR("Type")));
            if (type == nullptr)
            {
                continue;
            }

            if (CFStringCompare(type, CFSTR("InternalBattery"), 0) == kCFCompareEqualTo)
            {
                ps.batteryPresent = true;

                CFBooleanRef charging = static_cast<CFBooleanRef>(CFDictionaryGetValue(desc, CFSTR("Is Charging")));
                if (charging != nullptr)
                {
                    ps.charging = (CFBooleanGetValue(charging) != 0);
                }

                CFNumberRef cap = static_cast<CFNumberRef>(CFDictionaryGetValue(desc, CFSTR("Current Capacity")));
                CFNumberRef max = static_cast<CFNumberRef>(CFDictionaryGetValue(desc, CFSTR("Max Capacity")));
                int cur = 0, mx = 0;
                if (cap != nullptr && max != nullptr && (CFNumberGetValue(cap, kCFNumberIntType, &cur) != 0) && (CFNumberGetValue(max, kCFNumberIntType, &mx) != 0) && mx > 0)
                {
                    ps.batteryPercent = static_cast<int32>((cur * 100) / mx);
                }

                CFNumberRef timeToEmpty = static_cast<CFNumberRef>(CFDictionaryGetValue(desc, CFSTR("Time to Empty")));
                int tte = -1;
                if (timeToEmpty != nullptr && (CFNumberGetValue(timeToEmpty, kCFNumberIntType, &tte) != 0) && tte >= 0)
                {
                    ps.minutesRemaining = tte;
                }
            }
        }

        CFRelease(list);
        CFRelease(blob);
        return ps;
    }
}
