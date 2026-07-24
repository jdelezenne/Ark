#pragma once

#include "Ark/Core/Flags.hpp"
#include "Ark/Core/Option.hpp"
#include "Ark/Core/Types.hpp"
#include "Ark/Display/Monitor.hpp"
#include "Ark/Display/Types.hpp"
#include "Ark/Strings/String.hpp"

namespace Ark
{
    class EventDispatcher;
}

namespace Ark::Display
{
    class Window;
    struct Surface;

    /// Window flag bitmask enum.
    enum class WindowFlags : uint64
    {
        None = 0,
        Fullscreen = ARK_BIT(0),         ///< Window in fullscreen mode
        Hidden = ARK_BIT(1),             ///< Window is not visible
        Borderless = ARK_BIT(2),         ///< Window has no border
        Resizable = ARK_BIT(3),          ///< Window can be resized
        Minimized = ARK_BIT(4),          ///< Window is minimized
        Maximized = ARK_BIT(5),          ///< Window is maximized
        AlwaysOnTop = ARK_BIT(6),        ///< Window stays above others
        NotFocusable = ARK_BIT(7),       ///< Window cannot receive focus
        Modal = ARK_BIT(8),              ///< Window is modal
        HighPixelDensity = ARK_BIT(9),   ///< Window supports high DPI
        MouseGrabbed = ARK_BIT(10),      ///< Mouse confined to window
        KeyboardGrabbed = ARK_BIT(11),   ///< Keyboard confined to window
        Utility = ARK_BIT(12),           ///< Utility/tool window
        Tooltip = ARK_BIT(13),           ///< Tooltip window
        PopupMenu = ARK_BIT(14),         ///< Popup menu window
        Transparent = ARK_BIT(15),       ///< Window is transparent
        AcceptTouchEvents = ARK_BIT(16), ///< Window accepts touch input
    };

    ARK_FLAG_ENUM(WindowFlags)

    /// Window position hint.
    enum class WindowPosition
    {
        Unspecified, ///< Let OS choose position
        Custom,      ///< Use custom positionX/positionY
        Centered,    ///< Center on screen
    };

    /// Window creation parameters.
    struct WindowCreateInfo final
    {
        /// Window title bar text.
        String title;

        /// Initial width in logical pixels.
        uint32 width = 1280;
        /// Initial height in logical pixels.
        uint32 height = 720;

        /// Position behavior.
        WindowPosition position = WindowPosition::Unspecified;
        /// Custom X coordinate (when position == Custom).
        Option<int32> positionX;
        /// Custom Y coordinate (when position == Custom).
        Option<int32> positionY;

        /// Window behavior flags.
        WindowFlags flags = WindowFlags::None;
        /// Display/monitor to create window on.
        Option<MonitorId> display;

        /// Parent window ID (for child windows).
        Option<WindowId> parentId;

        /// Event dispatcher that receives window-generated events.
        /// Required for input/window notifications; when null, those events are dropped.
        /// Typically `&eventLoop.getDispatcher()` from Ark::EventLoop.
        EventDispatcher* eventDispatcher = nullptr;
    };

    /// Abstract window interface.
    /// Platform-specific implementations provide OS window management.
    class Window
    {
    public:
        /// Destroys the window.
        virtual ~Window() = default;

        /// Returns the window ID.
        virtual WindowId getId() const = 0;

        /// Returns the window flags.
        virtual WindowFlags getFlags() const = 0;

        /// Returns the native OS window handle (HWND, NSWindow*, etc.).
        virtual void* getNativeHandle() const = 0;

        /// Shows the window.
        virtual void show() = 0;

        /// Hides the window.
        virtual void hide() = 0;

        /// Sets the window title.
        /// @param title New title text.
        virtual void setTitle(String const& title) = 0;

        /// Gets the window title.
        virtual String getTitle() const = 0;

        /// Sets the window position in logical pixels.
        /// @param x X coordinate.
        /// @param y Y coordinate.
        virtual void setPosition(int32 x, int32 y) = 0;

        /// Gets the window position in logical pixels.
        /// @param x Output X coordinate.
        /// @param y Output Y coordinate.
        virtual void getPosition(int32& x, int32& y) const = 0;

        /// Sets the window size in logical pixels.
        /// @param width Width in logical pixels.
        /// @param height Height in logical pixels.
        virtual void setSize(uint32 width, uint32 height) = 0;

        /// Gets the window size in logical pixels.
        /// @param width Output width in logical pixels.
        /// @param height Output height in logical pixels.
        virtual void getSize(uint32& width, uint32& height) const = 0;

        /// Gets the window size in physical pixels (scaled by DPI).
        /// @param width Output width in physical pixels.
        /// @param height Output height in physical pixels.
        virtual void getSizeInPixels(uint32& width, uint32& height) const = 0;

        /// Gets the border/frame thickness.
        /// @param left Output left border width.
        /// @param top Output top border width.
        /// @param right Output right border width.
        /// @param bottom Output bottom border width.
        virtual void getBordersSize(int32& left, int32& top, int32& right, int32& bottom) const = 0;

        /// Gets the DPI scale factor (1.0 = 96 DPI, 2.0 = 192 DPI, etc.).
        virtual float32 getDisplayScale() const = 0;

        /// Sets the minimum window size.
        /// @param width Minimum width in logical pixels.
        /// @param height Minimum height in logical pixels.
        virtual void setMinimumSize(uint32 width, uint32 height) = 0;

        /// Gets the minimum window size.
        /// @param width Output minimum width.
        /// @param height Output minimum height.
        virtual void getMinimumSize(uint32& width, uint32& height) const = 0;

        /// Sets the maximum window size.
        /// @param width Maximum width in logical pixels.
        /// @param height Maximum height in logical pixels.
        virtual void setMaximumSize(uint32 width, uint32 height) = 0;

        /// Gets the maximum window size.
        /// @param width Output maximum width.
        /// @param height Output maximum height.
        virtual void getMaximumSize(uint32& width, uint32& height) const = 0;

        /// Sets the aspect ratio constraint (width:height).
        /// @param width Aspect ratio width.
        /// @param height Aspect ratio height.
        virtual void setAspectRatio(uint32 width, uint32 height) = 0;

        /// Gets the aspect ratio constraint (width:height).
        /// @param width Output aspect ratio width.
        /// @param height Output aspect ratio height.
        virtual void getAspectRatio(uint32& width, uint32& height) const = 0;

        /// Sets the window opacity (0.0 = transparent, 1.0 = opaque).
        /// @param opacity Alpha value in range [0, 1].
        virtual void setOpacity(float32 opacity) = 0;

        /// Gets the window opacity.
        virtual float32 getOpacity() const = 0;

        /// Sets whether the window is resizable.
        /// @param resizable True to allow resizing.
        virtual void setResizable(bool resizable) = 0;

        /// Sets whether the window can receive keyboard focus.
        /// @param focusable True to allow focus.
        virtual void setFocusable(bool focusable) = 0;

        /// Minimizes the window.
        virtual void minimize() = 0;

        /// Maximizes the window.
        virtual void maximize() = 0;

        /// Restores the window from minimized/maximized state.
        virtual void restore() = 0;

        /// Raises the window above others.
        virtual void raise() = 0;

        /// Flashes or briefly highlights the window.
        virtual void flash() = 0;

        /// Sets the window to always appear on top of other windows.
        /// @param enabled True to enable always-on-top.
        virtual void setAlwaysOnTop(bool enabled) = 0;

        /// Sets whether the window has a border/frame.
        /// @param bordered True to show border.
        virtual void setBordered(bool bordered) = 0;

        /// Sets fullscreen mode.
        /// @param enabled True to enter fullscreen.
        /// @param mode Optional fullscreen mode (video mode, etc.).
        virtual void setFullscreen(bool enabled, Option<Mode> mode = none) = 0;

        /// Queries fullscreen state.
        virtual bool isFullscreen() const = 0;

        /// Confines mouse cursor to the window.
        /// @param grabbed True to confine mouse.
        virtual void setMouseGrab(bool grabbed) = 0;

        /// Queries mouse grab state.
        virtual bool getMouseGrab() const = 0;

        /// Confines keyboard input to the window.
        /// @param grabbed True to confine keyboard.
        virtual void setKeyboardGrab(bool grabbed) = 0;

        /// Queries keyboard grab state.
        virtual bool getKeyboardGrab() const = 0;

        /// Queries minimized state.
        virtual bool isMinimized() const = 0;

        /// Queries maximized state.
        virtual bool isMaximized() const = 0;

        /// Queries visibility state.
        virtual bool isVisible() const = 0;

        /// Queries whether the window has keyboard focus.
        virtual bool hasFocus() const = 0;

        /// Gets the parent window ID (if this is a child window).
        virtual Option<WindowId> getParentId() const = 0;

        /// Queries whether the window is still open.
        virtual bool isOpen() const = 0;

        /// Closes the window.
        virtual void close() = 0;

        /// Presents pixel surface to the window.
        /// @param surface Surface data to display.
        virtual void presentSurface(struct Surface const& surface) = 0;

        /// Invalidates the window, triggering a redraw.
        virtual void invalidate() = 0;

        /// Creates a window with the given configuration.
        /// @param info Creation parameters.
        /// @return Pointer to created window (caller-owned).
        static Window* create(WindowCreateInfo const& info);
    };
}
