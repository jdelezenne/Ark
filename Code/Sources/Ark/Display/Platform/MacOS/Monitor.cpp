#if defined(ARK_PLATFORM_MACOS)

#include "Ark/Display/Monitor.hpp"

#include "Ark/Collections/Array.hpp"
#include "Ark/Strings/String.hpp"

#include <CoreGraphics/CoreGraphics.h>
#include <cstdio>
#include <cstring>

namespace Ark::Display
{
    namespace
    {
        static Orientation toOrientation(CGFloat rotationDegrees)
        {
            int const roundedDegree = static_cast<int>(rotationDegrees);

            switch (roundedDegree)
            {
                case 0:
                    return Orientation::Landscape;

                case 90:
                    return Orientation::Portrait;

                case 180:
                    return Orientation::LandscapeFlipped;

                case 270:
                    return Orientation::PortraitFlipped;

                default:
                    return Orientation::Unknown;
            }
        }

        static String makeDisplayName(CGDirectDisplayID displayId)
        {
            String name;
            char buffer[32] = {};
            std::snprintf(buffer, sizeof(buffer), "Display %u", static_cast<unsigned int>(displayId));
            name.append(buffer, static_cast<String::SizeType>(Utf8Strings::getByteLengthUnsafe(buffer)));
            return name;
        }
    }

    Ark::Collections::Array<MonitorId> Monitors::enumerate()
    {
        Ark::Collections::Array<MonitorId> ids;

        CGDirectDisplayID displayList[32] = {};
        uint32_t max = 32;
        uint32_t count = 0;
        if (CGGetActiveDisplayList(max, displayList, &count) != kCGErrorSuccess)
        {
            return ids;
        }

        CGDirectDisplayID primary = CGMainDisplayID();
        if (primary != 0)
        {
            ids.append(static_cast<MonitorId>(primary));
        }

        for (uint32_t i = 0; i < count; ++i)
        {
            if (displayList[i] == primary)
            {
                continue;
            }
            ids.append(static_cast<MonitorId>(displayList[i]));
        }

        return ids;
    }

    Option<MonitorId> Monitors::getPrimary()
    {
        CGDirectDisplayID primary = CGMainDisplayID();
        if (primary != 0)
        {
            return static_cast<MonitorId>(primary);
        }
        return none;
    }

    String Monitors::getName(MonitorId id)
    {
        return makeDisplayName(static_cast<CGDirectDisplayID>(id));
    }

    MonitorInfo Monitors::getInfo(MonitorId id)
    {
        CGDirectDisplayID displayId = static_cast<CGDirectDisplayID>(id);
        CGRect bounds = CGDisplayBounds(displayId);

        RectInt rectBounds{static_cast<int>(bounds.origin.x), static_cast<int>(bounds.origin.y), static_cast<int>(bounds.size.width), static_cast<int>(bounds.size.height)};

        // Content scale: pixelsWide / pointsWidth
        size_t pixelsWide = CGDisplayPixelsWide(displayId);
        float scaleX = (bounds.size.width > 0.0) ? static_cast<float>(pixelsWide) / static_cast<float>(bounds.size.width) : 1.0f;

        MonitorInfo info{
            .name = makeDisplayName(displayId),
            .bounds = rectBounds,
            .usableBounds = rectBounds,
            .contentScale = static_cast<float32>(scaleX),
            .naturalOrientation = Orientation::Landscape,
            .currentOrientation = toOrientation(CGDisplayRotation(displayId)),
        };

        return info;
    }

    Ark::Collections::Array<Mode> Monitors::getFullscreenModes(MonitorId id)
    {
        Ark::Collections::Array<Mode> modes;
        CGDirectDisplayID displayId = static_cast<CGDirectDisplayID>(id);

        CFArrayRef modeList = CGDisplayCopyAllDisplayModes(displayId, nullptr);
        if (modeList == nullptr)
        {
            return modes;
        }

        CFIndex count = CFArrayGetCount(modeList);
        for (CFIndex i = 0; i < count; ++i)
        {
            auto dm = static_cast<CGDisplayModeRef>(const_cast<void*>(CFArrayGetValueAtIndex(modeList, i)));
            if (dm == nullptr)
            {
                continue;
            }

            Mode m{};
            m.width = static_cast<uint32>(CGDisplayModeGetWidth(dm));
            m.height = static_cast<uint32>(CGDisplayModeGetHeight(dm));
            m.refreshRate = static_cast<float32>(CGDisplayModeGetRefreshRate(dm));
            m.pixelDensity = 1.0f;
            modes.append(m);
        }

        CFRelease(modeList);
        return modes;
    }

    Option<Mode> Monitors::getDesktopMode(MonitorId id)
    {
        CGDirectDisplayID displayId = static_cast<CGDirectDisplayID>(id);
        CGDisplayModeRef dm = CGDisplayCopyDisplayMode(displayId);
        if (dm == nullptr)
        {
            return none;
        }

        Mode m{};
        m.width = static_cast<uint32>(CGDisplayModeGetWidth(dm));
        m.height = static_cast<uint32>(CGDisplayModeGetHeight(dm));
        m.refreshRate = static_cast<float32>(CGDisplayModeGetRefreshRate(dm));
        m.pixelDensity = 1.0f;
        CGDisplayModeRelease(dm);
        return m;
    }

    Option<Mode> Monitors::getCurrentMode(MonitorId id)
    {
        // CoreGraphics current mode is the desktop mode
        return getDesktopMode(id);
    }
}

#endif
