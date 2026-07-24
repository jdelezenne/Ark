#pragma once

#include "Ark/Collections/Array.hpp"
#include "Ark/Core/Option.hpp"
#include "Ark/Core/Types.hpp"
#include "Ark/Display/Types.hpp"
#include "Ark/Math/Rect.hpp"
#include "Ark/Strings/String.hpp"

namespace Ark::Display
{
    struct Mode final
    {
        uint32 width = 0;
        uint32 height = 0;
        float32 pixelDensity = 1.0f;
        float32 refreshRate = 0.0f;
        int32 refreshRateNumerator = 0;
        int32 refreshRateDenominator = 0;
    };

    struct MonitorInfo final
    {
        String name;
        RectInt bounds;
        RectInt usableBounds;
        float32 contentScale = 1.0f;
        Orientation naturalOrientation = Orientation::Unknown;
        Orientation currentOrientation = Orientation::Unknown;
    };

    struct Monitors final
    {
        Monitors() = delete;

        /// Returns the IDs of all connected monitors. The first item is the primary monitor.
        static Collections::Array<MonitorId> enumerate();

        /// Returns the primary monitor ID when available.
        static Option<MonitorId> getPrimary();

        /// Returns the human-readable monitor name when available.
        static String getName(MonitorId id);

        /// Returns bounds, usable bounds, content scale and orientation for a monitor.
        static MonitorInfo getInfo(MonitorId id);

        /// Returns available fullscreen display modes for the monitor.
        static Collections::Array<Mode> getFullscreenModes(MonitorId id);

        /// Returns the desktop display mode.
        static Option<Mode> getDesktopMode(MonitorId id);

        /// Returns the current display mode.
        static Option<Mode> getCurrentMode(MonitorId id);
    };
}
