#pragma once

#include "Ark/Core/Types.hpp"

namespace Ark::Display
{
    using WindowId = UInt64;
    using MonitorId = UInt32;

    enum class Orientation : uint32
    {
        Unknown = 0,
        Landscape = 1,
        LandscapeFlipped = 2,
        Portrait = 3,
        PortraitFlipped = 4,
    };

    enum class WindowError
    {
        None = 0,
        CreationFailed,
        AlreadyExists,
        InvalidParameters,
        InvalidHandle,
        InvalidMonitor,
        UnsupportedFeature,
        SystemError,
    };

    enum class WindowMode
    {
        Windowed,
        Fullscreen,
        Borderless,
    };

    enum class FlashOperation
    {
        Cancel,
        Briefly,
        UntilFocused,
    };

    enum class ProgressState
    {
        Invalid = -1,
        None,
        Indeterminate,
        Normal,
        Paused,
        Error,
    };

    enum class HitTestResult
    {
        Normal,
        Draggable,
        ResizeTopLeft,
        ResizeTop,
        ResizeTopRight,
        ResizeRight,
        ResizeBottomRight,
        ResizeBottom,
        ResizeBottomLeft,
        ResizeLeft,
    };
}

/// Compatibility aliases for the former Ark::Windowing enums (now owned by Display).
namespace Ark::Windowing
{
    using WindowError = Display::WindowError;
    using WindowMode = Display::WindowMode;
    using DisplayOrientation = Display::Orientation;
    using FlashOperation = Display::FlashOperation;
    using ProgressState = Display::ProgressState;
    using HitTestResult = Display::HitTestResult;
}
