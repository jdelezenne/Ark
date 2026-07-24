#if defined(ARK_PLATFORM_MACOS)

#import <AppKit/AppKit.h>
#import <CoreGraphics/CGDisplayConfiguration.h>

#include "Ark/Application/Platform/MacOS/EventSource.hpp"
#include "Ark/Application/Event.hpp"

namespace Ark::Platform::MacOS
{
    namespace
    {
        void displayReconfigurationCallback(
            CGDirectDisplayID display,
            CGDisplayChangeSummaryFlags flags,
            void* userInfo)
        {
            auto* source = static_cast<MacOSEventSource*>(userInfo);
            if (source == nullptr)
            {
                return;
            }

            Ark::uint32 id = static_cast<Ark::uint32>(display);
            if ((flags & kCGDisplayAddFlag) != 0)
            {
                source->enqueuePending(Ark::DisplayAddedEvent{.displayId = id});
            }
            if ((flags & kCGDisplayRemoveFlag) != 0)
            {
                source->enqueuePending(Ark::DisplayRemovedEvent{.displayId = id});
            }
            if ((flags & kCGDisplayMovedFlag) != 0)
            {
                source->enqueuePending(Ark::DisplayMovedEvent{.displayId = id});
            }
            if ((flags & kCGDisplaySetModeFlag) != 0 || (flags & kCGDisplayDesktopShapeChangedFlag) != 0)
            {
                source->enqueuePending(Ark::DisplayModeChangedEvent{.displayId = id});
            }
        }
    }

    void MacOSEventSource::enqueuePending(Ark::Event&& event)
    {
        Concurrency::LockGuard<Concurrency::Mutex> lock(pendingMutex);
        pendingEvents.append(Ark::move(event));
    }

    void MacOSEventSource::drainPending(Collections::Array<Ark::Event>& outEvents)
    {
        Concurrency::LockGuard<Concurrency::Mutex> lock(pendingMutex);
        for (usize i = 0; i < pendingEvents.getCount(); ++i)
        {
            outEvents.append(Ark::move(pendingEvents[i]));
        }
        pendingEvents.clear();
    }

    MacOSEventSource::MacOSEventSource()
        : notificationObservers(nullptr)
        , lastPasteboardChangeCount(0)
    {
        @autoreleasepool
        {
            if (NSApp == nil)
            {
                [NSApplication sharedApplication];
                [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
                [NSApp finishLaunching];
            }

            lastPasteboardChangeCount = [[NSPasteboard generalPasteboard] changeCount];

            NSMutableArray* observers = [[NSMutableArray alloc] init];
            notificationObservers = (void*)CFBridgingRetain(observers);

            __block MacOSEventSource* source = this;

            id quitObserver = [[NSNotificationCenter defaultCenter]
                addObserverForName:NSApplicationWillTerminateNotification
                            object:NSApp
                             queue:nil
                        usingBlock:^(NSNotification*)
                {
                    source->enqueuePending(Ark::QuitEvent{});
                }];
            [observers addObject:quitObserver];

            id localeObserver = [[NSNotificationCenter defaultCenter]
                addObserverForName:NSCurrentLocaleDidChangeNotification
                            object:nil
                             queue:nil
                        usingBlock:^(NSNotification*)
                {
                    source->enqueuePending(Ark::LocaleChangedEvent{});
                }];
            [observers addObject:localeObserver];

            id themeObserver = [[[NSDistributedNotificationCenter defaultCenter]
                addObserverForName:@"AppleInterfaceThemeChangedNotification"
                            object:nil
                             queue:nil
                        usingBlock:^(NSNotification*)
                {
                    source->enqueuePending(Ark::ThemeChangedEvent{});
                }] retain];
            [observers addObject:themeObserver];

            CGDisplayRegisterReconfigurationCallback(displayReconfigurationCallback, this);
        }
    }

    MacOSEventSource::~MacOSEventSource()
    {
        @autoreleasepool
        {
            CGDisplayRemoveReconfigurationCallback(displayReconfigurationCallback, this);

            if (notificationObservers != nullptr)
            {
                NSMutableArray* observers = (NSMutableArray*)CFBridgingRelease(notificationObservers);

                for (id observer in observers)
                {
                    [[NSNotificationCenter defaultCenter] removeObserver:observer];
                    [[NSDistributedNotificationCenter defaultCenter] removeObserver:observer];
                    [observer release];
                }

                notificationObservers = nullptr;
            }
        }
    }

    void MacOSEventSource::pollEvents(Collections::Array<Ark::Event>& outEvents)
    {
        @autoreleasepool
        {
            drainPending(outEvents);

            NSInteger currentChangeCount = [[NSPasteboard generalPasteboard] changeCount];
            if (currentChangeCount != lastPasteboardChangeCount)
            {
                lastPasteboardChangeCount = currentChangeCount;
                outEvents.append(Ark::ClipboardUpdatedEvent{});
            }

            while (true)
            {
                NSEvent* nsEvent = [NSApp nextEventMatchingMask:NSEventMaskAny
                                                      untilDate:[NSDate distantPast]
                                                         inMode:NSDefaultRunLoopMode
                                                        dequeue:YES];

                if (nsEvent == nil)
                {
                    break;
                }

                [NSApp sendEvent:nsEvent];
            }

            [NSApp updateWindows];
            drainPending(outEvents);
        }
    }

    void MacOSEventSource::waitEvents(Collections::Array<Ark::Event>& outEvents, uint32 timeoutMs)
    {
        @autoreleasepool
        {
            NSDate* untilDate = (timeoutMs == 0)
                ? [NSDate distantFuture]
                : [NSDate dateWithTimeIntervalSinceNow:(timeoutMs / 1000.0)];

            while (true)
            {
                NSEvent* nsEvent = [NSApp nextEventMatchingMask:NSEventMaskAny
                                                      untilDate:untilDate
                                                         inMode:NSDefaultRunLoopMode
                                                        dequeue:YES];

                if (nsEvent == nil)
                {
                    break;
                }

                [NSApp sendEvent:nsEvent];
                untilDate = [NSDate distantPast];
            }

            [NSApp updateWindows];
            pollEvents(outEvents);
        }
    }
}

#endif
