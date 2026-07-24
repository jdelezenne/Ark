#if defined(ARK_PLATFORM_WEBASSEMBLY)

#include "Ark/Display/Monitor.hpp"

#include <emscripten/emscripten.h>
#include <emscripten/html5.h>

namespace Ark::Display
{
    Collections::Array<MonitorId> Monitors::enumerate()
    {
        Collections::Array<MonitorId> ids;
        ids.append(static_cast<MonitorId>(1));
        return ids;
    }

    Option<MonitorId> Monitors::getPrimary()
    {
        return static_cast<MonitorId>(1);
    }

    String Monitors::getName(MonitorId /*id*/)
    {
        return String("Browser Display");
    }

    MonitorInfo Monitors::getInfo(MonitorId /*id*/)
    {
        int width = 0;
        int height = 0;
        emscripten_get_screen_size(&width, &height);
        if (width <= 0 || height <= 0)
        {
            width = 1280;
            height = 720;
        }

        double scale = emscripten_get_device_pixel_ratio();
        if (scale <= 0.0)
        {
            scale = 1.0;
        }

        MonitorInfo info{};
        info.name = getName(1);
        info.bounds = RectInt{0, 0, width, height};
        info.usableBounds = info.bounds;
        info.contentScale = static_cast<float32>(scale);
        info.naturalOrientation = (width >= height) ? Orientation::Landscape : Orientation::Portrait;
        info.currentOrientation = info.naturalOrientation;
        return info;
    }

    Collections::Array<Mode> Monitors::getFullscreenModes(MonitorId id)
    {
        Collections::Array<Mode> modes;
        if (auto desktop = getDesktopMode(id))
        {
            modes.append(*desktop);
        }
        return modes;
    }

    Option<Mode> Monitors::getDesktopMode(MonitorId id)
    {
        MonitorInfo info = getInfo(id);
        Mode mode{};
        mode.width = static_cast<uint32>(info.bounds.width);
        mode.height = static_cast<uint32>(info.bounds.height);
        mode.pixelDensity = info.contentScale;
        mode.refreshRate = 60.0f;
        return mode;
    }

    Option<Mode> Monitors::getCurrentMode(MonitorId id)
    {
        return getDesktopMode(id);
    }
}

#endif
