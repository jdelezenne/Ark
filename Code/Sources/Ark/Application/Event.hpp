#pragma once

#include "Ark/Core/Types.hpp"
#include "Ark/Core/Variant.hpp"
#include "Ark/Display/Types.hpp"
#include "Ark/Strings/String.hpp"
#include "Ark/System/Key.hpp"
#include "Ark/System/Mouse.hpp"
#include "Ark/System/Scancode.hpp"

namespace Ark
{
    /// Application quit request event.
    /// Signaled when the user requests application shutdown (e.g., window close button, Cmd+Q).
    struct QuitEvent final
    {
    };

    /// Locale/language settings changed event.
    /// Signaled when the system locale changes.
    struct LocaleChangedEvent final
    {
    };

    /// System theme changed event (light/dark mode).
    /// Signaled when the OS theme setting changes.
    struct ThemeChangedEvent final
    {
    };

    /// Display orientation changed event.
    /// Signaled when a display is rotated (portrait/landscape).
    struct DisplayOrientationChangedEvent final
    {
        /// Display ID.
        uint32 displayId = 0;
        /// New orientation.
        Display::Orientation orientation = Display::Orientation::Unknown;
    };

    /// Display connected/added event.
    /// Signaled when a display is detected (e.g., monitor plugged in).
    struct DisplayAddedEvent final
    {
        /// ID of the newly added display.
        uint32 displayId = 0;
    };

    /// Display disconnected/removed event.
    /// Signaled when a display is disconnected (e.g., monitor unplugged).
    struct DisplayRemovedEvent final
    {
        /// ID of the removed display.
        uint32 displayId = 0;
    };

    /// Display position/offset changed event.
    /// Signaled when a display's position changes in the desktop arrangement.
    struct DisplayMovedEvent final
    {
        /// Display ID.
        uint32 displayId = 0;
        /// New X position.
        int32 x = 0;
        /// New Y position.
        int32 y = 0;
    };

    /// Display resolution/mode changed event.
    /// Signaled when a display's resolution or refresh rate changes.
    struct DisplayModeChangedEvent final
    {
        /// Display ID.
        uint32 displayId = 0;
    };

    /// Display scale factor changed event (DPI).
    /// Signaled when a display's DPI scaling changes.
    struct DisplayScaleChangedEvent final
    {
        /// Display ID.
        uint32 displayId = 0;
        /// New DPI scale factor.
        float32 scale = 1.0f;
    };

    /// Window shown event.
    /// Signaled when a window becomes visible.
    struct WindowShownEvent final
    {
        /// Window ID.
        Display::WindowId windowId = 0;
    };

    /// Window hidden event.
    /// Signaled when a window becomes invisible.
    struct WindowHiddenEvent final
    {
        /// Window ID.
        Display::WindowId windowId = 0;
    };

    /// Window exposed/needs redraw event.
    /// Signaled when a window's content area is exposed and needs repainting.
    struct WindowExposedEvent final
    {
        /// Window ID.
        Display::WindowId windowId = 0;
    };

    /// Window moved event.
    /// Signaled when the window is repositioned.
    struct WindowMovedEvent final
    {
        /// Window ID.
        Display::WindowId windowId = 0;
        /// New X position in logical pixels.
        int32 x = 0;
        /// New Y position in logical pixels.
        int32 y = 0;
    };

    /// Window resized event.
    /// Signaled when the window size changes in logical pixels.
    struct WindowResizedEvent final
    {
        /// Window ID.
        Display::WindowId windowId = 0;
        /// New width in logical pixels.
        uint32 width = 0;
        /// New height in logical pixels.
        uint32 height = 0;
    };

    /// Window pixel size changed event.
    /// Signaled when the window physical pixel size changes (DPI-aware).
    struct WindowPixelSizeChangedEvent final
    {
        /// Window ID.
        Display::WindowId windowId = 0;
        /// New width in physical pixels.
        uint32 width = 0;
        /// New height in physical pixels.
        uint32 height = 0;
    };

    /// Window minimized event.
    /// Signaled when the window is minimized.
    struct WindowMinimizedEvent final
    {
        /// Window ID.
        Display::WindowId windowId = 0;
    };

    /// Window maximized event.
    /// Signaled when the window is maximized.
    struct WindowMaximizedEvent final
    {
        /// Window ID.
        Display::WindowId windowId = 0;
    };

    /// Window restored event.
    /// Signaled when the window is restored from minimized/maximized state.
    struct WindowRestoredEvent final
    {
        /// Window ID.
        Display::WindowId windowId = 0;
    };

    /// Mouse entered window event.
    /// Signaled when the mouse pointer enters the window.
    struct WindowMouseEnterEvent final
    {
        /// Window ID.
        Display::WindowId windowId = 0;
    };

    /// Mouse left window event.
    /// Signaled when the mouse pointer leaves the window.
    struct WindowMouseLeaveEvent final
    {
        /// Window ID.
        Display::WindowId windowId = 0;
    };

    /// Window gained input focus event.
    /// Signaled when the window receives keyboard focus.
    struct WindowFocusGainedEvent final
    {
        /// Window ID.
        Display::WindowId windowId = 0;
    };

    /// Window lost input focus event.
    /// Signaled when the window loses keyboard focus.
    struct WindowFocusLostEvent final
    {
        /// Window ID.
        Display::WindowId windowId = 0;
    };

    /// Window close requested event.
    /// Signaled when the user requests window closure (e.g., close button clicked).
    struct WindowCloseRequestedEvent final
    {
        /// Window ID.
        Display::WindowId windowId = 0;
    };

    /// Window closed event.
    /// Signaled when the window is actually closed.
    struct WindowClosedEvent final
    {
        /// Window ID.
        Display::WindowId windowId = 0;
    };

    /// Window moved to different display event.
    /// Signaled when the window is moved to a different display.
    struct WindowDisplayChangedEvent final
    {
        /// Window ID.
        Display::WindowId windowId = 0;
        /// New display ID.
        uint32 displayId = 0;
    };

    /// Window display scale changed event.
    /// Signaled when the window's display DPI scale changes.
    struct WindowDisplayScaleChangedEvent final
    {
        /// Window ID.
        Display::WindowId windowId = 0;
        /// New scale factor.
        float32 scale = 1.0f;
    };

    /// Window occluded/hidden by other windows event.
    /// Signaled when the window is completely hidden behind other windows.
    struct WindowOccludedEvent final
    {
        /// Window ID.
        Display::WindowId windowId = 0;
    };

    /// Window entered fullscreen event.
    /// Signaled when the window enters fullscreen mode.
    struct WindowEnteredFullscreenEvent final
    {
        /// Window ID.
        Display::WindowId windowId = 0;
    };

    /// Window left fullscreen event.
    /// Signaled when the window exits fullscreen mode.
    struct WindowLeftFullscreenEvent final
    {
        /// Window ID.
        Display::WindowId windowId = 0;
    };

    /// Window destroyed event.
    /// Signaled when a window is destroyed and resources are released.
    struct WindowDestroyedEvent final
    {
        /// Window ID.
        Display::WindowId windowId = 0;
    };

    /// Key press event.
    /// Signaled when a key is pressed.
    struct KeyDownEvent final
    {
        /// Window ID.
        Display::WindowId windowId = 0;
        /// Keyboard device ID.
        uint32 keyboardId = 0;
        /// Hardware scancode (physical key position).
        Scancode scancode = Scancode::Unknown;
        /// Logical key identifier.
        Key key = Key::Unknown;
        /// Active modifier keys (Shift, Ctrl, Alt, etc.).
        KeyModifiers modifiers = KeyModifiers::None;
        /// True if this is a key repeat from holding the key.
        bool repeat = false;
    };

    /// Key release event.
    /// Signaled when a key is released.
    struct KeyUpEvent final
    {
        /// Window ID.
        Display::WindowId windowId = 0;
        /// Keyboard device ID.
        uint32 keyboardId = 0;
        /// Hardware scancode (physical key position).
        Scancode scancode = Scancode::Unknown;
        /// Logical key identifier.
        Key key = Key::Unknown;
        /// Active modifier keys (Shift, Ctrl, Alt, etc.).
        KeyModifiers modifiers = KeyModifiers::None;
    };

    /// Text input event.
    /// Signaled for user text input (respects keyboard layout/IME).
    struct TextInputEvent final
    {
        /// Window ID.
        Display::WindowId windowId = 0;
        /// UTF-8 text input.
        String text;
    };

    /// Text editing event (IME composition).
    /// Signaled during IME (Input Method Editor) text composition.
    struct TextEditingEvent final
    {
        /// Window ID.
        Display::WindowId windowId = 0;
        /// Editing text being composed.
        String text;
        /// Start of selection in the composition.
        int32 start = 0;
        /// Length of selection in the composition.
        int32 length = 0;
    };

    /// Mouse motion event.
    /// Signaled when the mouse moves.
    struct MouseMotionEvent final
    {
        /// Window ID.
        Display::WindowId windowId = 0;
        /// Mouse device ID.
        uint32 mouseId = 0;
        /// X coordinate in window-relative pixels.
        int32 x = 0;
        /// Y coordinate in window-relative pixels.
        int32 y = 0;
        /// Change in X since last event.
        int32 dx = 0;
        /// Change in Y since last event.
        int32 dy = 0;
        /// Currently pressed buttons.
        MouseButtons state = MouseButtons::None;
    };

    /// Mouse button press event.
    /// Signaled when a mouse button is pressed.
    struct MouseButtonDownEvent final
    {
        /// Window ID.
        Display::WindowId windowId = 0;
        /// Mouse device ID.
        uint32 mouseId = 0;
        /// Button that was pressed.
        MouseButton button = MouseButton::Left;
        /// Click count (1=single, 2=double, etc.).
        int32 clicks = 1;
        /// X coordinate in window-relative pixels.
        int32 x = 0;
        /// Y coordinate in window-relative pixels.
        int32 y = 0;
    };

    /// Mouse button release event.
    /// Signaled when a mouse button is released.
    struct MouseButtonUpEvent final
    {
        /// Window ID.
        Display::WindowId windowId = 0;
        /// Mouse device ID.
        uint32 mouseId = 0;
        /// Button that was released.
        MouseButton button = MouseButton::Left;
        /// Click count (1=single, 2=double, etc.).
        int32 clicks = 1;
        /// X coordinate in window-relative pixels.
        int32 x = 0;
        /// Y coordinate in window-relative pixels.
        int32 y = 0;
    };

    /// Mouse wheel scroll event.
    /// Signaled when the mouse wheel is scrolled.
    struct MouseWheelEvent final
    {
        /// Window ID.
        Display::WindowId windowId = 0;
        /// Mouse device ID.
        uint32 mouseId = 0;
        /// Scroll amount on horizontal axis.
        float32 x = 0.0f;
        /// Scroll amount on vertical axis.
        float32 y = 0.0f;
        /// Scroll direction (normal or flipped).
        MouseWheelDirection direction = MouseWheelDirection::Normal;
    };

    /// Pinch/magnify gesture event (trackpad/touch).
    /// Signaled when a magnify/pinch gesture is performed.
    struct GestureMagnifyEvent final
    {
        /// Window ID.
        Display::WindowId windowId = 0;
        /// Magnification factor (>1.0 = zoom in, <1.0 = zoom out).
        float32 magnitude = 0.0f;
    };

    /// Rotate gesture event (trackpad/touch).
    /// Signaled when a rotation gesture is performed.
    struct GestureRotateEvent final
    {
        /// Window ID.
        Display::WindowId windowId = 0;
        /// Rotation angle in degrees.
        float32 rotation = 0.0f;
    };

    /// Pan/swipe gesture event (trackpad/touch).
    /// Signaled when a pan/swipe gesture is performed.
    struct GesturePanEvent final
    {
        /// Window ID.
        Display::WindowId windowId = 0;
        /// X position.
        int32 x = 0;
        /// Y position.
        int32 y = 0;
        /// X delta since gesture start.
        int32 dx = 0;
        /// Y delta since gesture start.
        int32 dy = 0;
    };

    /// Two-finger tap gesture event (trackpad/touch).
    /// Signaled when two fingers tap simultaneously.
    struct GestureTwoFingerTapEvent final
    {
        /// Window ID.
        Display::WindowId windowId = 0;
    };

    /// Touch point information (trackpad/touch screen).
    struct TouchPoint final
    {
        /// Unique touch point ID.
        UInt64 id = 0;
        /// X coordinate in window-relative pixels.
        float32 x = 0.0f;
        /// Y coordinate in window-relative pixels.
        float32 y = 0.0f;
        /// X coordinate normalized to [0, 1] range.
        float32 normalizedX = 0.0f;
        /// Y coordinate normalized to [0, 1] range.
        float32 normalizedY = 0.0f;
        /// Touch pressure (if supported), 0.0-1.0.
        float32 pressure = 0.0f;
    };

    /// Touch began event.
    /// Signaled when one or more touch points start.
    struct TouchBeganEvent final
    {
        /// Window ID.
        Display::WindowId windowId = 0;
        /// Array of new touch points.
        Collections::Array<TouchPoint> touches;
    };

    /// Touch moved event.
    /// Signaled when touch points move.
    struct TouchMovedEvent final
    {
        /// Window ID.
        Display::WindowId windowId = 0;
        /// Array of moving touch points.
        Collections::Array<TouchPoint> touches;
    };

    /// Touch ended event.
    /// Signaled when touch points are released.
    struct TouchEndedEvent final
    {
        /// Window ID.
        Display::WindowId windowId = 0;
        /// Array of released touch points.
        Collections::Array<TouchPoint> touches;
    };

    /// Touch cancelled event.
    /// Signaled when touch is interrupted (e.g., system gesture).
    struct TouchCancelledEvent final
    {
        /// Window ID.
        Display::WindowId windowId = 0;
        /// Array of cancelled touch points.
        Collections::Array<TouchPoint> touches;
    };

    /// Clipboard updated event.
    /// Signaled when the system clipboard content changes.
    struct ClipboardUpdatedEvent final
    {
    };

    /// File dropped on window event.
    /// Signaled when files are dragged and dropped on the window.
    struct DropFileEvent final
    {
        /// Window ID.
        Display::WindowId windowId = 0;
        /// File path.
        String file;
        /// X coordinate of drop location in pixels.
        float32 x = 0.0f;
        /// Y coordinate of drop location in pixels.
        float32 y = 0.0f;
    };

    /// Text dropped on window event.
    /// Signaled when text is dragged and dropped on the window.
    struct DropTextEvent final
    {
        /// Window ID.
        Display::WindowId windowId = 0;
        /// Dropped text.
        String text;
        /// X coordinate of drop location in pixels.
        float32 x = 0.0f;
        /// Y coordinate of drop location in pixels.
        float32 y = 0.0f;
    };

    /// Drag and drop started event.
    /// Signaled when a drag operation begins over the window.
    struct DropBeginEvent final
    {
        /// Window ID.
        Display::WindowId windowId = 0;
    };

    /// Drag and drop completed event.
    /// Signaled when a drag operation ends.
    struct DropCompleteEvent final
    {
        /// Window ID.
        Display::WindowId windowId = 0;
    };

    using Event = Variant<
        QuitEvent,
        LocaleChangedEvent,
        ThemeChangedEvent,
        DisplayOrientationChangedEvent,
        DisplayAddedEvent,
        DisplayRemovedEvent,
        DisplayMovedEvent,
        DisplayModeChangedEvent,
        DisplayScaleChangedEvent,
        WindowShownEvent,
        WindowHiddenEvent,
        WindowExposedEvent,
        WindowMovedEvent,
        WindowResizedEvent,
        WindowPixelSizeChangedEvent,
        WindowMinimizedEvent,
        WindowMaximizedEvent,
        WindowRestoredEvent,
        WindowMouseEnterEvent,
        WindowMouseLeaveEvent,
        WindowFocusGainedEvent,
        WindowFocusLostEvent,
        WindowCloseRequestedEvent,
        WindowClosedEvent,
        WindowDisplayChangedEvent,
        WindowDisplayScaleChangedEvent,
        WindowOccludedEvent,
        WindowEnteredFullscreenEvent,
        WindowLeftFullscreenEvent,
        WindowDestroyedEvent,
        KeyDownEvent,
        KeyUpEvent,
        TextInputEvent,
        TextEditingEvent,
        MouseMotionEvent,
        MouseButtonDownEvent,
        MouseButtonUpEvent,
        MouseWheelEvent,
        GestureMagnifyEvent,
        GestureRotateEvent,
        GesturePanEvent,
        GestureTwoFingerTapEvent,
        TouchBeganEvent,
        TouchMovedEvent,
        TouchEndedEvent,
        TouchCancelledEvent,
        ClipboardUpdatedEvent,
        DropFileEvent,
        DropTextEvent,
        DropBeginEvent,
        DropCompleteEvent>;
}
