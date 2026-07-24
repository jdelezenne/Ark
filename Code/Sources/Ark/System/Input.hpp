#pragma once

#include "Ark/Core/Core.hpp"
#include "Ark/Math/Point2.hpp"
#include "Ark/System/Key.hpp"
#include "Ark/System/Mouse.hpp"

namespace Ark::System::Input
{
    using WindowHandle = void*;

    /// Gets the current state of a keyboard key
    /// @param key The key to query
    /// @return true if the key is currently pressed, false otherwise
    bool isKeyPressed(Key key);

    /// Gets the current state of a mouse button
    /// @param button The mouse button to query
    /// @return true if the button is currently pressed, false otherwise
    bool isMouseButtonPressed(MouseButton button);

    /// Gets the current mouse position in screen coordinates
    /// @return The mouse position relative to the screen's top-left corner
    Point2Int getMousePosition();

    /// Gets the mouse position relative to a window
    /// @param windowHandle Handle to the window (can be cast from platform-specific window types)
    /// @return The mouse position relative to the window's client area, or (0,0) if windowHandle is null
    Point2Int getMousePositionInWindow(WindowHandle windowHandle);

    /// Sets the mouse position in screen coordinates
    /// @param x The x coordinate relative to the screen's top-left corner
    /// @param y The y coordinate relative to the screen's top-left corner
    void setMousePosition(int32 x, int32 y);

    /// Sets the mouse position relative to a window
    /// @param windowHandle Handle to the window (can be cast from platform-specific window types)
    /// @param x The x coordinate relative to the window's client area
    /// @param y The y coordinate relative to the window's client area
    void setMousePositionInWindow(WindowHandle windowHandle, int32 x, int32 y);

    /// Gets the current state of all keyboard modifiers
    /// @return Flags indicating which modifier keys are currently pressed
    KeyModifiers getKeyModifiers();
}
