#if defined(ARK_PLATFORM_WEBASSEMBLY)

#include "Ark/System/Cursor.hpp"

#include <emscripten/emscripten.h>

namespace Ark::System::Cursor
{
    namespace
    {
        static bool cursorVisible = true;
        static SystemCursor currentCursor = SystemCursor::Arrow;

        static char const* getCursorStyleName(SystemCursor cursor)
        {
            switch (cursor)
            {
                case SystemCursor::IBeam:
                    return "text";
                case SystemCursor::Wait:
                    return "wait";
                case SystemCursor::Crosshair:
                    return "crosshair";
                case SystemCursor::WaitArrow:
                    return "progress";
                case SystemCursor::SizeNWSE:
                    return "nwse-resize";
                case SystemCursor::SizeNESW:
                    return "nesw-resize";
                case SystemCursor::SizeWE:
                    return "ew-resize";
                case SystemCursor::SizeNS:
                    return "ns-resize";
                case SystemCursor::SizeAll:
                    return "move";
                case SystemCursor::No:
                    return "not-allowed";
                case SystemCursor::Hand:
                    return "pointer";
                case SystemCursor::Arrow:
                default:
                    return "default";
            }
        }

        static void applyCursor()
        {
            char const* style = cursorVisible ? getCursorStyleName(currentCursor) : "none";
            EM_ASM({
                var style = UTF8ToString($0);
                document.body.style.cursor = style;
                if (Module.canvas)
                {
                    Module.canvas.style.cursor = style;
                }
            }, style);
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
