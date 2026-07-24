// Ark::System::Clipboard implementation for macOS
#include "Ark/System/Clipboard.hpp"
#include "Ark/Strings/String.hpp"
#include "Ark/Strings/StringSlice.hpp"
#include "Ark/Core/Types.hpp"

#import <AppKit/AppKit.h>
#import <dispatch/dispatch.h>

namespace Ark::System::Clipboard
{
    static uint64 gSequence = 0;
    static Ark::String gPrimarySelectionText;

    bool setText(Ark::StringSlice text)
    {
        __block BOOL ok = NO;
        auto work = ^{
            @autoreleasepool
            {
                NSPasteboard* pb = [NSPasteboard generalPasteboard];
                [pb clearContents];
                NSString* s = [NSString stringWithUTF8String:text.asPointer()];
                ok = [pb setString:s forType:NSPasteboardTypeString];
                if (ok)
                {
                    ++gSequence;
                }
            }
        };
        if ([NSThread isMainThread])
        {
            work();
        }
        else
        {
            dispatch_sync(dispatch_get_main_queue(), work);
        }
        return ok == YES;
    }

    Ark::String getText()
    {
        __block Ark::String result;
        auto work = ^{
            @autoreleasepool
            {
                NSPasteboard* pb = [NSPasteboard generalPasteboard];
                NSString* s = [pb stringForType:NSPasteboardTypeString];
                if (s == nil)
                {
                    result = Ark::String("");
                }
                else
                {
                    const char* utf8 = [s UTF8String];
                    result = Ark::String(utf8 != nullptr ? utf8 : "");
                }
            }
        };
        if ([NSThread isMainThread])
        {
            work();
        }
        else
        {
            dispatch_sync(dispatch_get_main_queue(), work);
        }
        return result;
    }

    bool hasText()
    {
        __block BOOL ok = NO;
        auto work = ^{
            @autoreleasepool
            {
                NSPasteboard* pb = [NSPasteboard generalPasteboard];
                NSString* s = [pb stringForType:NSPasteboardTypeString];
                ok = (s != nil && [s length] > 0) ? YES : NO;
            }
        };
        if ([NSThread isMainThread])
        {
            work();
        }
        else
        {
            dispatch_sync(dispatch_get_main_queue(), work);
        }
        return ok == YES;
    }

    bool setPrimarySelectionText(Ark::StringSlice text)
    {
        __block BOOL ok = NO;
        auto work = ^{
            gPrimarySelectionText = Ark::String(text.asPointer());
            ++gSequence;
            ok = YES;
        };
        if ([NSThread isMainThread])
        {
            work();
        }
        else
        {
            dispatch_sync(dispatch_get_main_queue(), work);
        }
        return ok == YES;
    }

    Ark::String getPrimarySelectionText()
    {
        return gPrimarySelectionText;
    }

    bool hasPrimarySelectionText()
    {
        return !gPrimarySelectionText.isEmpty();
    }

    uint64 getSequenceNumber()
    {
        @autoreleasepool
        {
            NSPasteboard* pb = [NSPasteboard generalPasteboard];
            static NSInteger lastChangeCount = -1;
            NSInteger changeCount = [pb changeCount];
            if (lastChangeCount != changeCount)
            {
                lastChangeCount = changeCount;
                ++gSequence;
            }
            return gSequence;
        }
    }
}


