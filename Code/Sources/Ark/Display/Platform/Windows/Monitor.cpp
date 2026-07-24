#if defined(ARK_PLATFORM_WINDOWS)

#include "Ark/Display/Monitor.hpp"

#include "Ark/Collections/Array.hpp"
#include "Ark/Strings/Unicode.hpp"

#include <Windows.h>
#include <dxgi.h>

namespace Ark::Display
{
    namespace
    {
        struct EnumContext
        {
            Collections::Array<MonitorId>* ids;
            bool wantPrimary;
        };

        BOOL CALLBACK enumMonitorsProc(HMONITOR hMonitor, HDC, LPRECT, LPARAM dwData)
        {
            auto* ctx = reinterpret_cast<EnumContext*>(dwData);

            MONITORINFOEXW info{};
            info.cbSize = sizeof(info);
            if (GetMonitorInfoW(hMonitor, &info) == 0)
            {
                return TRUE;
            }

            bool const isPrimary = (info.dwFlags & MONITORINFOF_PRIMARY) == MONITORINFOF_PRIMARY;
            if (isPrimary == ctx->wantPrimary)
            {
                ctx->ids->push(static_cast<MonitorId>(reinterpret_cast<UIntPtr>(hMonitor)));
            }
            return TRUE;
        }

        static Orientation toOrientation(DWORD orientation)
        {
            switch (orientation)
            {
                case DMDO_DEFAULT:
                    return Orientation::Landscape;
                case DMDO_180:
                    return Orientation::LandscapeFlipped;
                case DMDO_90:
                    return Orientation::Portrait;
                case DMDO_270:
                    return Orientation::PortraitFlipped;
                default:
                    return Orientation::Unknown;
            }
        }
    }

    Collections::Array<MonitorId> Monitors::enumerate()
    {
        Collections::Array<MonitorId> ids;
        EnumContext ctx{&ids, true};
        EnumDisplayMonitors(nullptr, nullptr, enumMonitorsProc, reinterpret_cast<LPARAM>(&ctx));
        ctx.wantPrimary = false;
        EnumDisplayMonitors(nullptr, nullptr, enumMonitorsProc, reinterpret_cast<LPARAM>(&ctx));
        return ids;
    }

    Option<MonitorId> Monitors::getPrimary()
    {
        HMONITOR primary = MonitorFromPoint(POINT{0, 0}, MONITOR_DEFAULTTOPRIMARY);
        if (primary != nullptr)
        {
            return static_cast<MonitorId>(reinterpret_cast<UIntPtr>(primary));
        }
        return none;
    }

    String Monitors::getName(MonitorId id)
    {
        HMONITOR hMonitor = reinterpret_cast<HMONITOR>(static_cast<UIntPtr>(id));

        MONITORINFOEXW info{};
        info.cbSize = sizeof(info);
        if (GetMonitorInfoW(hMonitor, &info) != 0)
        {
            return Unicode::fromWide(info.szDevice);
        }
        return {};
    }

    MonitorInfo Monitors::getInfo(MonitorId id)
    {
        MonitorInfo result{};

        HMONITOR hMonitor = reinterpret_cast<HMONITOR>(static_cast<UIntPtr>(id));

        MONITORINFOEXW minfo{};
        minfo.cbSize = sizeof(minfo);
        if (GetMonitorInfoW(hMonitor, &minfo) != 0)
        {
            result.name = Unicode::fromWide(minfo.szDevice);
            result.bounds = RectInt{minfo.rcMonitor.left, minfo.rcMonitor.top, minfo.rcMonitor.right - minfo.rcMonitor.left, minfo.rcMonitor.bottom - minfo.rcMonitor.top};
            result.usableBounds = RectInt{minfo.rcWork.left, minfo.rcWork.top, minfo.rcWork.right - minfo.rcWork.left, minfo.rcWork.bottom - minfo.rcWork.top};
        }

        DEVMODEW dev{};
        dev.dmSize = sizeof(dev);
        if (EnumDisplaySettingsExW(minfo.szDevice, ENUM_CURRENT_SETTINGS, &dev, 0))
        {
            result.naturalOrientation = toOrientation(dev.dmDisplayOrientation);
            result.currentOrientation = toOrientation(dev.dmDisplayOrientation);
        }

        // Content scale from DPI
        UINT dpiX = 96, dpiY = 96;
        HDC hdc = CreateDCW(L"DISPLAY", minfo.szDevice, nullptr, nullptr);
        if (hdc != nullptr)
        {
            dpiX = GetDeviceCaps(hdc, LOGPIXELSX);
            dpiY = GetDeviceCaps(hdc, LOGPIXELSY);
            DeleteDC(hdc);
        }
        result.contentScale = static_cast<Float32>(dpiX) / 96.0f;

        return result;
    }

    Collections::Array<Mode> Monitors::getFullscreenModes(MonitorId id)
    {
        Collections::Array<Mode> modes;

        HMONITOR hMonitor = reinterpret_cast<HMONITOR>(static_cast<UIntPtr>(id));

        MONITORINFOEXW info{};
        info.cbSize = sizeof(info);
        if (GetMonitorInfoW(hMonitor, &info) == 0)
        {
            return modes;
        }

        for (DWORD i = 0;; ++i)
        {
            DEVMODEW dev{};
            dev.dmSize = sizeof(dev);
            if (!EnumDisplaySettingsExW(info.szDevice, i, &dev, 0))
            {
                break;
            }

            if ((dev.dmFields & (DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY)) != 0)
            {
                Mode m{};
                m.width = static_cast<uint32>(dev.dmPelsWidth);
                m.height = static_cast<uint32>(dev.dmPelsHeight);
                m.refreshRate = (dev.dmDisplayFrequency > 1) ? static_cast<float32>(dev.dmDisplayFrequency) : 0.0f;
                m.pixelDensity = 1.0f; // no per-mode scale; report 1.0
                modes.push(m);
            }
        }

        return modes;
    }

    Option<Mode> Monitors::getDesktopMode(MonitorId id)
    {
        HMONITOR hMonitor = reinterpret_cast<HMONITOR>(static_cast<UIntPtr>(id));

        MONITORINFOEXW info{};
        info.cbSize = sizeof(info);
        if (GetMonitorInfoW(hMonitor, &info) == 0)
        {
            return none;
        }

        DEVMODEW dev{};
        dev.dmSize = sizeof(dev);
        if (!EnumDisplaySettingsExW(info.szDevice, ENUM_REGISTRY_SETTINGS, &dev, 0))
        {
            return none;
        }

        Mode m{};
        m.width = static_cast<uint32>(dev.dmPelsWidth);
        m.height = static_cast<uint32>(dev.dmPelsHeight);
        m.refreshRate = (dev.dmDisplayFrequency > 1) ? static_cast<float32>(dev.dmDisplayFrequency) : 0.0f;
        m.pixelDensity = 1.0f;
        return m;
    }

    Option<Mode> Monitors::getCurrentMode(MonitorId id)
    {
        HMONITOR hMonitor = reinterpret_cast<HMONITOR>(static_cast<UIntPtr>(id));

        MONITORINFOEXW info{};
        info.cbSize = sizeof(info);
        if (GetMonitorInfoW(hMonitor, &info) == 0)
        {
            return none;
        }

        DEVMODEW dev{};
        dev.dmSize = sizeof(dev);
        if (!EnumDisplaySettingsExW(info.szDevice, ENUM_CURRENT_SETTINGS, &dev, 0))
        {
            return none;
        }

        Mode m{};
        m.width = static_cast<uint32>(dev.dmPelsWidth);
        m.height = static_cast<uint32>(dev.dmPelsHeight);
        m.refreshRate = (dev.dmDisplayFrequency > 1) ? static_cast<float32>(dev.dmDisplayFrequency) : 0.0f;
        m.pixelDensity = 1.0f;
        return m;
    }
}

#endif
