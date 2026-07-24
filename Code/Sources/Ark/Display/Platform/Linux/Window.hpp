#pragma once

#if defined(ARK_PLATFORM_LINUX) && !defined(ARK_PLATFORM_WEBASSEMBLY)

#include "Ark/Display/Window.hpp"

#include <X11/Xlib.h>

namespace Ark::Display::Platform::Linux
{
    class LinuxWindow;

    Window* createWindow(WindowCreateInfo const& info);

    /// Dispatches a pending X event to the owning window (used by EventSource).
    void dispatchXEvent(XEvent const& event);
}

#endif
