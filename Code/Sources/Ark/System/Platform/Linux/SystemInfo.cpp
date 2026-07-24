#if defined(ARK_PLATFORM_LINUX)

#include "Ark/System/SystemInfo.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <pwd.h>
#include <sys/sysinfo.h>
#include <sys/utsname.h>
#include <unistd.h>

namespace Ark::System::SystemInfo
{
    static String readProcFile(char const* path)
    {
        FILE* f = fopen(path, "r");
        if (f == nullptr)
        {
            return {};
        }

        char buffer[512] = {};
        size_t n = fread(buffer, 1, sizeof(buffer) - 1, f);
        fclose(f);
        if (n == 0)
        {
            return {};
        }

        // Trim trailing newline
        if (buffer[n - 1] == '\n')
        {
            buffer[n - 1] = '\0';
        }
        return String(buffer);
    }

    String getOsName()
    {
        return "Linux";
    }

    String getOsVersion()
    {
        // Try /etc/os-release first
        FILE* f = fopen("/etc/os-release", "r");
        if (f != nullptr)
        {
            char line[512] = {};
            String pretty;
            String name;
            String versionId;
            while (fgets(line, sizeof(line), f) != nullptr)
            {
                // Remove trailing newline
                usize len = strlen(line);
                if (len > 0 && line[len - 1] == '\n')
                {
                    line[len - 1] = '\0';
                }

                if (strncmp(line, "PRETTY_NAME=", 12) == 0)
                {
                    char const* v = line + 12;
                    if (*v == '"')
                    {
                        ++v;
                    }
                    pretty = v;
                    if (!pretty.isEmpty() && pretty.getLast() == '"')
                    {
                        pretty.removeLast();
                    }
                }
                else if (strncmp(line, "NAME=", 5) == 0)
                {
                    char const* v = line + 5;
                    if (*v == '"')
                    {
                        ++v;
                    }
                    name = v;
                    if (!name.isEmpty() && name.getLast() == '"')
                    {
                        name.removeLast();
                    }
                }
                else if (strncmp(line, "VERSION_ID=", 11) == 0)
                {
                    char const* v = line + 11;
                    if (*v == '"')
                    {
                        ++v;
                    }
                    versionId = v;
                    if (!versionId.isEmpty() && versionId.getLast() == '"')
                    {
                        versionId.removeLast();
                    }
                }
            }
            fclose(f);

            if (!pretty.isEmpty())
            {
                return pretty;
            }
            if (!name.isEmpty())
            {
                if (!versionId.isEmpty())
                {
                    String out;
                    out.append(name);
                    out.push(' ');
                    out.append(versionId);
                    return out;
                }
                return name;
            }
        }

        // Fallback to kernel release
        struct utsname u{};
        if (uname(&u) == 0)
        {
            return u.release;
        }
        return {};
    }

    String getComputerName()
    {
        char name[256] = {};
        if (gethostname(name, sizeof(name)) == 0)
        {
            name[sizeof(name) - 1] = '\0';
            return name;
        }
        return {};
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
        // /proc/self/comm contains the command name (truncated to 16)
        String comm = readProcFile("/proc/self/comm");
        if (!comm.isEmpty())
        {
            return comm;
        }
        // Fallback: /proc/self/cmdline first token
        FILE* f = fopen("/proc/self/cmdline", "r");
        if (f == nullptr)
        {
            return {};
        }
        char buffer[1024] = {};
        size_t n = fread(buffer, 1, sizeof(buffer) - 1, f);
        fclose(f);
        if (n == 0)
        {
            return {};
        }
        return buffer;
    }

    Collections::Array<String> getCommandLineArguments()
    {
        // Not globally stored; return empty array.
        return {};
    }

    DateTime getLocalDateTime()
    {
        DateTime dt{};
        time_t now = time(nullptr);
        tm localTm{};
        localtime_r(&now, &localTm);

        dt.Date.Year = static_cast<uint32>(localTm.tm_year + 1900);
        dt.Date.Month = static_cast<Month>(localTm.tm_mon + 1);
        dt.Date.Day = static_cast<uint32>(localTm.tm_mday);
        dt.Date.Weekday = static_cast<Weekday>(localTm.tm_wday);

        dt.Time.Hour = static_cast<uint32>(localTm.tm_hour);
        dt.Time.Minute = static_cast<uint32>(localTm.tm_min);
        dt.Time.Second = static_cast<uint32>(localTm.tm_sec);
        dt.Time.Millisecond = 0;
        return dt;
    }

    MemoryInfo getMemoryInfo()
    {
        MemoryInfo info{};

        struct sysinfo si{};
        if (sysinfo(&si) == 0)
        {
            info.totalPhysicalBytes = static_cast<uint64>(si.totalram) * static_cast<uint64>(si.mem_unit);
            info.availablePhysicalBytes = static_cast<uint64>(si.freeram) * static_cast<uint64>(si.mem_unit);
            info.totalVirtualBytes = static_cast<uint64>(si.totalswap) * static_cast<uint64>(si.mem_unit);
            info.availableVirtualBytes = static_cast<uint64>(si.freeswap) * static_cast<uint64>(si.mem_unit);
        }

        // Process memory from /proc/self/statm
        FILE* f = fopen("/proc/self/statm", "r");
        if (f != nullptr)
        {
            unsigned long size = 0, resident = 0;
            if (fscanf(f, "%lu %lu", &size, &resident) == 2)
            {
                long page = sysconf(_SC_PAGESIZE);
                info.processVirtualBytes = static_cast<uint64>(size) * static_cast<uint64>(page);
                info.processResidentBytes = static_cast<uint64>(resident) * static_cast<uint64>(page);
            }
            fclose(f);
        }

        return info;
    }

    String getLocale()
    {
        // Try common environment variables in order of precedence
        char const* vars[] = {"LC_ALL", "LC_MESSAGES", "LANG"};
        for (char const* v : vars)
        {
            char const* val = getenv(v);
            if (val != nullptr && val[0] != '\0')
            {
                return String(val);
            }
        }
        return {};
    }

    Theme getAppTheme()
    {
        // Theme detection is DE-specific; return Unknown by default.
        return Theme::Unknown;
    }

    RgbaColor getAccentColor()
    {
        // No standard accent color; return zero alpha to indicate unavailable
        return {0, 0, 0, 0};
    }

    PowerStatus getPowerStatus()
    {
        PowerStatus ps{};

        // Try upower (if available) for rough info by reading /sys/class/power_supply
        // Basic fallback: detect AC/battery presence and percent
        FILE* f = fopen("/sys/class/power_supply/AC/online", "r");
        if (f != nullptr)
        {
            int online = 0;
            if (fscanf(f, "%d", &online) == 1)
            {
                ps.onBattery = (online == 0);
            }
            fclose(f);
        }

        f = fopen("/sys/class/power_supply/BAT0/status", "r");
        if (f != nullptr)
        {
            char status[32] = {};
            if (fscanf(f, "%31s", status) == 1)
            {
                ps.batteryPresent = true;
                if (std::strcmp(status, "Charging") == 0)
                {
                    ps.charging = true;
                }
            }
            fclose(f);
        }

        // Battery percent (if capacity file exists)
        f = fopen("/sys/class/power_supply/BAT0/capacity", "r");
        if (f != nullptr)
        {
            int pct = -1;
            if (fscanf(f, "%d", &pct) == 1)
            {
                ps.batteryPercent = pct;
            }
            fclose(f);
        }

        // Minutes remaining not standardized; leave -1
        return ps;
    }
}

#endif
