#pragma once

#if defined(ARK_PLATFORM_LINUX) && !defined(ARK_PLATFORM_WEBASSEMBLY)

#include "Ark/Core/Types.hpp"

#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

namespace Ark::Display::Platform::Linux
{
    class LinuxWindow;

    /// Shared X11 connection used by Display, Application EventSource, and System Input/Cursor.
    struct X11Context final
    {
        ::Display* display = nullptr;
        int screen = 0;
        ::Window root = 0;
        Atom wmDelete = 0;
        Atom wmProtocols = 0;
        Atom netWmState = 0;
        Atom netWmStateFullscreen = 0;
        Atom netWmName = 0;
        Atom utf8String = 0;
        Atom clipboard = 0;
        Atom primary = 0;
        Atom targets = 0;
        Atom text = 0;

        static X11Context& get();
        bool isValid() const
        {
            return display != nullptr;
        }

        void registerWindow(::Window xid, LinuxWindow* window);
        void unregisterWindow(::Window xid);
        LinuxWindow* findWindow(::Window xid) const;

    private:
        static constexpr usize MaxWindows = 64;
        ::Window xids[MaxWindows]{};
        LinuxWindow* windows[MaxWindows]{};
        usize windowCount = 0;
    };
}

#endif
