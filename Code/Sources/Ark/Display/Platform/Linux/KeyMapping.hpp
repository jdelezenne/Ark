#pragma once

#if defined(ARK_PLATFORM_LINUX) && !defined(ARK_PLATFORM_WEBASSEMBLY)

#include "Ark/System/Key.hpp"
#include "Ark/System/Mouse.hpp"
#include "Ark/System/Scancode.hpp"

#include <X11/Xlib.h>
#include <X11/keysym.h>

namespace Ark::Display::Platform::Linux
{
    inline Key keySymToKey(KeySym keySym)
    {
        switch (keySym)
        {
            case XK_Return:
            case XK_KP_Enter:
                return Key::Return;
            case XK_Escape:
                return Key::Escape;
            case XK_BackSpace:
                return Key::Backspace;
            case XK_Tab:
                return Key::Tab;
            case XK_space:
                return Key::Space;
            case XK_0:
                return Key::Digit0;
            case XK_1:
                return Key::Digit1;
            case XK_2:
                return Key::Digit2;
            case XK_3:
                return Key::Digit3;
            case XK_4:
                return Key::Digit4;
            case XK_5:
                return Key::Digit5;
            case XK_6:
                return Key::Digit6;
            case XK_7:
                return Key::Digit7;
            case XK_8:
                return Key::Digit8;
            case XK_9:
                return Key::Digit9;
            case XK_a:
            case XK_A:
                return Key::A;
            case XK_b:
            case XK_B:
                return Key::B;
            case XK_c:
            case XK_C:
                return Key::C;
            case XK_d:
            case XK_D:
                return Key::D;
            case XK_e:
            case XK_E:
                return Key::E;
            case XK_f:
            case XK_F:
                return Key::F;
            case XK_g:
            case XK_G:
                return Key::G;
            case XK_h:
            case XK_H:
                return Key::H;
            case XK_i:
            case XK_I:
                return Key::I;
            case XK_j:
            case XK_J:
                return Key::J;
            case XK_k:
            case XK_K:
                return Key::K;
            case XK_l:
            case XK_L:
                return Key::L;
            case XK_m:
            case XK_M:
                return Key::M;
            case XK_n:
            case XK_N:
                return Key::N;
            case XK_o:
            case XK_O:
                return Key::O;
            case XK_p:
            case XK_P:
                return Key::P;
            case XK_q:
            case XK_Q:
                return Key::Q;
            case XK_r:
            case XK_R:
                return Key::R;
            case XK_s:
            case XK_S:
                return Key::S;
            case XK_t:
            case XK_T:
                return Key::T;
            case XK_u:
            case XK_U:
                return Key::U;
            case XK_v:
            case XK_V:
                return Key::V;
            case XK_w:
            case XK_W:
                return Key::W;
            case XK_x:
            case XK_X:
                return Key::X;
            case XK_y:
            case XK_Y:
                return Key::Y;
            case XK_z:
            case XK_Z:
                return Key::Z;
            case XK_Left:
                return Key::Left;
            case XK_Right:
                return Key::Right;
            case XK_Up:
                return Key::Up;
            case XK_Down:
                return Key::Down;
            case XK_Shift_L:
            case XK_Shift_R:
                return Key::LeftShift;
            case XK_Control_L:
            case XK_Control_R:
                return Key::LeftControl;
            case XK_Alt_L:
            case XK_Alt_R:
                return Key::LeftAlt;
            case XK_F1:
                return Key::F1;
            case XK_F2:
                return Key::F2;
            case XK_F3:
                return Key::F3;
            case XK_F4:
                return Key::F4;
            case XK_F5:
                return Key::F5;
            case XK_F6:
                return Key::F6;
            case XK_F7:
                return Key::F7;
            case XK_F8:
                return Key::F8;
            case XK_F9:
                return Key::F9;
            case XK_F10:
                return Key::F10;
            case XK_F11:
                return Key::F11;
            case XK_F12:
                return Key::F12;
            case XK_Delete:
                return Key::Delete;
            case XK_Home:
                return Key::Home;
            case XK_End:
                return Key::End;
            case XK_Page_Up:
                return Key::PageUp;
            case XK_Page_Down:
                return Key::PageDown;
            default:
                return Key::Unknown;
        }
    }

    inline KeyModifiers stateToModifiers(unsigned int state)
    {
        KeyModifiers modifiers = KeyModifiers::None;
        if ((state & ShiftMask) != 0)
        {
            modifiers |= KeyModifiers::Shift;
        }
        if ((state & ControlMask) != 0)
        {
            modifiers |= KeyModifiers::Control;
        }
        if ((state & Mod1Mask) != 0)
        {
            modifiers |= KeyModifiers::Alt;
        }
        if ((state & Mod4Mask) != 0)
        {
            modifiers |= KeyModifiers::Cmd;
        }
        return modifiers;
    }

    inline MouseButton xButtonToMouse(unsigned int button)
    {
        switch (button)
        {
            case Button1:
                return MouseButton::Left;
            case Button2:
                return MouseButton::Middle;
            case Button3:
                return MouseButton::Right;
            case Button8:
                return MouseButton::Button4;
            case Button9:
                return MouseButton::Button5;
            default:
                return MouseButton::Left;
        }
    }

    inline MouseButtons stateToMouseButtons(unsigned int state)
    {
        MouseButtons buttons = MouseButtons::None;
        if ((state & Button1Mask) != 0)
        {
            buttons |= MouseButtons::Left;
        }
        if ((state & Button2Mask) != 0)
        {
            buttons |= MouseButtons::Middle;
        }
        if ((state & Button3Mask) != 0)
        {
            buttons |= MouseButtons::Right;
        }
        return buttons;
    }
}

#endif
