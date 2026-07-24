#include "Ark/System/Cursor.hpp"

#include "Ark/Core/Platform/Windows/Platform.hpp"

namespace Ark::System::Cursor
{
    static HCURSOR getSystemCursorHandle(SystemCursor cursor)
    {
        LPCWSTR cursorId = nullptr;

        switch (cursor)
        {
            case SystemCursor::Arrow:
                cursorId = reinterpret_cast<LPCWSTR>(IDC_ARROW);
                break;
            case SystemCursor::IBeam:
                cursorId = reinterpret_cast<LPCWSTR>(IDC_IBEAM);
                break;
            case SystemCursor::Wait:
                cursorId = reinterpret_cast<LPCWSTR>(IDC_WAIT);
                break;
            case SystemCursor::Crosshair:
                cursorId = reinterpret_cast<LPCWSTR>(IDC_CROSS);
                break;
            case SystemCursor::WaitArrow:
                cursorId = reinterpret_cast<LPCWSTR>(IDC_APPSTARTING);
                break;
            case SystemCursor::SizeNWSE:
                cursorId = reinterpret_cast<LPCWSTR>(IDC_SIZENWSE);
                break;
            case SystemCursor::SizeNESW:
                cursorId = reinterpret_cast<LPCWSTR>(IDC_SIZENESW);
                break;
            case SystemCursor::SizeWE:
                cursorId = reinterpret_cast<LPCWSTR>(IDC_SIZEWE);
                break;
            case SystemCursor::SizeNS:
                cursorId = reinterpret_cast<LPCWSTR>(IDC_SIZENS);
                break;
            case SystemCursor::SizeAll:
                cursorId = reinterpret_cast<LPCWSTR>(IDC_SIZEALL);
                break;
            case SystemCursor::No:
                cursorId = reinterpret_cast<LPCWSTR>(IDC_NO);
                break;
            case SystemCursor::Hand:
                cursorId = reinterpret_cast<LPCWSTR>(IDC_HAND);
                break;
        }

        return ::LoadCursorW(nullptr, cursorId);
    }

    void setVisible(bool visible)
    {
        // ShowCursor increments/decrements an internal counter; loop to desired visibility.
        int result;
        if (visible)
        {
            do
            {
                result = ::ShowCursor(TRUE);
            }
            while (result < 0);
        }
        else
        {
            do
            {
                result = ::ShowCursor(FALSE);
            }
            while (result >= 0);
        }
    }

    bool isVisible()
    {
        // Query current cursor visibility via GetCursorInfo
        CURSORINFO cursorInfo;
        cursorInfo.cbSize = sizeof(cursorInfo);
        if (::GetCursorInfo(&cursorInfo))
        {
            return (cursorInfo.flags & CURSOR_SHOWING) != 0;
        }

        return true;
    }

    void setCursor(SystemCursor cursor)
    {
        HCURSOR hCursor = getSystemCursorHandle(cursor);
        if (hCursor != nullptr)
        {
            ::SetCursor(hCursor);
        }
    }

    void resetCursor()
    {
        setCursor(SystemCursor::Arrow);
    }
}
