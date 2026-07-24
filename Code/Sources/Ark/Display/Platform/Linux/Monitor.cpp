#if defined(ARK_PLATFORM_LINUX) && !defined(ARK_PLATFORM_WEBASSEMBLY)

#include "Ark/Display/Monitor.hpp"
#include "Ark/Display/Platform/Linux/X11Context.hpp"

#include "Ark/Collections/Array.hpp"
#include "Ark/Strings/String.hpp"

#include <X11/extensions/Xrandr.h>

#include <cstdio>

namespace Ark::Display
{
    namespace
    {
        static String makeMonitorName(int index)
        {
            char buffer[32] = {};
            std::snprintf(buffer, sizeof(buffer), "Display %d", index);
            return String(buffer);
        }

        static bool queryRandR(XRRScreenResources*& resources, RROutput& primary)
        {
            resources = nullptr;
            primary = 0;

            Platform::Linux::X11Context& ctx = Platform::Linux::X11Context::get();
            if (!ctx.isValid())
            {
                return false;
            }

            int eventBase = 0;
            int errorBase = 0;
            if (!XRRQueryExtension(ctx.display, &eventBase, &errorBase))
            {
                return false;
            }

            resources = XRRGetScreenResourcesCurrent(ctx.display, ctx.root);
            if (resources == nullptr)
            {
                return false;
            }

            primary = XRRGetOutputPrimary(ctx.display, ctx.root);
            return true;
        }
    }

    Collections::Array<MonitorId> Monitors::enumerate()
    {
        Collections::Array<MonitorId> ids;
        XRRScreenResources* resources = nullptr;
        RROutput primary = 0;
        if (!queryRandR(resources, primary))
        {
            Platform::Linux::X11Context& ctx = Platform::Linux::X11Context::get();
            if (ctx.isValid())
            {
                ids.append(static_cast<MonitorId>(1));
            }
            return ids;
        }

        if (primary != 0)
        {
            ids.append(static_cast<MonitorId>(primary));
        }

        for (int i = 0; i < resources->noutput; ++i)
        {
            RROutput output = resources->outputs[i];
            XRROutputInfo* info = XRRGetOutputInfo(Platform::Linux::X11Context::get().display, resources, output);
            if (info == nullptr)
            {
                continue;
            }
            if (info->connection == RR_Connected && info->crtc != 0 && output != primary)
            {
                ids.append(static_cast<MonitorId>(output));
            }
            XRRFreeOutputInfo(info);
        }

        XRRFreeScreenResources(resources);

        if (ids.isEmpty())
        {
            ids.append(static_cast<MonitorId>(1));
        }
        return ids;
    }

    Option<MonitorId> Monitors::getPrimary()
    {
        Collections::Array<MonitorId> ids = enumerate();
        if (ids.isEmpty())
        {
            return none;
        }
        return ids[0];
    }

    String Monitors::getName(MonitorId id)
    {
        XRRScreenResources* resources = nullptr;
        RROutput primary = 0;
        if (!queryRandR(resources, primary))
        {
            return makeMonitorName(static_cast<int>(id));
        }

        String name = makeMonitorName(static_cast<int>(id));
        for (int i = 0; i < resources->noutput; ++i)
        {
            if (resources->outputs[i] != static_cast<RROutput>(id))
            {
                continue;
            }
            XRROutputInfo* info = XRRGetOutputInfo(Platform::Linux::X11Context::get().display, resources, resources->outputs[i]);
            if (info != nullptr)
            {
                if (info->name != nullptr)
                {
                    name = String(info->name);
                }
                XRRFreeOutputInfo(info);
            }
            break;
        }

        XRRFreeScreenResources(resources);
        return name;
    }

    MonitorInfo Monitors::getInfo(MonitorId id)
    {
        MonitorInfo info{};
        info.name = getName(id);
        info.contentScale = 1.0f;
        info.naturalOrientation = Orientation::Landscape;
        info.currentOrientation = Orientation::Landscape;

        Platform::Linux::X11Context& ctx = Platform::Linux::X11Context::get();
        if (!ctx.isValid())
        {
            return info;
        }

        XRRScreenResources* resources = nullptr;
        RROutput primary = 0;
        if (!queryRandR(resources, primary))
        {
            int w = DisplayWidth(ctx.display, ctx.screen);
            int h = DisplayHeight(ctx.display, ctx.screen);
            info.bounds = RectInt{0, 0, w, h};
            info.usableBounds = info.bounds;
            return info;
        }

        for (int i = 0; i < resources->noutput; ++i)
        {
            if (resources->outputs[i] != static_cast<RROutput>(id))
            {
                continue;
            }

            XRROutputInfo* outputInfo = XRRGetOutputInfo(ctx.display, resources, resources->outputs[i]);
            if (outputInfo == nullptr || outputInfo->crtc == 0)
            {
                if (outputInfo != nullptr)
                {
                    XRRFreeOutputInfo(outputInfo);
                }
                break;
            }

            XRRCrtcInfo* crtc = XRRGetCrtcInfo(ctx.display, resources, outputInfo->crtc);
            if (crtc != nullptr)
            {
                info.bounds = RectInt{
                    static_cast<int>(crtc->x),
                    static_cast<int>(crtc->y),
                    static_cast<int>(crtc->width),
                    static_cast<int>(crtc->height),
                };
                info.usableBounds = info.bounds;
                XRRFreeCrtcInfo(crtc);
            }
            XRRFreeOutputInfo(outputInfo);
            break;
        }

        XRRFreeScreenResources(resources);
        return info;
    }

    Collections::Array<Mode> Monitors::getFullscreenModes(MonitorId id)
    {
        Collections::Array<Mode> modes;
        Platform::Linux::X11Context& ctx = Platform::Linux::X11Context::get();
        if (!ctx.isValid())
        {
            return modes;
        }

        XRRScreenResources* resources = nullptr;
        RROutput primary = 0;
        if (!queryRandR(resources, primary))
        {
            Mode m{};
            m.width = static_cast<uint32>(DisplayWidth(ctx.display, ctx.screen));
            m.height = static_cast<uint32>(DisplayHeight(ctx.display, ctx.screen));
            m.refreshRate = 60.0f;
            m.pixelDensity = 1.0f;
            modes.append(m);
            return modes;
        }

        for (int i = 0; i < resources->noutput; ++i)
        {
            if (resources->outputs[i] != static_cast<RROutput>(id))
            {
                continue;
            }

            XRROutputInfo* outputInfo = XRRGetOutputInfo(ctx.display, resources, resources->outputs[i]);
            if (outputInfo == nullptr)
            {
                break;
            }

            for (int m = 0; m < outputInfo->nmode; ++m)
            {
                RRMode modeId = outputInfo->modes[m];
                for (int r = 0; r < resources->nmode; ++r)
                {
                    if (resources->modes[r].id != modeId)
                    {
                        continue;
                    }
                    XRRModeInfo const& modeInfo = resources->modes[r];
                    Mode mode{};
                    mode.width = modeInfo.width;
                    mode.height = modeInfo.height;
                    mode.pixelDensity = 1.0f;
                    if (modeInfo.hTotal != 0 && modeInfo.vTotal != 0)
                    {
                        mode.refreshRate = static_cast<float32>(modeInfo.dotClock) / static_cast<float32>(modeInfo.hTotal * modeInfo.vTotal);
                    }
                    modes.append(mode);
                    break;
                }
            }

            XRRFreeOutputInfo(outputInfo);
            break;
        }

        XRRFreeScreenResources(resources);
        return modes;
    }

    Option<Mode> Monitors::getDesktopMode(MonitorId id)
    {
        Collections::Array<Mode> modes = getFullscreenModes(id);
        if (modes.isEmpty())
        {
            MonitorInfo info = getInfo(id);
            Mode m{};
            m.width = static_cast<uint32>(info.bounds.width);
            m.height = static_cast<uint32>(info.bounds.height);
            m.refreshRate = 60.0f;
            m.pixelDensity = 1.0f;
            return m;
        }
        return modes[0];
    }

    Option<Mode> Monitors::getCurrentMode(MonitorId id)
    {
        return getDesktopMode(id);
    }
}

#endif
