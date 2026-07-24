#if defined(ARK_PLATFORM_LINUX) && !defined(ARK_PLATFORM_WEBASSEMBLY)

#include "Ark/Display/Platform/Linux/X11Context.hpp"

namespace Ark::Display::Platform::Linux
{
    X11Context& X11Context::get()
    {
        static X11Context context = []
        {
            X11Context ctx{};
            XInitThreads();
            ctx.display = XOpenDisplay(nullptr);
            if (ctx.display == nullptr)
            {
                return ctx;
            }

            ctx.screen = DefaultScreen(ctx.display);
            ctx.root = RootWindow(ctx.display, ctx.screen);
            ctx.wmDelete = XInternAtom(ctx.display, "WM_DELETE_WINDOW", False);
            ctx.wmProtocols = XInternAtom(ctx.display, "WM_PROTOCOLS", False);
            ctx.netWmState = XInternAtom(ctx.display, "_NET_WM_STATE", False);
            ctx.netWmStateFullscreen = XInternAtom(ctx.display, "_NET_WM_STATE_FULLSCREEN", False);
            ctx.netWmName = XInternAtom(ctx.display, "_NET_WM_NAME", False);
            ctx.utf8String = XInternAtom(ctx.display, "UTF8_STRING", False);
            ctx.clipboard = XInternAtom(ctx.display, "CLIPBOARD", False);
            ctx.primary = XA_PRIMARY;
            ctx.targets = XInternAtom(ctx.display, "TARGETS", False);
            ctx.text = XInternAtom(ctx.display, "TEXT", False);
            return ctx;
        }();
        return context;
    }

    void X11Context::registerWindow(::Window xid, LinuxWindow* window)
    {
        if (windowCount >= MaxWindows || xid == 0 || window == nullptr)
        {
            return;
        }
        xids[windowCount] = xid;
        windows[windowCount] = window;
        ++windowCount;
    }

    void X11Context::unregisterWindow(::Window xid)
    {
        for (usize i = 0; i < windowCount; ++i)
        {
            if (xids[i] != xid)
            {
                continue;
            }
            xids[i] = xids[windowCount - 1];
            windows[i] = windows[windowCount - 1];
            --windowCount;
            return;
        }
    }

    LinuxWindow* X11Context::findWindow(::Window xid) const
    {
        for (usize i = 0; i < windowCount; ++i)
        {
            if (xids[i] == xid)
            {
                return windows[i];
            }
        }
        return nullptr;
    }
}

#endif
