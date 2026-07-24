#if defined(ARK_PLATFORM_LINUX) && !defined(ARK_PLATFORM_WEBASSEMBLY)

#include "Ark/Display/Platform/Linux/Window.hpp"

#include "Ark/Application/Event.hpp"
#include "Ark/Application/EventDispatcher.hpp"
#include "Ark/Display/Platform/Linux/KeyMapping.hpp"
#include "Ark/Display/Platform/Linux/X11Context.hpp"
#include "Ark/Display/Surface.hpp"
#include "Ark/Strings/String.hpp"

#include <X11/Xatom.h>
#include <X11/Xutil.h>

#include <cstring>

namespace Ark::Display::Platform::Linux
{
    class LinuxWindow final : public Window
    {
    public:
        EventDispatcher* eventDispatcher = nullptr;

        explicit LinuxWindow(WindowCreateInfo const& info)
            : eventDispatcher(info.eventDispatcher)
            , title(info.title)
            , flags(info.flags)
            , parentWindowId(info.parentId)
        {
            X11Context& ctx = X11Context::get();
            if (!ctx.isValid())
            {
                return;
            }

            width = info.width == 0 ? 1280u : info.width;
            height = info.height == 0 ? 720u : info.height;

            XSetWindowAttributes attrs{};
            attrs.event_mask = ExposureMask | StructureNotifyMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | FocusChangeMask | EnterWindowMask | LeaveWindowMask | PropertyChangeMask;
            attrs.background_pixel = BlackPixel(ctx.display, ctx.screen);
            attrs.border_pixel = BlackPixel(ctx.display, ctx.screen);

            unsigned long valueMask = CWEventMask | CWBackPixel | CWBorderPixel;

            int posX = 0;
            int posY = 0;
            if (info.position == WindowPosition::Custom)
            {
                posX = info.positionX ? *info.positionX : 0;
                posY = info.positionY ? *info.positionY : 0;
            }
            else if (info.position == WindowPosition::Centered)
            {
                posX = (DisplayWidth(ctx.display, ctx.screen) - static_cast<int>(width)) / 2;
                posY = (DisplayHeight(ctx.display, ctx.screen) - static_cast<int>(height)) / 2;
            }

            ::Window parent = ctx.root;
            if (info.parentId)
            {
                parent = static_cast<::Window>(*info.parentId);
            }

            xid = XCreateWindow(
                ctx.display,
                parent,
                posX,
                posY,
                width,
                height,
                0,
                CopyFromParent,
                InputOutput,
                CopyFromParent,
                valueMask,
                &attrs);

            if (xid == 0)
            {
                return;
            }

            XStoreName(ctx.display, xid, title.asPointer());
            if (ctx.netWmName != 0 && ctx.utf8String != 0)
            {
                XChangeProperty(
                    ctx.display,
                    xid,
                    ctx.netWmName,
                    ctx.utf8String,
                    8,
                    PropModeReplace,
                    reinterpret_cast<unsigned char const*>(title.asPointer()),
                    static_cast<int>(title.getLength()));
            }

            Atom protocols[] = {ctx.wmDelete};
            XSetWMProtocols(ctx.display, xid, protocols, 1);

            applySizeHints();

            if ((flags & WindowFlags::Hidden) == WindowFlags::None)
            {
                XMapWindow(ctx.display, xid);
                visible = true;
            }

            if ((flags & WindowFlags::Fullscreen) != WindowFlags::None)
            {
                setFullscreen(true, none);
            }

            ctx.registerWindow(xid, this);
            XFlush(ctx.display);
        }

        ~LinuxWindow() override
        {
            destroy();
        }

        void handleEvent(XEvent const& event)
        {
            switch (event.type)
            {
                case ClientMessage:
                    if (event.xclient.message_type == X11Context::get().wmProtocols && static_cast<Atom>(event.xclient.data.l[0]) == X11Context::get().wmDelete)
                    {
                        post(WindowCloseRequestedEvent{.windowId = getId()});
                    }
                    break;

                case DestroyNotify:
                    open = false;
                    post(WindowDestroyedEvent{.windowId = getId()});
                    break;

                case ConfigureNotify:
                    if (static_cast<uint32>(event.xconfigure.width) != width || static_cast<uint32>(event.xconfigure.height) != height)
                    {
                        width = static_cast<uint32>(event.xconfigure.width);
                        height = static_cast<uint32>(event.xconfigure.height);
                        post(WindowResizedEvent{.windowId = getId(), .width = width, .height = height});
                    }
                    posX = event.xconfigure.x;
                    posY = event.xconfigure.y;
                    break;

                case MapNotify:
                    visible = true;
                    post(WindowShownEvent{.windowId = getId()});
                    break;

                case UnmapNotify:
                    visible = false;
                    post(WindowHiddenEvent{.windowId = getId()});
                    break;

                case FocusIn:
                    focused = true;
                    post(WindowFocusGainedEvent{.windowId = getId()});
                    break;

                case FocusOut:
                    focused = false;
                    post(WindowFocusLostEvent{.windowId = getId()});
                    break;

                case KeyPress:
                case KeyRelease:
                {
                    KeySym keySym = XLookupKeysym(const_cast<XKeyEvent*>(&event.xkey), 0);
                    Key key = keySymToKey(keySym);
                    KeyModifiers modifiers = stateToModifiers(event.xkey.state);
                    bool const isDown = event.type == KeyPress;
                    bool const repeat = isDown && event.xkey.same_screen; // X11 doesn't expose repeat cleanly; keep false via unused
                    (void)repeat;
                    if (isDown)
                    {
                        post(KeyDownEvent{
                            .windowId = getId(),
                            .scancode = Scancode::Unknown,
                            .key = key,
                            .modifiers = modifiers,
                            .repeat = false,
                        });
                    }
                    else
                    {
                        post(KeyUpEvent{
                            .windowId = getId(),
                            .scancode = Scancode::Unknown,
                            .key = key,
                            .modifiers = modifiers,
                        });
                    }
                    break;
                }

                case ButtonPress:
                case ButtonRelease:
                {
                    unsigned int button = event.xbutton.button;
                    if (button == Button4 || button == Button5)
                    {
                        if (event.type == ButtonPress)
                        {
                            post(MouseWheelEvent{
                                .windowId = getId(),
                                .x = 0.0f,
                                .y = (button == Button4) ? 1.0f : -1.0f,
                            });
                        }
                        break;
                    }

                    MouseButton mouseButton = xButtonToMouse(button);
                    if (event.type == ButtonPress)
                    {
                        post(MouseButtonDownEvent{
                            .windowId = getId(),
                            .button = mouseButton,
                            .x = event.xbutton.x,
                            .y = event.xbutton.y,
                        });
                    }
                    else
                    {
                        post(MouseButtonUpEvent{
                            .windowId = getId(),
                            .button = mouseButton,
                            .x = event.xbutton.x,
                            .y = event.xbutton.y,
                        });
                    }
                    break;
                }

                case MotionNotify:
                    post(MouseMotionEvent{
                        .windowId = getId(),
                        .x = event.xmotion.x,
                        .y = event.xmotion.y,
                        .dx = event.xmotion.x - lastMouseX,
                        .dy = event.xmotion.y - lastMouseY,
                        .state = stateToMouseButtons(event.xmotion.state),
                    });
                    lastMouseX = event.xmotion.x;
                    lastMouseY = event.xmotion.y;
                    break;

                case Expose:
                    invalidate();
                    break;

                default:
                    break;
            }
        }

        WindowId getId() const override
        {
            return static_cast<WindowId>(xid);
        }

        WindowFlags getFlags() const override
        {
            return flags;
        }

        void* getNativeHandle() const override
        {
            return reinterpret_cast<void*>(static_cast<UIntPtr>(xid));
        }

        void show() override
        {
            X11Context& ctx = X11Context::get();
            if (!ctx.isValid() || xid == 0)
            {
                return;
            }
            XMapWindow(ctx.display, xid);
            XFlush(ctx.display);
            flags &= ~WindowFlags::Hidden;
        }

        void hide() override
        {
            X11Context& ctx = X11Context::get();
            if (!ctx.isValid() || xid == 0)
            {
                return;
            }
            XUnmapWindow(ctx.display, xid);
            XFlush(ctx.display);
            flags |= WindowFlags::Hidden;
        }

        void setTitle(String const& newTitle) override
        {
            title = newTitle;
            X11Context& ctx = X11Context::get();
            if (!ctx.isValid() || xid == 0)
            {
                return;
            }
            XStoreName(ctx.display, xid, title.asPointer());
            if (ctx.netWmName != 0 && ctx.utf8String != 0)
            {
                XChangeProperty(
                    ctx.display,
                    xid,
                    ctx.netWmName,
                    ctx.utf8String,
                    8,
                    PropModeReplace,
                    reinterpret_cast<unsigned char const*>(title.asPointer()),
                    static_cast<int>(title.getLength()));
            }
            XFlush(ctx.display);
        }

        String getTitle() const override
        {
            return title;
        }

        void setPosition(int32 x, int32 y) override
        {
            X11Context& ctx = X11Context::get();
            if (!ctx.isValid() || xid == 0)
            {
                return;
            }
            XMoveWindow(ctx.display, xid, x, y);
            XFlush(ctx.display);
            posX = x;
            posY = y;
        }

        void getPosition(int32& x, int32& y) const override
        {
            x = posX;
            y = posY;
        }

        void setSize(uint32 w, uint32 h) override
        {
            X11Context& ctx = X11Context::get();
            if (!ctx.isValid() || xid == 0)
            {
                return;
            }
            width = w == 0 ? 1 : w;
            height = h == 0 ? 1 : h;
            XResizeWindow(ctx.display, xid, width, height);
            applySizeHints();
            XFlush(ctx.display);
        }

        void getSize(uint32& w, uint32& h) const override
        {
            w = width;
            h = height;
        }

        void getSizeInPixels(uint32& w, uint32& h) const override
        {
            w = width;
            h = height;
        }

        void getBordersSize(int32& left, int32& top, int32& right, int32& bottom) const override
        {
            left = top = right = bottom = 0;
        }

        float32 getDisplayScale() const override
        {
            return 1.0f;
        }

        void setMinimumSize(uint32 w, uint32 h) override
        {
            minW = w;
            minH = h;
            applySizeHints();
        }

        void getMinimumSize(uint32& w, uint32& h) const override
        {
            w = minW;
            h = minH;
        }

        void setMaximumSize(uint32 w, uint32 h) override
        {
            maxW = w;
            maxH = h;
            applySizeHints();
        }

        void getMaximumSize(uint32& w, uint32& h) const override
        {
            w = maxW;
            h = maxH;
        }

        void setAspectRatio(uint32 w, uint32 h) override
        {
            arW = w;
            arH = h;
            applySizeHints();
        }

        void getAspectRatio(uint32& w, uint32& h) const override
        {
            w = arW;
            h = arH;
        }

        void setOpacity(float32 value) override
        {
            opacity = value < 0.0f ? 0.0f : (value > 1.0f ? 1.0f : value);
            X11Context& ctx = X11Context::get();
            if (!ctx.isValid() || xid == 0)
            {
                return;
            }
            Atom opacityAtom = XInternAtom(ctx.display, "_NET_WM_WINDOW_OPACITY", False);
            unsigned long opacityValue = static_cast<unsigned long>(opacity * 0xFFFFFFFFul);
            XChangeProperty(ctx.display, xid, opacityAtom, XA_CARDINAL, 32, PropModeReplace, reinterpret_cast<unsigned char*>(&opacityValue), 1);
            XFlush(ctx.display);
        }

        float32 getOpacity() const override
        {
            return opacity;
        }

        void setResizable(bool resizable) override
        {
            if (resizable)
            {
                flags |= WindowFlags::Resizable;
            }
            else
            {
                flags &= ~WindowFlags::Resizable;
            }
            applySizeHints();
        }

        void setFocusable(bool focusable) override
        {
            if (focusable)
            {
                flags &= ~WindowFlags::NotFocusable;
            }
            else
            {
                flags |= WindowFlags::NotFocusable;
            }
        }

        void minimize() override
        {
            X11Context& ctx = X11Context::get();
            if (!ctx.isValid() || xid == 0)
            {
                return;
            }
            XIconifyWindow(ctx.display, xid, ctx.screen);
            XFlush(ctx.display);
            flags |= WindowFlags::Minimized;
            flags &= ~WindowFlags::Maximized;
        }

        void maximize() override
        {
            setNetWmState(true, XInternAtom(X11Context::get().display, "_NET_WM_STATE_MAXIMIZED_VERT", False), XInternAtom(X11Context::get().display, "_NET_WM_STATE_MAXIMIZED_HORZ", False));
            flags |= WindowFlags::Maximized;
            flags &= ~WindowFlags::Minimized;
        }

        void restore() override
        {
            setNetWmState(false, XInternAtom(X11Context::get().display, "_NET_WM_STATE_MAXIMIZED_VERT", False), XInternAtom(X11Context::get().display, "_NET_WM_STATE_MAXIMIZED_HORZ", False));
            flags &= ~(WindowFlags::Maximized | WindowFlags::Minimized);
            show();
        }

        void raise() override
        {
            X11Context& ctx = X11Context::get();
            if (!ctx.isValid() || xid == 0)
            {
                return;
            }
            XRaiseWindow(ctx.display, xid);
            XSetInputFocus(ctx.display, xid, RevertToParent, CurrentTime);
            XFlush(ctx.display);
        }

        void flash() override
        {
            X11Context& ctx = X11Context::get();
            if (!ctx.isValid() || xid == 0)
            {
                return;
            }
            XBell(ctx.display, 0);
        }

        void setAlwaysOnTop(bool enabled) override
        {
            Atom above = XInternAtom(X11Context::get().display, "_NET_WM_STATE_ABOVE", False);
            setNetWmState(enabled, above, 0);
            if (enabled)
            {
                flags |= WindowFlags::AlwaysOnTop;
            }
            else
            {
                flags &= ~WindowFlags::AlwaysOnTop;
            }
        }

        void setBordered(bool bordered) override
        {
            if (bordered)
            {
                flags &= ~WindowFlags::Borderless;
            }
            else
            {
                flags |= WindowFlags::Borderless;
            }
            // Motif hints would be needed for true borderless; skip for baseline support.
        }

        void setFullscreen(bool enabled, Option<Mode> /*mode*/) override
        {
            X11Context& ctx = X11Context::get();
            if (!ctx.isValid() || xid == 0)
            {
                return;
            }
            setNetWmState(enabled, ctx.netWmStateFullscreen, 0);
            if (enabled)
            {
                flags |= WindowFlags::Fullscreen;
            }
            else
            {
                flags &= ~WindowFlags::Fullscreen;
            }
            fullscreen = enabled;
        }

        bool isFullscreen() const override
        {
            return fullscreen;
        }

        void setMouseGrab(bool grabbed) override
        {
            X11Context& ctx = X11Context::get();
            if (!ctx.isValid() || xid == 0)
            {
                return;
            }
            if (grabbed)
            {
                XGrabPointer(ctx.display, xid, True, ButtonPressMask | ButtonReleaseMask | PointerMotionMask, GrabModeAsync, GrabModeAsync, xid, None, CurrentTime);
            }
            else
            {
                XUngrabPointer(ctx.display, CurrentTime);
            }
            mouseGrabbed = grabbed;
        }

        bool getMouseGrab() const override
        {
            return mouseGrabbed;
        }

        void setKeyboardGrab(bool grabbed) override
        {
            X11Context& ctx = X11Context::get();
            if (!ctx.isValid() || xid == 0)
            {
                return;
            }
            if (grabbed)
            {
                XGrabKeyboard(ctx.display, xid, True, GrabModeAsync, GrabModeAsync, CurrentTime);
            }
            else
            {
                XUngrabKeyboard(ctx.display, CurrentTime);
            }
            keyboardGrabbed = grabbed;
        }

        bool getKeyboardGrab() const override
        {
            return keyboardGrabbed;
        }

        bool isMinimized() const override
        {
            return (flags & WindowFlags::Minimized) != WindowFlags::None;
        }

        bool isMaximized() const override
        {
            return (flags & WindowFlags::Maximized) != WindowFlags::None;
        }

        bool isVisible() const override
        {
            return visible;
        }

        bool hasFocus() const override
        {
            return focused;
        }

        Option<WindowId> getParentId() const override
        {
            return parentWindowId;
        }

        bool isOpen() const override
        {
            return open && xid != 0;
        }

        void close() override
        {
            destroy();
        }

        void presentSurface(Surface const& surface) override
        {
            X11Context& ctx = X11Context::get();
            if (!ctx.isValid() || xid == 0 || surface.pixels.isEmpty() || surface.width == 0 || surface.height == 0)
            {
                return;
            }

            uint32 const srcStride = surface.strideBytes != 0 ? surface.strideBytes : surface.width * 4u;
            uint32 const dstStride = surface.width * 4u;
            usize const byteCount = static_cast<usize>(dstStride) * surface.height;

            if (presentBufferSize < byteCount)
            {
                delete[] presentBuffer;
                presentBuffer = new uint8[byteCount];
                presentBufferSize = byteCount;
            }

            uint8 const* src = surface.pixels.asPointer();
            for (uint32 y = 0; y < surface.height; ++y)
            {
                uint8 const* srcRow = src + static_cast<usize>(y) * srcStride;
                uint8* dstRow = presentBuffer + static_cast<usize>(y) * dstStride;
                for (uint32 x = 0; x < surface.width; ++x)
                {
                    uint8 r = srcRow[x * 4 + 0];
                    uint8 g = srcRow[x * 4 + 1];
                    uint8 b = srcRow[x * 4 + 2];
                    uint8 a = srcRow[x * 4 + 3];
                    // X11 TrueColor little-endian typically expects BGRA in the 32-bit word.
                    dstRow[x * 4 + 0] = b;
                    dstRow[x * 4 + 1] = g;
                    dstRow[x * 4 + 2] = r;
                    dstRow[x * 4 + 3] = a;
                }
            }

            Visual* visual = DefaultVisual(ctx.display, ctx.screen);
            int depth = DefaultDepth(ctx.display, ctx.screen);
            XImage* image = XCreateImage(
                ctx.display,
                visual,
                static_cast<unsigned int>(depth),
                ZPixmap,
                0,
                reinterpret_cast<char*>(presentBuffer),
                surface.width,
                surface.height,
                32,
                static_cast<int>(dstStride));
            if (image == nullptr)
            {
                return;
            }

            GC gc = XCreateGC(ctx.display, xid, 0, nullptr);
            XPutImage(ctx.display, xid, gc, image, 0, 0, 0, 0, surface.width, surface.height);
            XFreeGC(ctx.display, gc);
            image->data = nullptr; // owned by presentBuffer
            XDestroyImage(image);
            XFlush(ctx.display);
        }

        void invalidate() override
        {
            X11Context& ctx = X11Context::get();
            if (!ctx.isValid() || xid == 0)
            {
                return;
            }
            XClearArea(ctx.display, xid, 0, 0, 0, 0, True);
            XFlush(ctx.display);
        }

    private:
        template <typename T>
        void post(T&& event)
        {
            if (eventDispatcher != nullptr)
            {
                eventDispatcher->enqueueEvent(static_cast<T&&>(event));
            }
        }

        void applySizeHints()
        {
            X11Context& ctx = X11Context::get();
            if (!ctx.isValid() || xid == 0)
            {
                return;
            }

            XSizeHints hints{};
            hints.flags = PMinSize | PMaxSize;
            if ((flags & WindowFlags::Resizable) == WindowFlags::None)
            {
                hints.min_width = hints.max_width = static_cast<int>(width);
                hints.min_height = hints.max_height = static_cast<int>(height);
            }
            else
            {
                hints.min_width = static_cast<int>(minW);
                hints.min_height = static_cast<int>(minH);
                hints.max_width = maxW == 0 ? 32767 : static_cast<int>(maxW);
                hints.max_height = maxH == 0 ? 32767 : static_cast<int>(maxH);
            }
            if (arW != 0 && arH != 0)
            {
                hints.flags |= PAspect;
                hints.min_aspect.x = hints.max_aspect.x = static_cast<int>(arW);
                hints.min_aspect.y = hints.max_aspect.y = static_cast<int>(arH);
            }
            XSetWMNormalHints(ctx.display, xid, &hints);
        }

        void setNetWmState(bool enable, Atom atom1, Atom atom2)
        {
            X11Context& ctx = X11Context::get();
            if (!ctx.isValid() || xid == 0 || ctx.netWmState == 0)
            {
                return;
            }

            XEvent event{};
            event.xclient.type = ClientMessage;
            event.xclient.window = xid;
            event.xclient.message_type = ctx.netWmState;
            event.xclient.format = 32;
            event.xclient.data.l[0] = enable ? 1 : 0;
            event.xclient.data.l[1] = static_cast<long>(atom1);
            event.xclient.data.l[2] = static_cast<long>(atom2);
            event.xclient.data.l[3] = 1;
            XSendEvent(ctx.display, ctx.root, False, SubstructureRedirectMask | SubstructureNotifyMask, &event);
            XFlush(ctx.display);
        }

        void destroy()
        {
            X11Context& ctx = X11Context::get();
            if (xid != 0 && ctx.isValid())
            {
                ctx.unregisterWindow(xid);
                XDestroyWindow(ctx.display, xid);
                XFlush(ctx.display);
                xid = 0;
            }
            open = false;
            delete[] presentBuffer;
            presentBuffer = nullptr;
            presentBufferSize = 0;
        }

        ::Window xid = 0;
        String title;
        WindowFlags flags = WindowFlags::None;
        Option<WindowId> parentWindowId;
        uint32 width = 1280;
        uint32 height = 720;
        int32 posX = 0;
        int32 posY = 0;
        uint32 minW = 0;
        uint32 minH = 0;
        uint32 maxW = 0;
        uint32 maxH = 0;
        uint32 arW = 0;
        uint32 arH = 0;
        float32 opacity = 1.0f;
        bool visible = false;
        bool focused = false;
        bool open = true;
        bool fullscreen = false;
        bool mouseGrabbed = false;
        bool keyboardGrabbed = false;
        int32 lastMouseX = 0;
        int32 lastMouseY = 0;
        uint8* presentBuffer = nullptr;
        usize presentBufferSize = 0;
    };

    void dispatchXEvent(XEvent const& event)
    {
        ::Window target = 0;
        switch (event.type)
        {
            case KeyPress:
            case KeyRelease:
                target = event.xkey.window;
                break;
            case ButtonPress:
            case ButtonRelease:
                target = event.xbutton.window;
                break;
            case MotionNotify:
                target = event.xmotion.window;
                break;
            case ConfigureNotify:
                target = event.xconfigure.window;
                break;
            case MapNotify:
                target = event.xmap.window;
                break;
            case UnmapNotify:
                target = event.xunmap.window;
                break;
            case FocusIn:
            case FocusOut:
                target = event.xfocus.window;
                break;
            case Expose:
                target = event.xexpose.window;
                break;
            case ClientMessage:
                target = event.xclient.window;
                break;
            case DestroyNotify:
                target = event.xdestroywindow.window;
                break;
            default:
                target = event.xany.window;
                break;
        }

        LinuxWindow* window = X11Context::get().findWindow(target);
        if (window != nullptr)
        {
            window->handleEvent(event);
        }
    }

    Window* createWindow(WindowCreateInfo const& info)
    {
        if (!X11Context::get().isValid())
        {
            return nullptr;
        }
        auto* window = new LinuxWindow(info);
        if (window->getNativeHandle() == nullptr)
        {
            delete window;
            return nullptr;
        }
        return window;
    }
}

#endif
