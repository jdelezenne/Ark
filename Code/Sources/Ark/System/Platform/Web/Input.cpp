#if defined(ARK_PLATFORM_WEBASSEMBLY)

#include "Ark/System/Input.hpp"

#include <emscripten/html5.h>

namespace Ark::System::Input
{
    namespace
    {
        static bool keyState[512] = {};
        static bool mouseState[8] = {};
        static int mouseX = 0;
        static int mouseY = 0;
        static bool callbacksRegistered = false;

        static EM_BOOL onMouse(int eventType, EmscriptenMouseEvent const* event, void*)
        {
            if (event == nullptr)
            {
                return EM_FALSE;
            }
            mouseX = event->screenX;
            mouseY = event->screenY;
            int button = event->button;
            if (button >= 0 && button < 8)
            {
                if (eventType == EMSCRIPTEN_EVENT_MOUSEDOWN)
                {
                    mouseState[button] = true;
                }
                else if (eventType == EMSCRIPTEN_EVENT_MOUSEUP)
                {
                    mouseState[button] = false;
                }
            }
            return EM_FALSE;
        }

        static EM_BOOL onKey(int eventType, EmscriptenKeyboardEvent const* event, void*)
        {
            if (event == nullptr)
            {
                return EM_FALSE;
            }
            // Track ASCII letter/digit keys for isKeyPressed queries.
            char ch = event->key[0];
            int index = -1;
            if (ch >= 'a' && ch <= 'z')
            {
                index = static_cast<int>(Key::A) + (ch - 'a');
            }
            else if (ch >= 'A' && ch <= 'Z')
            {
                index = static_cast<int>(Key::A) + (ch - 'A');
            }
            else if (ch >= '0' && ch <= '9')
            {
                index = static_cast<int>(Key::Digit0) + (ch - '0');
            }
            if (index >= 0 && index < 512)
            {
                keyState[index] = (eventType == EMSCRIPTEN_EVENT_KEYDOWN);
            }
            return EM_FALSE;
        }

        static void ensureCallbacks()
        {
            if (callbacksRegistered)
            {
                return;
            }
            callbacksRegistered = true;
            emscripten_set_mousedown_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr, EM_TRUE, onMouse);
            emscripten_set_mouseup_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr, EM_TRUE, onMouse);
            emscripten_set_mousemove_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr, EM_TRUE, onMouse);
            emscripten_set_keydown_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr, EM_TRUE, onKey);
            emscripten_set_keyup_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr, EM_TRUE, onKey);
        }
    }

    bool isKeyPressed(Key key)
    {
        ensureCallbacks();
        int index = static_cast<int>(key);
        if (index < 0 || index >= 512)
        {
            return false;
        }
        return keyState[index];
    }

    bool isMouseButtonPressed(MouseButton button)
    {
        ensureCallbacks();
        switch (button)
        {
            case MouseButton::Left:
                return mouseState[0];
            case MouseButton::Middle:
                return mouseState[1];
            case MouseButton::Right:
                return mouseState[2];
            default:
                return false;
        }
    }

    Point2Int getMousePosition()
    {
        ensureCallbacks();
        return Point2Int{mouseX, mouseY};
    }

    Point2Int getMousePositionInWindow(WindowHandle /*windowHandle*/)
    {
        ensureCallbacks();
        EmscriptenMouseEvent event{};
        if (emscripten_get_mouse_status(&event) == EMSCRIPTEN_RESULT_SUCCESS)
        {
            return Point2Int{static_cast<int32>(event.targetX), static_cast<int32>(event.targetY)};
        }
        return Point2Int{0, 0};
    }

    void setMousePosition(int32 /*x*/, int32 /*y*/)
    {
        // Not supported in browsers without pointer lock APIs specific to the page.
    }

    void setMousePositionInWindow(WindowHandle /*windowHandle*/, int32 /*x*/, int32 /*y*/)
    {
    }

    KeyModifiers getKeyModifiers()
    {
        ensureCallbacks();
        return KeyModifiers::None;
    }
}

#endif
