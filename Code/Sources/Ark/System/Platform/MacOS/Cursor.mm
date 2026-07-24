#include "Ark/System/Cursor.hpp"

#import <AppKit/AppKit.h>
#import <Foundation/Foundation.h>
#include <dispatch/dispatch.h>

namespace Ark::System::Cursor
{
    namespace
    {
        static bool cursorVisible = true;
        static int hideCount = 0;

        static void applyVisibility(bool visible)
        {
            if (visible)
            {
                if (hideCount > 0)
                {
                    [NSCursor unhide];
                    hideCount = 0;
                }
            }
            else
            {
                if (hideCount == 0)
                {
                    [NSCursor hide];
                    hideCount = 1;
                }
            }
            cursorVisible = visible;
        }

        static NSCursor* getSystemCursor(SystemCursor cursor)
        {
            switch (cursor)
            {
                case SystemCursor::Arrow:
                    return [NSCursor arrowCursor];
                case SystemCursor::IBeam:
                    return [NSCursor IBeamCursor];
                case SystemCursor::Wait:
                    // macOS doesn't have a pure wait cursor, use the spinning cursor
                    return [NSCursor arrowCursor]; // Could use a custom spinning cursor
                case SystemCursor::Crosshair:
                    return [NSCursor crosshairCursor];
                case SystemCursor::WaitArrow:
                    return [NSCursor arrowCursor]; // macOS doesn't have a combined wait+arrow
                case SystemCursor::SizeNWSE:
                    // macOS doesn't have diagonal resize cursors, use closest alternative
                    return [NSCursor arrowCursor];
                case SystemCursor::SizeNESW:
                    return [NSCursor arrowCursor];
                case SystemCursor::SizeWE:
                    return [NSCursor resizeLeftRightCursor];
                case SystemCursor::SizeNS:
                    return [NSCursor resizeUpDownCursor];
                case SystemCursor::SizeAll:
                    // macOS doesn't have a 4-way arrow, use open hand as closest
                    return [NSCursor openHandCursor];
                case SystemCursor::No:
                    return [NSCursor operationNotAllowedCursor];
                case SystemCursor::Hand:
                    return [NSCursor pointingHandCursor];
                default:
                    return [NSCursor arrowCursor];
            }
        }

        static void applySystemCursor(SystemCursor cursor)
        {
            NSCursor* nsCursor = getSystemCursor(cursor);
            [nsCursor set];
        }
    }

    void setVisible(bool visible)
    {
        @autoreleasepool
        {
            if ([NSThread isMainThread])
            {
                applyVisibility(visible);
            }
            else
            {
                bool target = visible;
                dispatch_sync(dispatch_get_main_queue(), ^{
                    applyVisibility(target);
                });
            }
        }
    }

    bool isVisible()
    {
        return cursorVisible;
    }

    void setCursor(SystemCursor cursor)
    {
        @autoreleasepool
        {
            if ([NSThread isMainThread])
            {
                applySystemCursor(cursor);
            }
            else
            {
                SystemCursor target = cursor;
                dispatch_sync(dispatch_get_main_queue(), ^{
                    applySystemCursor(target);
                });
            }
        }
    }

    void resetCursor()
    {
        setCursor(SystemCursor::Arrow);
    }
}

