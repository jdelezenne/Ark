#if defined(ARK_PLATFORM_LINUX) && !defined(ARK_PLATFORM_WEBASSEMBLY)

#include "Ark/System/Input.hpp"

#include <X11/Xlib.h>
#include <X11/keysym.h>

namespace Ark::System::Input
{
    namespace
    {
        Display* getX11Display()
        {
            static Display* display = XOpenDisplay(nullptr);
            return display;
        }

        KeySym keyToKeySym(Key key)
        {
            switch (key)
            {
                case Key::A:
                    return XK_a;
                case Key::B:
                    return XK_b;
                case Key::C:
                    return XK_c;
                case Key::D:
                    return XK_d;
                case Key::E:
                    return XK_e;
                case Key::F:
                    return XK_f;
                case Key::G:
                    return XK_g;
                case Key::H:
                    return XK_h;
                case Key::I:
                    return XK_i;
                case Key::J:
                    return XK_j;
                case Key::K:
                    return XK_k;
                case Key::L:
                    return XK_l;
                case Key::M:
                    return XK_m;
                case Key::N:
                    return XK_n;
                case Key::O:
                    return XK_o;
                case Key::P:
                    return XK_p;
                case Key::Q:
                    return XK_q;
                case Key::R:
                    return XK_r;
                case Key::S:
                    return XK_s;
                case Key::T:
                    return XK_t;
                case Key::U:
                    return XK_u;
                case Key::V:
                    return XK_v;
                case Key::W:
                    return XK_w;
                case Key::X:
                    return XK_x;
                case Key::Y:
                    return XK_y;
                case Key::Z:
                    return XK_z;
                case Key::Digit0:
                    return XK_0;
                case Key::Digit1:
                    return XK_1;
                case Key::Digit2:
                    return XK_2;
                case Key::Digit3:
                    return XK_3;
                case Key::Digit4:
                    return XK_4;
                case Key::Digit5:
                    return XK_5;
                case Key::Digit6:
                    return XK_6;
                case Key::Digit7:
                    return XK_7;
                case Key::Digit8:
                    return XK_8;
                case Key::Digit9:
                    return XK_9;
                case Key::Return:
                    return XK_Return;
                case Key::Escape:
                    return XK_Escape;
                case Key::Backspace:
                    return XK_BackSpace;
                case Key::Tab:
                    return XK_Tab;
                case Key::Space:
                    return XK_space;
                case Key::Minus:
                    return XK_minus;
                case Key::Equals:
                    return XK_equal;
                case Key::LeftBracket:
                    return XK_bracketleft;
                case Key::RightBracket:
                    return XK_bracketright;
                case Key::Backslash:
                    return XK_backslash;
                case Key::Semicolon:
                    return XK_semicolon;
                case Key::Apostrophe:
                    return XK_apostrophe;
                case Key::GraveAccent:
                    return XK_grave;
                case Key::Comma:
                    return XK_comma;
                case Key::Period:
                    return XK_period;
                case Key::Slash:
                    return XK_slash;
                case Key::CapsLock:
                    return XK_Caps_Lock;
                case Key::F1:
                    return XK_F1;
                case Key::F2:
                    return XK_F2;
                case Key::F3:
                    return XK_F3;
                case Key::F4:
                    return XK_F4;
                case Key::F5:
                    return XK_F5;
                case Key::F6:
                    return XK_F6;
                case Key::F7:
                    return XK_F7;
                case Key::F8:
                    return XK_F8;
                case Key::F9:
                    return XK_F9;
                case Key::F10:
                    return XK_F10;
                case Key::F11:
                    return XK_F11;
                case Key::F12:
                    return XK_F12;
                case Key::F13:
                    return XK_F13;
                case Key::F14:
                    return XK_F14;
                case Key::F15:
                    return XK_F15;
                case Key::F16:
                    return XK_F16;
                case Key::F17:
                    return XK_F17;
                case Key::F18:
                    return XK_F18;
                case Key::F19:
                    return XK_F19;
                case Key::F20:
                    return XK_F20;
                case Key::F21:
                    return XK_F21;
                case Key::F22:
                    return XK_F22;
                case Key::F23:
                    return XK_F23;
                case Key::F24:
                    return XK_F24;
                case Key::PrintScreen:
                    return XK_Print;
                case Key::ScrollLock:
                    return XK_Scroll_Lock;
                case Key::Pause:
                    return XK_Pause;
                case Key::Insert:
                    return XK_Insert;
                case Key::Home:
                    return XK_Home;
                case Key::PageUp:
                    return XK_Page_Up;
                case Key::Delete:
                    return XK_Delete;
                case Key::End:
                    return XK_End;
                case Key::PageDown:
                    return XK_Page_Down;
                case Key::Right:
                    return XK_Right;
                case Key::Left:
                    return XK_Left;
                case Key::Down:
                    return XK_Down;
                case Key::Up:
                    return XK_Up;
                case Key::NumLockClear:
                    return XK_Num_Lock;
                case Key::KeypadDivide:
                    return XK_KP_Divide;
                case Key::KeypadMultiply:
                    return XK_KP_Multiply;
                case Key::KeypadMinus:
                    return XK_KP_Subtract;
                case Key::KeypadPlus:
                    return XK_KP_Add;
                case Key::KeypadEnter:
                    return XK_KP_Enter;
                case Key::Keypad0:
                    return XK_KP_0;
                case Key::Keypad1:
                    return XK_KP_1;
                case Key::Keypad2:
                    return XK_KP_2;
                case Key::Keypad3:
                    return XK_KP_3;
                case Key::Keypad4:
                    return XK_KP_4;
                case Key::Keypad5:
                    return XK_KP_5;
                case Key::Keypad6:
                    return XK_KP_6;
                case Key::Keypad7:
                    return XK_KP_7;
                case Key::Keypad8:
                    return XK_KP_8;
                case Key::Keypad9:
                    return XK_KP_9;
                case Key::KeypadPeriod:
                    return XK_KP_Decimal;
                case Key::LeftControl:
                    return XK_Control_L;
                case Key::LeftShift:
                    return XK_Shift_L;
                case Key::LeftAlt:
                    return XK_Alt_L;
                case Key::LeftCmd:
                    return XK_Super_L;
                case Key::RightControl:
                    return XK_Control_R;
                case Key::RightShift:
                    return XK_Shift_R;
                case Key::RightAlt:
                    return XK_Alt_R;
                case Key::RightCmd:
                    return XK_Super_R;
                default:
                    return NoSymbol;
            }
        }
    }

    bool isKeyPressed(Key key)
    {
        Display* display = getX11Display();
        if (display == nullptr)
        {
            return false;
        }

        KeySym keySym = keyToKeySym(key);
        if (keySym == NoSymbol)
        {
            return false;
        }

        KeyCode keyCode = XKeysymToKeycode(display, keySym);
        if (keyCode == 0)
        {
            return false;
        }

        char keyStates[32];
        XQueryKeymap(display, keyStates);

        return (keyStates[keyCode / 8] & (1 << (keyCode % 8))) != 0;
    }

    bool isMouseButtonPressed(MouseButton button)
    {
        Display* display = getX11Display();
        if (display == nullptr)
        {
            return false;
        }

        Window rootWindow = DefaultRootWindow(display);
        Window childWindow;
        int rootX = 0;
        int rootY = 0;
        int winX = 0;
        int winY = 0;
        unsigned int mask = 0;

        if (!XQueryPointer(display, rootWindow, &rootWindow, &childWindow, &rootX, &rootY, &winX, &winY, &mask))
        {
            return false;
        }

        switch (button)
        {
            case MouseButton::Left:
                return (mask & Button1Mask) != 0;

            case MouseButton::Right:
                return (mask & Button3Mask) != 0;

            case MouseButton::Middle:
                return (mask & Button2Mask) != 0;

            case MouseButton::Button4:
                return (mask & Button4Mask) != 0;

            case MouseButton::Button5:
                return (mask & Button5Mask) != 0;

            default:
                return false;
        }
    }

    Point2Int getMousePosition()
    {
        Display* display = getX11Display();
        if (display == nullptr)
        {
            return Point2Int(0, 0);
        }

        Window rootWindow = DefaultRootWindow(display);
        Window childWindow;
        int rootX = 0;
        int rootY = 0;
        int winX = 0;
        int winY = 0;
        unsigned int mask = 0;

        if (XQueryPointer(display, rootWindow, &rootWindow, &childWindow, &rootX, &rootY, &winX, &winY, &mask))
        {
            return Point2Int(static_cast<int32>(rootX), static_cast<int32>(rootY));
        }

        return Point2Int(0, 0);
    }

    Point2Int getMousePositionInWindow(WindowHandle windowHandle)
    {
        if (windowHandle == nullptr)
        {
            return Point2Int(0, 0);
        }

        Display* display = getX11Display();
        if (display == nullptr)
        {
            return Point2Int(0, 0);
        }

        Window x11Window = reinterpret_cast<Window>(windowHandle);
        Window rootWindow;
        Window childWindow;
        int rootX = 0;
        int rootY = 0;
        int winX = 0;
        int winY = 0;
        unsigned int mask = 0;

        if (!XQueryPointer(display, x11Window, &rootWindow, &childWindow, &rootX, &rootY, &winX, &winY, &mask))
        {
            return Point2Int(0, 0);
        }

        return Point2Int(static_cast<int32>(winX), static_cast<int32>(winY));
    }

    void setMousePosition(int32 x, int32 y)
    {
        Display* display = getX11Display();
        if (display == nullptr)
        {
            return;
        }

        Window rootWindow = DefaultRootWindow(display);
        XWarpPointer(display, None, rootWindow, 0, 0, 0, 0, x, y);
        XFlush(display);
    }

    void setMousePositionInWindow(WindowHandle windowHandle, int32 x, int32 y)
    {
        if (windowHandle == nullptr)
        {
            return;
        }

        Display* display = getX11Display();
        if (display == nullptr)
        {
            return;
        }

        Window x11Window = reinterpret_cast<Window>(windowHandle);
        XWarpPointer(display, None, x11Window, 0, 0, 0, 0, x, y);
        XFlush(display);
    }

    KeyModifiers getKeyModifiers()
    {
        Display* display = getX11Display();
        if (display == nullptr)
        {
            return KeyModifiers::None;
        }

        Window rootWindow = DefaultRootWindow(display);
        Window childWindow;
        int rootX = 0;
        int rootY = 0;
        int winX = 0;
        int winY = 0;
        unsigned int mask = 0;

        if (!XQueryPointer(display, rootWindow, &rootWindow, &childWindow, &rootX, &rootY, &winX, &winY, &mask))
        {
            return KeyModifiers::None;
        }

        KeyModifiers modifiers = KeyModifiers::None;

        if ((mask & ShiftMask) != 0)
        {
            modifiers = modifiers | KeyModifiers::Shift;
        }

        if ((mask & ControlMask) != 0)
        {
            modifiers = modifiers | KeyModifiers::Control;
        }

        if ((mask & Mod1Mask) != 0)
        {
            modifiers = modifiers | KeyModifiers::Alt;
        }

        if ((mask & Mod4Mask) != 0)
        {
            modifiers = modifiers | KeyModifiers::Cmd;
        }

        if ((mask & LockMask) != 0)
        {
            modifiers = modifiers | KeyModifiers::Caps;
        }

        if ((mask & Mod2Mask) != 0)
        {
            modifiers = modifiers | KeyModifiers::Num;
        }

        return modifiers;
    }
}

#endif
