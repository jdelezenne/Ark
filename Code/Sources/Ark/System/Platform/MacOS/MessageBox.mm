// Only compile on macOS
#if defined(ARK_PLATFORM_MACOS)

#import <Cocoa/Cocoa.h>

#include "Ark/System/MessageBox.hpp"
#include "Ark/Core/Types.hpp"
#include "Ark/Strings/String.hpp"
#include "Ark/Strings/StringSlice.hpp"
#include "Ark/Core/Result.hpp"

namespace Ark::System
{
    static NSAlertStyle toAlertStyle(MessageBoxType type)
    {
        switch (type)
        {
            case MessageBoxType::Information: return NSAlertStyleInformational;
            case MessageBoxType::Warning:     return NSAlertStyleWarning;
            case MessageBoxType::Error:       return NSAlertStyleCritical;
        }

        return NSAlertStyleInformational;
    }

    static void configureButtons(NSAlert* alert, MessageBoxButtons buttons)
    {
        switch (buttons)
        {
            case MessageBoxButtons::Ok:
                [alert addButtonWithTitle:@"OK"];
                break;
            case MessageBoxButtons::OkCancel:
                [alert addButtonWithTitle:@"OK"];
                [alert addButtonWithTitle:@"Cancel"];
                break;
            case MessageBoxButtons::YesNo:
                [alert addButtonWithTitle:@"Yes"];
                [alert addButtonWithTitle:@"No"];
                break;
            case MessageBoxButtons::YesNoCancel:
                [alert addButtonWithTitle:@"Yes"];
                [alert addButtonWithTitle:@"No"];
                [alert addButtonWithTitle:@"Cancel"];
                break;
        }
    }

    Ark::Result<MessageBoxButton, Ark::String> showMessageBox(Ark::StringSlice title, Ark::StringSlice message, MessageBoxOptions const& options)
    {
        @autoreleasepool
        {
            NSAlert* alert = [[NSAlert alloc] init];
            alert.alertStyle = toAlertStyle(options.type);
            alert.messageText = [NSString stringWithUTF8String:title.asPointer()];
            alert.informativeText = [NSString stringWithUTF8String:message.asPointer()];

            configureButtons(alert, options.buttons);

            NSModalResponse response = NSModalResponseOK;
            if (options.owner != nullptr)
            {
                NSWindow* win = (__bridge NSWindow*)options.owner;
                response = [alert runModal];
                // For a full sheet implementation we'd beginSheetModalForWindow with a completion handler.
            }
            else
            {
                response = [alert runModal];
            }

            NSArray<NSButton*>* buttonArray = [alert buttons];
            NSInteger index = response - NSAlertFirstButtonReturn;
            if (index < 0 || static_cast<NSUInteger>(index) >= buttonArray.count)
            {
                return Ark::Result<MessageBoxButton, Ark::String>(Ark::unexpectedResult, Ark::String("Alert dismissed"));
            }

            NSString* titleStr = [[buttonArray objectAtIndex:index] title];
            if ([titleStr isEqualToString:@"OK"])    { return Ark::Result<MessageBoxButton, Ark::String>(MessageBoxButton::Ok); }
            if ([titleStr isEqualToString:@"Cancel"]) { return Ark::Result<MessageBoxButton, Ark::String>(MessageBoxButton::Cancel); }
            if ([titleStr isEqualToString:@"Yes"])   { return Ark::Result<MessageBoxButton, Ark::String>(MessageBoxButton::Yes); }
            if ([titleStr isEqualToString:@"No"])    { return Ark::Result<MessageBoxButton, Ark::String>(MessageBoxButton::No); }

            return Ark::Result<MessageBoxButton, Ark::String>(MessageBoxButton::Ok);
        }
    }
}

#endif
