#include "Ark/System/Input.hpp"

#include "Ark/System/Platform/MacOS/KeyMapping.hpp"

#import <AppKit/AppKit.h>
#import <CoreGraphics/CoreGraphics.h>

namespace Ark::System::Input
{
    namespace
    {
        unsigned short keyToKeyCode(Key key)
        {
            Scancode scancode = Scancode::Unknown;

            switch (key)
            {
                case Key::A: scancode = Scancode::A; break;
                case Key::B: scancode = Scancode::B; break;
                case Key::C: scancode = Scancode::C; break;
                case Key::D: scancode = Scancode::D; break;
                case Key::E: scancode = Scancode::E; break;
                case Key::F: scancode = Scancode::F; break;
                case Key::G: scancode = Scancode::G; break;
                case Key::H: scancode = Scancode::H; break;
                case Key::I: scancode = Scancode::I; break;
                case Key::J: scancode = Scancode::J; break;
                case Key::K: scancode = Scancode::K; break;
                case Key::L: scancode = Scancode::L; break;
                case Key::M: scancode = Scancode::M; break;
                case Key::N: scancode = Scancode::N; break;
                case Key::O: scancode = Scancode::O; break;
                case Key::P: scancode = Scancode::P; break;
                case Key::Q: scancode = Scancode::Q; break;
                case Key::R: scancode = Scancode::R; break;
                case Key::S: scancode = Scancode::S; break;
                case Key::T: scancode = Scancode::T; break;
                case Key::U: scancode = Scancode::U; break;
                case Key::V: scancode = Scancode::V; break;
                case Key::W: scancode = Scancode::W; break;
                case Key::X: scancode = Scancode::X; break;
                case Key::Y: scancode = Scancode::Y; break;
                case Key::Z: scancode = Scancode::Z; break;
                case Key::Digit0: scancode = Scancode::Digit0; break;
                case Key::Digit1: scancode = Scancode::Digit1; break;
                case Key::Digit2: scancode = Scancode::Digit2; break;
                case Key::Digit3: scancode = Scancode::Digit3; break;
                case Key::Digit4: scancode = Scancode::Digit4; break;
                case Key::Digit5: scancode = Scancode::Digit5; break;
                case Key::Digit6: scancode = Scancode::Digit6; break;
                case Key::Digit7: scancode = Scancode::Digit7; break;
                case Key::Digit8: scancode = Scancode::Digit8; break;
                case Key::Digit9: scancode = Scancode::Digit9; break;
                case Key::Return: scancode = Scancode::Return; break;
                case Key::Escape: scancode = Scancode::Escape; break;
                case Key::Backspace: scancode = Scancode::Backspace; break;
                case Key::Tab: scancode = Scancode::Tab; break;
                case Key::Space: scancode = Scancode::Space; break;
                case Key::Minus: scancode = Scancode::Minus; break;
                case Key::Equals: scancode = Scancode::Equals; break;
                case Key::LeftBracket: scancode = Scancode::LeftBracket; break;
                case Key::RightBracket: scancode = Scancode::RightBracket; break;
                case Key::Backslash: scancode = Scancode::Backslash; break;
                case Key::Semicolon: scancode = Scancode::Semicolon; break;
                case Key::Apostrophe: scancode = Scancode::Apostrophe; break;
                case Key::GraveAccent: scancode = Scancode::GraveAccent; break;
                case Key::Comma: scancode = Scancode::Comma; break;
                case Key::Period: scancode = Scancode::Period; break;
                case Key::Slash: scancode = Scancode::Slash; break;
                case Key::CapsLock: scancode = Scancode::CapsLock; break;
                case Key::F1: scancode = Scancode::F1; break;
                case Key::F2: scancode = Scancode::F2; break;
                case Key::F3: scancode = Scancode::F3; break;
                case Key::F4: scancode = Scancode::F4; break;
                case Key::F5: scancode = Scancode::F5; break;
                case Key::F6: scancode = Scancode::F6; break;
                case Key::F7: scancode = Scancode::F7; break;
                case Key::F8: scancode = Scancode::F8; break;
                case Key::F9: scancode = Scancode::F9; break;
                case Key::F10: scancode = Scancode::F10; break;
                case Key::F11: scancode = Scancode::F11; break;
                case Key::F12: scancode = Scancode::F12; break;
                case Key::F13: scancode = Scancode::F13; break;
                case Key::F14: scancode = Scancode::F14; break;
                case Key::F15: scancode = Scancode::F15; break;
                case Key::F16: scancode = Scancode::F16; break;
                case Key::F17: scancode = Scancode::F17; break;
                case Key::F18: scancode = Scancode::F18; break;
                case Key::F19: scancode = Scancode::F19; break;
                case Key::F20: scancode = Scancode::F20; break;
                case Key::PrintScreen: scancode = Scancode::PrintScreen; break;
                case Key::ScrollLock: scancode = Scancode::ScrollLock; break;
                case Key::Pause: scancode = Scancode::Pause; break;
                case Key::Insert: scancode = Scancode::Insert; break;
                case Key::Home: scancode = Scancode::Home; break;
                case Key::PageUp: scancode = Scancode::PageUp; break;
                case Key::Delete: scancode = Scancode::Delete; break;
                case Key::End: scancode = Scancode::End; break;
                case Key::PageDown: scancode = Scancode::PageDown; break;
                case Key::Right: scancode = Scancode::Right; break;
                case Key::Left: scancode = Scancode::Left; break;
                case Key::Down: scancode = Scancode::Down; break;
                case Key::Up: scancode = Scancode::Up; break;
                case Key::NumLockClear: scancode = Scancode::NumLockClear; break;
                case Key::KeypadDivide: scancode = Scancode::KeypadDivide; break;
                case Key::KeypadMultiply: scancode = Scancode::KeypadMultiply; break;
                case Key::KeypadMinus: scancode = Scancode::KeypadMinus; break;
                case Key::KeypadPlus: scancode = Scancode::KeypadPlus; break;
                case Key::KeypadEnter: scancode = Scancode::KeypadEnter; break;
                case Key::Keypad0: scancode = Scancode::Keypad0; break;
                case Key::Keypad1: scancode = Scancode::Keypad1; break;
                case Key::Keypad2: scancode = Scancode::Keypad2; break;
                case Key::Keypad3: scancode = Scancode::Keypad3; break;
                case Key::Keypad4: scancode = Scancode::Keypad4; break;
                case Key::Keypad5: scancode = Scancode::Keypad5; break;
                case Key::Keypad6: scancode = Scancode::Keypad6; break;
                case Key::Keypad7: scancode = Scancode::Keypad7; break;
                case Key::Keypad8: scancode = Scancode::Keypad8; break;
                case Key::Keypad9: scancode = Scancode::Keypad9; break;
                case Key::KeypadPeriod: scancode = Scancode::KeypadPeriod; break;
                case Key::KeypadEquals: scancode = Scancode::KeypadEquals; break;
                case Key::LeftControl: scancode = Scancode::LeftControl; break;
                case Key::LeftShift: scancode = Scancode::LeftShift; break;
                case Key::LeftAlt: scancode = Scancode::LeftAlt; break;
                case Key::LeftCmd: scancode = Scancode::LeftCmd; break;
                case Key::RightControl: scancode = Scancode::RightControl; break;
                case Key::RightShift: scancode = Scancode::RightShift; break;
                case Key::RightAlt: scancode = Scancode::RightAlt; break;
                case Key::RightCmd: scancode = Scancode::RightCmd; break;
                case Key::Help: scancode = Scancode::Help; break;
                default: return 0xFFFF;
            }

            for (unsigned short keyCode = 0; keyCode < 128; ++keyCode)
            {
                if (Platform::MacOS::mapKeyCodeToScancode(keyCode) == scancode)
                {
                    return keyCode;
                }
            }

            return 0xFFFF;
        }
    }

    bool isKeyPressed(Key key)
    {
        @autoreleasepool
        {
            unsigned short keyCode = keyToKeyCode(key);
            if (keyCode == 0xFFFF)
            {
                return false;
            }

            return CGEventSourceKeyState(kCGEventSourceStateHIDSystemState, keyCode);
        }
    }

    bool isMouseButtonPressed(MouseButton button)
    {
        @autoreleasepool
        {
            NSUInteger pressedButtons = [NSEvent pressedMouseButtons];

            switch (button)
            {
                case MouseButton::Left:
                    return (pressedButtons & (1 << 0)) != 0;

                case MouseButton::Right:
                    return (pressedButtons & (1 << 1)) != 0;

                case MouseButton::Middle:
                    return (pressedButtons & (1 << 2)) != 0;

                case MouseButton::Button4:
                    return (pressedButtons & (1 << 3)) != 0;

                case MouseButton::Button5:
                    return (pressedButtons & (1 << 4)) != 0;

                default:
                    return false;
            }
        }
    }

    Point2Int getMousePosition()
    {
        @autoreleasepool
        {
            NSPoint mouseLocation = [NSEvent mouseLocation];
            NSScreen* mainScreen = [NSScreen mainScreen];

            if (mainScreen != nullptr)
            {
                CGFloat screenHeight = [mainScreen frame].size.height;
                return Point2Int(static_cast<int32>(mouseLocation.x), static_cast<int32>(screenHeight - mouseLocation.y));
            }

            return Point2Int(static_cast<int32>(mouseLocation.x), static_cast<int32>(mouseLocation.y));
        }
    }

    Point2Int getMousePositionInWindow(WindowHandle windowHandle)
    {
        if (windowHandle == nullptr)
        {
            return Point2Int(0, 0);
        }

        @autoreleasepool
        {
            NSWindow* nsWindow = (__bridge NSWindow*)windowHandle;
            NSPoint mouseLocation = [NSEvent mouseLocation];
            NSScreen* mainScreen = [NSScreen mainScreen];

            if (mainScreen == nullptr)
            {
                return Point2Int(0, 0);
            }

            CGFloat screenHeight = [mainScreen frame].size.height;
            NSRect windowFrame = [nsWindow frame];
            
            int32 windowX = static_cast<int32>(windowFrame.origin.x);
            int32 windowY = static_cast<int32>(screenHeight - windowFrame.origin.y - windowFrame.size.height);

            int32 relativeX = static_cast<int32>(mouseLocation.x) - windowX;
            int32 relativeY = static_cast<int32>(screenHeight - mouseLocation.y) - windowY;

            return Point2Int(relativeX, relativeY);
        }
    }

    void setMousePosition(int32 x, int32 y)
    {
        @autoreleasepool
        {
            NSScreen* mainScreen = [NSScreen mainScreen];
            if (mainScreen == nullptr)
            {
                return;
            }

            CGFloat screenHeight = [mainScreen frame].size.height;
            // Convert from top-left origin to bottom-left origin (macOS coordinate system)
            CGFloat cocoaY = screenHeight - static_cast<CGFloat>(y);
            
            CGPoint point = CGPointMake(static_cast<CGFloat>(x), cocoaY);
            CGWarpMouseCursorPosition(point);
        }
    }

    void setMousePositionInWindow(WindowHandle windowHandle, int32 x, int32 y)
    {
        if (windowHandle == nullptr)
        {
            return;
        }

        @autoreleasepool
        {
            NSWindow* nsWindow = (__bridge NSWindow*)windowHandle;
            NSRect windowFrame = [nsWindow frame];
            NSScreen* mainScreen = [NSScreen mainScreen];
            
            if (mainScreen == nullptr)
            {
                return;
            }

            CGFloat screenHeight = [mainScreen frame].size.height;
            // Convert window-relative coordinates to screen coordinates
            CGFloat screenX = windowFrame.origin.x + static_cast<CGFloat>(x);
            CGFloat screenY = screenHeight - (windowFrame.origin.y + windowFrame.size.height - static_cast<CGFloat>(y));
            
            CGPoint point = CGPointMake(screenX, screenY);
            CGWarpMouseCursorPosition(point);
        }
    }

    KeyModifiers getKeyModifiers()
    {
        @autoreleasepool
        {
            NSEventModifierFlags modifierFlags = [NSEvent modifierFlags];
            KeyModifiers modifiers = KeyModifiers::None;

            if ((modifierFlags & NSEventModifierFlagShift) != 0)
            {
                modifiers = modifiers | KeyModifiers::Shift;
            }

            if ((modifierFlags & NSEventModifierFlagControl) != 0)
            {
                modifiers = modifiers | KeyModifiers::Control;
            }

            if ((modifierFlags & NSEventModifierFlagOption) != 0)
            {
                modifiers = modifiers | KeyModifiers::Alt;
            }

            if ((modifierFlags & NSEventModifierFlagCommand) != 0)
            {
                modifiers = modifiers | KeyModifiers::Cmd;
            }

            if ((modifierFlags & NSEventModifierFlagCapsLock) != 0)
            {
                modifiers = modifiers | KeyModifiers::Caps;
            }

            if ((modifierFlags & NSEventModifierFlagNumericPad) != 0)
            {
                modifiers = modifiers | KeyModifiers::Num;
            }

            return modifiers;
        }
    }
}

