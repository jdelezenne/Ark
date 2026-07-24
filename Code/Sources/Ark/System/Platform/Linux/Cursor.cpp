#if defined(ARK_PLATFORM_LINUX) && !defined(ARK_PLATFORM_WEBASSEMBLY)

#include "Ark/System/Cursor.hpp"

#include <X11/Xlib.h>
#include <X11/cursorfont.h>

namespace Ark::System::Cursor
{
    namespace
    {
        static bool cursorVisible = true;
        static SystemCursor currentCursor = SystemCursor::Arrow;
        static ::Display* display = nullptr;
        static ::Cursor activeCursor = None;

        static ::Display* getDisplay()
        {
            if (display == nullptr)
            {
                display = XOpenDisplay(nullptr);
            }
            return display;
        }

        static unsigned int toFontCursor(SystemCursor cursor)
        {
            switch (cursor)
            {
                case SystemCursor::IBeam:
                    return XC_xterm;
                case SystemCursor::Wait:
                    return XC_watch;
                case SystemCursor::Crosshair:
                    return XC_crosshair;
                case SystemCursor::WaitArrow:
                    return XC_watch;
                case SystemCursor::SizeNWSE:
                    return XC_bottom_right_corner;
                case SystemCursor::SizeNESW:
                    return XC_bottom_left_corner;
                case SystemCursor::SizeWE:
                    return XC_sb_h_double_arrow;
                case SystemCursor::SizeNS:
                    return XC_sb_v_double_arrow;
                case SystemCursor::SizeAll:
                    return XC_fleur;
                case SystemCursor::No:
                    return XC_X_cursor;
                case SystemCursor::Hand:
                    return XC_hand2;
                case SystemCursor::Arrow:
                default:
                    return XC_left_ptr;
            }
        }

        static void applyCursor()
        {
            ::Display* dpy = getDisplay();
            if (dpy == nullptr)
            {
                return;
            }

            ::Window root = DefaultRootWindow(dpy);
            if (!cursorVisible)
            {
                static char data[1] = {0};
                XColor color{};
                Pixmap blank = XCreateBitmapFromData(dpy, root, data, 1, 1);
                ::Cursor invisible = XCreatePixmapCursor(dpy, blank, blank, &color, &color, 0, 0);
                XDefineCursor(dpy, root, invisible);
                if (activeCursor != None)
                {
                    XFreeCursor(dpy, activeCursor);
                }
                activeCursor = invisible;
                XFreePixmap(dpy, blank);
                XFlush(dpy);
                return;
            }

            ::Cursor next = XCreateFontCursor(dpy, toFontCursor(currentCursor));
            XDefineCursor(dpy, root, next);
            if (activeCursor != None)
            {
                XFreeCursor(dpy, activeCursor);
            }
            activeCursor = next;
            XFlush(dpy);
        }
    }

    void setVisible(bool visible)
    {
        cursorVisible = visible;
        applyCursor();
    }

    bool isVisible()
    {
        return cursorVisible;
    }

    void setCursor(SystemCursor cursor)
    {
        currentCursor = cursor;
        applyCursor();
    }

    void resetCursor()
    {
        setCursor(SystemCursor::Arrow);
    }
}

#endif
