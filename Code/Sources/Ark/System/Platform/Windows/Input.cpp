#include "Ark/System/Input.hpp"

#include <Windows.h>

namespace Ark::System::Input
{
    namespace
    {
        int keyToVirtualKey(Key key)
        {
            switch (key)
            {
                case Key::A:
                    return 'A';
                case Key::B:
                    return 'B';
                case Key::C:
                    return 'C';
                case Key::D:
                    return 'D';
                case Key::E:
                    return 'E';
                case Key::F:
                    return 'F';
                case Key::G:
                    return 'G';
                case Key::H:
                    return 'H';
                case Key::I:
                    return 'I';
                case Key::J:
                    return 'J';
                case Key::K:
                    return 'K';
                case Key::L:
                    return 'L';
                case Key::M:
                    return 'M';
                case Key::N:
                    return 'N';
                case Key::O:
                    return 'O';
                case Key::P:
                    return 'P';
                case Key::Q:
                    return 'Q';
                case Key::R:
                    return 'R';
                case Key::S:
                    return 'S';
                case Key::T:
                    return 'T';
                case Key::U:
                    return 'U';
                case Key::V:
                    return 'V';
                case Key::W:
                    return 'W';
                case Key::X:
                    return 'X';
                case Key::Y:
                    return 'Y';
                case Key::Z:
                    return 'Z';
                case Key::Digit0:
                    return '0';
                case Key::Digit1:
                    return '1';
                case Key::Digit2:
                    return '2';
                case Key::Digit3:
                    return '3';
                case Key::Digit4:
                    return '4';
                case Key::Digit5:
                    return '5';
                case Key::Digit6:
                    return '6';
                case Key::Digit7:
                    return '7';
                case Key::Digit8:
                    return '8';
                case Key::Digit9:
                    return '9';
                case Key::Return:
                    return VK_RETURN;
                case Key::Escape:
                    return VK_ESCAPE;
                case Key::Backspace:
                    return VK_BACK;
                case Key::Tab:
                    return VK_TAB;
                case Key::Space:
                    return VK_SPACE;
                case Key::Minus:
                    return VK_OEM_MINUS;
                case Key::Equals:
                    return VK_OEM_PLUS;
                case Key::LeftBracket:
                    return VK_OEM_4;
                case Key::RightBracket:
                    return VK_OEM_6;
                case Key::Backslash:
                    return VK_OEM_5;
                case Key::Semicolon:
                    return VK_OEM_1;
                case Key::Apostrophe:
                    return VK_OEM_7;
                case Key::GraveAccent:
                    return VK_OEM_3;
                case Key::Comma:
                    return VK_OEM_COMMA;
                case Key::Period:
                    return VK_OEM_PERIOD;
                case Key::Slash:
                    return VK_OEM_2;
                case Key::CapsLock:
                    return VK_CAPITAL;
                case Key::F1:
                    return VK_F1;
                case Key::F2:
                    return VK_F2;
                case Key::F3:
                    return VK_F3;
                case Key::F4:
                    return VK_F4;
                case Key::F5:
                    return VK_F5;
                case Key::F6:
                    return VK_F6;
                case Key::F7:
                    return VK_F7;
                case Key::F8:
                    return VK_F8;
                case Key::F9:
                    return VK_F9;
                case Key::F10:
                    return VK_F10;
                case Key::F11:
                    return VK_F11;
                case Key::F12:
                    return VK_F12;
                case Key::F13:
                    return VK_F13;
                case Key::F14:
                    return VK_F14;
                case Key::F15:
                    return VK_F15;
                case Key::F16:
                    return VK_F16;
                case Key::F17:
                    return VK_F17;
                case Key::F18:
                    return VK_F18;
                case Key::F19:
                    return VK_F19;
                case Key::F20:
                    return VK_F20;
                case Key::F21:
                    return VK_F21;
                case Key::F22:
                    return VK_F22;
                case Key::F23:
                    return VK_F23;
                case Key::F24:
                    return VK_F24;
                case Key::PrintScreen:
                    return VK_SNAPSHOT;
                case Key::ScrollLock:
                    return VK_SCROLL;
                case Key::Pause:
                    return VK_PAUSE;
                case Key::Insert:
                    return VK_INSERT;
                case Key::Home:
                    return VK_HOME;
                case Key::PageUp:
                    return VK_PRIOR;
                case Key::Delete:
                    return VK_DELETE;
                case Key::End:
                    return VK_END;
                case Key::PageDown:
                    return VK_NEXT;
                case Key::Right:
                    return VK_RIGHT;
                case Key::Left:
                    return VK_LEFT;
                case Key::Down:
                    return VK_DOWN;
                case Key::Up:
                    return VK_UP;
                case Key::NumLockClear:
                    return VK_NUMLOCK;
                case Key::KeypadDivide:
                    return VK_DIVIDE;
                case Key::KeypadMultiply:
                    return VK_MULTIPLY;
                case Key::KeypadMinus:
                    return VK_SUBTRACT;
                case Key::KeypadPlus:
                    return VK_ADD;
                case Key::KeypadEnter:
                    return VK_RETURN;
                case Key::Keypad0:
                    return VK_NUMPAD0;
                case Key::Keypad1:
                    return VK_NUMPAD1;
                case Key::Keypad2:
                    return VK_NUMPAD2;
                case Key::Keypad3:
                    return VK_NUMPAD3;
                case Key::Keypad4:
                    return VK_NUMPAD4;
                case Key::Keypad5:
                    return VK_NUMPAD5;
                case Key::Keypad6:
                    return VK_NUMPAD6;
                case Key::Keypad7:
                    return VK_NUMPAD7;
                case Key::Keypad8:
                    return VK_NUMPAD8;
                case Key::Keypad9:
                    return VK_NUMPAD9;
                case Key::KeypadPeriod:
                    return VK_DECIMAL;
                case Key::LeftControl:
                    return VK_LCONTROL;
                case Key::LeftShift:
                    return VK_LSHIFT;
                case Key::LeftAlt:
                    return VK_LMENU;
                case Key::LeftCmd:
                    return VK_LWIN;
                case Key::RightControl:
                    return VK_RCONTROL;
                case Key::RightShift:
                    return VK_RSHIFT;
                case Key::RightAlt:
                    return VK_RMENU;
                case Key::RightCmd:
                    return VK_RWIN;
                default:
                    return 0;
            }
        }
    }

    bool isKeyPressed(Key key)
    {
        int virtualKey = keyToVirtualKey(key);
        if (virtualKey == 0)
        {
            return false;
        }

        return (GetAsyncKeyState(virtualKey) & 0x8000) != 0;
    }

    bool isMouseButtonPressed(MouseButton button)
    {
        int virtualKey = 0;

        switch (button)
        {
            case MouseButton::Left:
                virtualKey = VK_LBUTTON;
                break;

            case MouseButton::Right:
                virtualKey = VK_RBUTTON;
                break;

            case MouseButton::Middle:
                virtualKey = VK_MBUTTON;
                break;

            case MouseButton::Button4:
                virtualKey = VK_XBUTTON1;
                break;

            case MouseButton::Button5:
                virtualKey = VK_XBUTTON2;
                break;

            default:
                return false;
        }

        return (GetAsyncKeyState(virtualKey) & 0x8000) != 0;
    }

    Point2Int getMousePosition()
    {
        POINT point;
        if (GetCursorPos(&point))
        {
            return Point2Int(static_cast<int32>(point.x), static_cast<int32>(point.y));
        }

        return Point2Int(0, 0);
    }

    Point2Int getMousePositionInWindow(WindowHandle windowHandle)
    {
        if (windowHandle == nullptr)
        {
            return Point2Int(0, 0);
        }

        HWND hwnd = static_cast<HWND>(windowHandle);
        POINT point;
        if (!GetCursorPos(&point))
        {
            return Point2Int(0, 0);
        }

        if (!ScreenToClient(hwnd, &point))
        {
            return Point2Int(0, 0);
        }

        return Point2Int(static_cast<int32>(point.x), static_cast<int32>(point.y));
    }

    void setMousePosition(int32 x, int32 y)
    {
        SetCursorPos(x, y);
    }

    void setMousePositionInWindow(WindowHandle windowHandle, int32 x, int32 y)
    {
        if (windowHandle == nullptr)
        {
            return;
        }

        HWND hwnd = static_cast<HWND>(windowHandle);
        POINT point;
        point.x = x;
        point.y = y;

        if (ClientToScreen(hwnd, &point))
        {
            SetCursorPos(point.x, point.y);
        }
    }

    KeyModifiers getKeyModifiers()
    {
        KeyModifiers modifiers = KeyModifiers::None;

        if ((GetAsyncKeyState(VK_LSHIFT) & 0x8000) != 0)
        {
            modifiers = modifiers | KeyModifiers::LeftShift;
        }

        if ((GetAsyncKeyState(VK_RSHIFT) & 0x8000) != 0)
        {
            modifiers = modifiers | KeyModifiers::RightShift;
        }

        if ((GetAsyncKeyState(VK_LCONTROL) & 0x8000) != 0)
        {
            modifiers = modifiers | KeyModifiers::LeftControl;
        }

        if ((GetAsyncKeyState(VK_RCONTROL) & 0x8000) != 0)
        {
            modifiers = modifiers | KeyModifiers::RightControl;
        }

        if ((GetAsyncKeyState(VK_LMENU) & 0x8000) != 0)
        {
            modifiers = modifiers | KeyModifiers::LeftAlt;
        }

        if ((GetAsyncKeyState(VK_RMENU) & 0x8000) != 0)
        {
            modifiers = modifiers | KeyModifiers::RightAlt;
        }

        if ((GetAsyncKeyState(VK_LWIN) & 0x8000) != 0)
        {
            modifiers = modifiers | KeyModifiers::LeftCmd;
        }

        if ((GetAsyncKeyState(VK_RWIN) & 0x8000) != 0)
        {
            modifiers = modifiers | KeyModifiers::RightCmd;
        }

        if ((GetAsyncKeyState(VK_CAPITAL) & 0x0001) != 0)
        {
            modifiers = modifiers | KeyModifiers::Caps;
        }

        if ((GetAsyncKeyState(VK_NUMLOCK) & 0x0001) != 0)
        {
            modifiers = modifiers | KeyModifiers::Num;
        }

        if ((GetAsyncKeyState(VK_SCROLL) & 0x0001) != 0)
        {
            modifiers = modifiers | KeyModifiers::Scroll;
        }

        return modifiers;
    }
}
