#include "Ark/Application/EventDispatcher.hpp"
#include "Ark/Application/Event.hpp"
#include "Ark/Core/Macros.hpp"
#include "Ark/Core/Types.hpp"
#include "Ark/Display/Surface.hpp"
#include "Ark/System/Platform/MacOS/KeyMapping.hpp"

#import <AppKit/AppKit.h>
#import <dispatch/dispatch.h>
#include <vector>

@interface ArkContentView : NSView<NSTextInputClient, NSDraggingDestination>
@property(nonatomic, assign) Ark::Display::WindowId windowId;
@property(nonatomic, assign) Ark::EventDispatcher* eventDispatcher;
@property(nonatomic, assign) NSEventModifierFlags previousModifierFlags;
@property(nonatomic, assign) void (*inputDriverEventCallback)(void* event);
@end

// Global callback storage
static void (*g_inputDriverEventCallback)(void* event) = nullptr;

// C function with external linkage for cross-module access
extern "C" void Ark_Display_MacOS_SetInputDriverEventCallback(void (*callback)(void* event))
{
    g_inputDriverEventCallback = callback;
    
    // Set the callback on all existing views
    @autoreleasepool
    {
        for (NSWindow* window in [NSApp windows])
        {
            NSView* view = [window contentView];
            if ([view isKindOfClass:[ArkContentView class]])
            {
                ArkContentView* cv = (ArkContentView*)view;
                cv.inputDriverEventCallback = callback;
            }
        }
    }
}

@implementation ArkContentView
{
    NSMutableString* _markedText;
    NSRange _markedRange;
    CGImageRef _cachedImage;
}

- (instancetype)initWithFrame:(NSRect)frameRect
{
    self = [super initWithFrame:frameRect];
    if (self)
    {
        _markedText = [[NSMutableString alloc] init];
        _markedRange = NSMakeRange(NSNotFound, 0);
        _cachedImage = nullptr;
        _previousModifierFlags = 0;
        _inputDriverEventCallback = nullptr;
        
        // Register for drag and drop (using modern API)
        [self registerForDraggedTypes:@[NSPasteboardTypeFileURL]];
    }
    return self;
}

- (void)dealloc
{
    [_markedText release];
    if (_cachedImage != nullptr)
    {
        CGImageRelease(_cachedImage);
        _cachedImage = nullptr;
    }
    [super dealloc];
}

- (void)drawRect:(NSRect)dirtyRect
{
    [super drawRect:dirtyRect];
    if (_cachedImage != nullptr)
    {
        CGContextRef ctx = [[NSGraphicsContext currentContext] CGContext];
        if (ctx != nullptr)
        {
            CGRect bounds = NSRectToCGRect([self bounds]);
            CGContextSaveGState(ctx);
            CGContextDrawImage(ctx, bounds, _cachedImage);
            CGContextRestoreGState(ctx);
        }
    }
}

static void ArkSurfaceReleaseCallback([[maybe_unused]] void* info, const void* data, [[maybe_unused]] Ark::usize size)
{
    std::free((void*)data);
}

- (void)updateWithSurface:(Ark::Display::Surface const&)surface
{
    if (surface.pixels.isEmpty() || surface.width == 0 || surface.height == 0)
    {
        return;
    }

    Ark::usize width = surface.width;
    Ark::usize height = surface.height;
    Ark::usize stride = (surface.strideBytes != 0) ? surface.strideBytes : width * 4;

    void* copied = std::malloc(height * stride);
    if (copied == nullptr)
    {
        return;
    }
    memcpy(copied, surface.pixels.asPointer(), height * stride);

    CGDataProviderRef provider = CGDataProviderCreateWithData(nullptr, copied, height * stride, ArkSurfaceReleaseCallback);
    if (provider == nullptr)
    {
        std::free(copied);
        return;
    }

    CGColorSpaceRef cs = CGColorSpaceCreateDeviceRGB();
    const Ark::usize bitsPerComponent = 8;
    const Ark::usize bitsPerPixel = 32;
    CGBitmapInfo bitmapInfo = (CGBitmapInfo)kCGImageAlphaPremultipliedLast | (CGBitmapInfo)kCGBitmapByteOrder32Big;
    CGImageRef image = CGImageCreate(width, height, bitsPerComponent, bitsPerPixel, stride, cs, bitmapInfo, provider, nullptr, false, kCGRenderingIntentDefault);
    CGColorSpaceRelease(cs);
    CGDataProviderRelease(provider);

    if (_cachedImage != nullptr)
    {
        CGImageRelease(_cachedImage);
        _cachedImage = nullptr;
    }
    _cachedImage = image;
    [self setNeedsDisplay:YES];
}

- (BOOL)acceptsFirstResponder
{
    return YES;
}

- (void)keyDown:(NSEvent*)event
{
    // Forward to input driver if registered
    if (self.inputDriverEventCallback != nullptr)
    {
        self.inputDriverEventCallback((__bridge void*)event);
    }

    Ark::EventDispatcher* dispatcher = self.eventDispatcher;
    if (dispatcher != nullptr)
    {
        Ark::Scancode scancode = Ark::Platform::MacOS::mapKeyCodeToScancode([event keyCode]);
        Ark::Key key = Ark::Platform::MacOS::scancodeToKey(scancode);
        
        dispatcher->enqueueEvent(Ark::KeyDownEvent
        {
            .windowId = self.windowId,
            .keyboardId = 0,
            .scancode = scancode,
            .key = key,
            .modifiers = static_cast<Ark::KeyModifiers>([event modifierFlags]),
            .repeat = [event isARepeat],
        });
    }

    [self interpretKeyEvents:@[event]];
}

- (void)keyUp:(NSEvent*)event
{
    // Forward to input driver if registered
    if (self.inputDriverEventCallback != nullptr)
    {
        self.inputDriverEventCallback((__bridge void*)event);
    }

    Ark::EventDispatcher* dispatcher = self.eventDispatcher;
    if (dispatcher != nullptr)
    {
        Ark::Scancode scancode = Ark::Platform::MacOS::mapKeyCodeToScancode([event keyCode]);
        Ark::Key key = Ark::Platform::MacOS::scancodeToKey(scancode);
        
        dispatcher->enqueueEvent(Ark::KeyUpEvent
        {
            .windowId = self.windowId,
            .keyboardId = 0,
            .scancode = scancode,
            .key = key,
            .modifiers = static_cast<Ark::KeyModifiers>([event modifierFlags]),
        });
    }
}

- (void)flagsChanged:(NSEvent*)event
{
    // Forward to input driver if registered
    if (self.inputDriverEventCallback != nullptr)
    {
        self.inputDriverEventCallback((__bridge void*)event);
    }

    Ark::EventDispatcher* dispatcher = self.eventDispatcher;
    if (dispatcher == nullptr)
    {
        return;
    }
    
    NSEventModifierFlags newFlags = [event modifierFlags];
    NSEventModifierFlags changedFlags = newFlags ^ self.previousModifierFlags;
    
    struct ModifierMapping
    {
        NSEventModifierFlags flag;
        Ark::Scancode scancode;
        Ark::Key key;
    };
    
    static ModifierMapping const modifiers[] = {
        { NSEventModifierFlagCapsLock, Ark::Scancode::CapsLock, Ark::Key::CapsLock },
        { NSEventModifierFlagShift, Ark::Scancode::LeftShift, Ark::Key::LeftShift },
        { NSEventModifierFlagControl, Ark::Scancode::LeftControl, Ark::Key::LeftControl },
        { NSEventModifierFlagOption, Ark::Scancode::LeftAlt, Ark::Key::LeftAlt },
        { NSEventModifierFlagCommand, Ark::Scancode::LeftCmd, Ark::Key::LeftCmd },
    };
    
    for (auto const& mod : modifiers)
    {
        if (changedFlags & mod.flag)
        {
            bool isPressed = (newFlags & mod.flag) != 0;
            
            if (isPressed)
            {
                dispatcher->enqueueEvent(Ark::KeyDownEvent
                {
                    .windowId = self.windowId,
                    .keyboardId = 0,
                    .scancode = mod.scancode,
                    .key = mod.key,
                    .modifiers = static_cast<Ark::KeyModifiers>(newFlags),
                    .repeat = false,
                });
            }
            else
            {
                dispatcher->enqueueEvent(Ark::KeyUpEvent
                {
                    .windowId = self.windowId,
                    .keyboardId = 0,
                    .scancode = mod.scancode,
                    .key = mod.key,
                    .modifiers = static_cast<Ark::KeyModifiers>(newFlags),
                });
            }
        }
    }
    
    self.previousModifierFlags = newFlags;
}

- (void)mouseDown:(NSEvent*)event
{
    // Forward to input driver if registered
    if (self.inputDriverEventCallback != nullptr)
    {
        self.inputDriverEventCallback((__bridge void*)event);
    }

    Ark::EventDispatcher* dispatcher = self.eventDispatcher;
    if (dispatcher != nullptr)
    {
        NSWindow* window = [self window];
        CGFloat scale = window != nullptr ? [window backingScaleFactor] : 1.0;
        NSPoint location = [event locationInWindow];
        NSRect bounds = [self bounds];
        Ark::int32 x = static_cast<Ark::int32>(location.x * scale);
        Ark::int32 y = static_cast<Ark::int32>((bounds.size.height - location.y) * scale);
        
        dispatcher->enqueueEvent(Ark::MouseButtonDownEvent
        {
            .windowId = reinterpret_cast<Ark::Display::WindowId>([self window]),
            .mouseId = 0,
            .button = static_cast<Ark::MouseButton>([event buttonNumber] + 1),
            .clicks = static_cast<Ark::int32>([event clickCount]),
            .x = x,
            .y = y,
        });
    }
}

- (void)mouseUp:(NSEvent*)event
{
    // Forward to input driver if registered
    if (self.inputDriverEventCallback != nullptr)
    {
        self.inputDriverEventCallback((__bridge void*)event);
    }

    Ark::EventDispatcher* dispatcher = self.eventDispatcher;
    if (dispatcher != nullptr)
    {
        NSWindow* window = [self window];
        CGFloat scale = window != nullptr ? [window backingScaleFactor] : 1.0;
        NSPoint location = [event locationInWindow];
        NSRect bounds = [self bounds];
        Ark::int32 x = static_cast<Ark::int32>(location.x * scale);
        Ark::int32 y = static_cast<Ark::int32>((bounds.size.height - location.y) * scale);
        
        dispatcher->enqueueEvent(Ark::MouseButtonUpEvent
        {
            .windowId = reinterpret_cast<Ark::Display::WindowId>([self window]),
            .mouseId = 0,
            .button = static_cast<Ark::MouseButton>([event buttonNumber] + 1),
            .clicks = static_cast<Ark::int32>([event clickCount]),
            .x = x,
            .y = y,
        });
    }
}

- (void)rightMouseDown:(NSEvent*)event
{
    Ark::EventDispatcher* dispatcher = self.eventDispatcher;
    if (dispatcher != nullptr)
    {
        NSWindow* window = [self window];
        CGFloat scale = window != nullptr ? [window backingScaleFactor] : 1.0;
        NSPoint location = [event locationInWindow];
        NSRect bounds = [self bounds];
        Ark::int32 x = static_cast<Ark::int32>(location.x * scale);
        Ark::int32 y = static_cast<Ark::int32>((bounds.size.height - location.y) * scale);
        
        dispatcher->enqueueEvent(Ark::MouseButtonDownEvent
        {
            .windowId = reinterpret_cast<Ark::Display::WindowId>([self window]),
            .mouseId = 0,
            .button = static_cast<Ark::MouseButton>([event buttonNumber] + 1),
            .clicks = static_cast<Ark::int32>([event clickCount]),
            .x = x,
            .y = y,
        });
    }
}

- (void)rightMouseUp:(NSEvent*)event
{
    Ark::EventDispatcher* dispatcher = self.eventDispatcher;
    if (dispatcher != nullptr)
    {
        NSWindow* window = [self window];
        CGFloat scale = window != nullptr ? [window backingScaleFactor] : 1.0;
        NSPoint location = [event locationInWindow];
        NSRect bounds = [self bounds];
        Ark::int32 x = static_cast<Ark::int32>(location.x * scale);
        Ark::int32 y = static_cast<Ark::int32>((bounds.size.height - location.y) * scale);
        
        dispatcher->enqueueEvent(Ark::MouseButtonUpEvent
        {
            .windowId = reinterpret_cast<Ark::Display::WindowId>([self window]),
            .mouseId = 0,
            .button = static_cast<Ark::MouseButton>([event buttonNumber] + 1),
            .clicks = static_cast<Ark::int32>([event clickCount]),
            .x = x,
            .y = y,
        });
    }
}

- (void)otherMouseDown:(NSEvent*)event
{
    Ark::EventDispatcher* dispatcher = self.eventDispatcher;
    if (dispatcher != nullptr)
    {
        NSWindow* window = [self window];
        CGFloat scale = window != nullptr ? [window backingScaleFactor] : 1.0;
        NSPoint location = [event locationInWindow];
        NSRect bounds = [self bounds];
        Ark::int32 x = static_cast<Ark::int32>(location.x * scale);
        Ark::int32 y = static_cast<Ark::int32>((bounds.size.height - location.y) * scale);
        
        dispatcher->enqueueEvent(Ark::MouseButtonDownEvent
        {
            .windowId = reinterpret_cast<Ark::Display::WindowId>([self window]),
            .mouseId = 0,
            .button = static_cast<Ark::MouseButton>([event buttonNumber] + 1),
            .clicks = static_cast<Ark::int32>([event clickCount]),
            .x = x,
            .y = y,
        });
    }
}

- (void)otherMouseUp:(NSEvent*)event
{
    Ark::EventDispatcher* dispatcher = self.eventDispatcher;
    if (dispatcher != nullptr)
    {
        NSPoint location = [event locationInWindow];
        NSRect bounds = [self bounds];
        Ark::int32 x = static_cast<Ark::int32>(location.x);
        Ark::int32 y = static_cast<Ark::int32>(bounds.size.height - location.y);
        
        dispatcher->enqueueEvent(Ark::MouseButtonUpEvent
        {
            .windowId = reinterpret_cast<Ark::Display::WindowId>([self window]),
            .mouseId = 0,
            .button = static_cast<Ark::MouseButton>([event buttonNumber] + 1),
            .clicks = static_cast<Ark::int32>([event clickCount]),
            .x = x,
            .y = y,
        });
    }
}

- (void)mouseMoved:(NSEvent*)event
{
    // Forward to input driver if registered
    if (self.inputDriverEventCallback != nullptr)
    {
        self.inputDriverEventCallback((__bridge void*)event);
    }

    NSPoint location = [event locationInWindow];
    NSPoint localPoint = [self convertPoint:location fromView:nil];
    NSWindow* window = [self window];
    CGFloat scale = window != nil ? [window backingScaleFactor] : 1.0;
    
    if (!NSPointInRect(localPoint, [self bounds]))
    {
        return;
    }
    
    Ark::EventDispatcher* dispatcher = self.eventDispatcher;
    if (dispatcher != nullptr)
    {
        NSRect bounds = [self bounds];
        Ark::int32 x = static_cast<Ark::int32>(location.x * scale);
        Ark::int32 y = static_cast<Ark::int32>((bounds.size.height - location.y) * scale);
        
        dispatcher->enqueueEvent(Ark::MouseMotionEvent
        {
            .windowId = reinterpret_cast<Ark::Display::WindowId>([self window]),
            .mouseId = 0,
            .x = x,
            .y = y,
            .dx = static_cast<Ark::int32>([event deltaX] * scale),
            .dy = static_cast<Ark::int32>([event deltaY] * scale),
            .state = Ark::MouseButtons::None,
        });
    }
}

- (void)mouseDragged:(NSEvent*)event
{
    // Forward to input driver if registered
    if (self.inputDriverEventCallback != nullptr)
    {
        self.inputDriverEventCallback((__bridge void*)event);
    }

    Ark::EventDispatcher* dispatcher = self.eventDispatcher;
    if (dispatcher != nullptr)
    {
        NSPoint location = [event locationInWindow];
        NSWindow* window = [self window];
        CGFloat scale = window != nil ? [window backingScaleFactor] : 1.0;
        NSRect bounds = [self bounds];
        Ark::int32 x = static_cast<Ark::int32>(location.x * scale);
        Ark::int32 y = static_cast<Ark::int32>((bounds.size.height - location.y) * scale);
        
        dispatcher->enqueueEvent(Ark::MouseMotionEvent
        {
            .windowId = reinterpret_cast<Ark::Display::WindowId>([self window]),
            .mouseId = 0,
            .x = x,
            .y = y,
            .dx = static_cast<Ark::int32>([event deltaX] * scale),
            .dy = static_cast<Ark::int32>([event deltaY] * scale),
            .state = Ark::MouseButtons::Left,
        });
    }
}

- (void)rightMouseDragged:(NSEvent*)event
{
    [self mouseDragged:event];
}

- (void)otherMouseDragged:(NSEvent*)event
{
    [self mouseDragged:event];
}

- (void)scrollWheel:(NSEvent*)event
{
    // Forward to input driver if registered
    if (self.inputDriverEventCallback != nullptr)
    {
        self.inputDriverEventCallback((__bridge void*)event);
    }

    Ark::EventDispatcher* dispatcher = self.eventDispatcher;
    if (dispatcher != nullptr)
    {
        dispatcher->enqueueEvent(Ark::MouseWheelEvent
        {
            .windowId = reinterpret_cast<Ark::Display::WindowId>([self window]),
            .mouseId = 0,
            .x = static_cast<Ark::float32>([event scrollingDeltaX]),
            .y = static_cast<Ark::float32>([event scrollingDeltaY]),
            .direction = ([event isDirectionInvertedFromDevice] ? Ark::MouseWheelDirection::Flipped : Ark::MouseWheelDirection::Normal),
        });
    }
}

- (void)magnifyWithEvent:(NSEvent*)event
{
    Ark::EventDispatcher* dispatcher = self.eventDispatcher;
    if (dispatcher != nullptr)
    {
        dispatcher->enqueueEvent(Ark::GestureMagnifyEvent
        {
            .windowId = reinterpret_cast<Ark::Display::WindowId>([self window]),
            .magnitude = static_cast<Ark::float32>([event magnification]),
        });
    }
}

- (void)rotateWithEvent:(NSEvent*)event
{
    Ark::EventDispatcher* dispatcher = self.eventDispatcher;
    if (dispatcher != nullptr)
    {
        dispatcher->enqueueEvent(Ark::GestureRotateEvent
        {
            .windowId = reinterpret_cast<Ark::Display::WindowId>([self window]),
            .rotation = static_cast<Ark::float32>([event rotation]),
        });
    }
}

- (void)touchesBeganWithEvent:(NSEvent*)event
{
    Ark::EventDispatcher* dispatcher = self.eventDispatcher;
    if (dispatcher != nullptr)
    {
        Ark::Collections::Array<Ark::TouchPoint> touchPoints;
        NSSet* touches = [event touchesMatchingPhase:NSTouchPhaseBegan inView:self];
        
        for (NSTouch* touch in touches)
        {
            NSPoint location = touch.normalizedPosition;
            NSSize deviceSize = touch.deviceSize;
            NSRect bounds = [self bounds];
            
            touchPoints.append(Ark::TouchPoint
            {
                .id = reinterpret_cast<Ark::UInt64>(touch.identity),
                .x = static_cast<Ark::float32>(location.x * bounds.size.width),
                .y = static_cast<Ark::float32>(location.y * bounds.size.height),
                .normalizedX = static_cast<Ark::float32>(location.x),
                .normalizedY = static_cast<Ark::float32>(location.y),
                .pressure = 1.0f,
            });
        }
        
        if (touchPoints.getCount() > 0)
        {
            dispatcher->enqueueEvent(Ark::TouchBeganEvent
            {
                .windowId = self.windowId,
                .touches = Ark::move(touchPoints),
            });
        }
    }
}

- (void)touchesMovedWithEvent:(NSEvent*)event
{
    Ark::EventDispatcher* dispatcher = self.eventDispatcher;
    if (dispatcher != nullptr)
    {
        Ark::Collections::Array<Ark::TouchPoint> touchPoints;
        NSSet* touches = [event touchesMatchingPhase:NSTouchPhaseMoved inView:self];
        
        for (NSTouch* touch in touches)
        {
            NSPoint location = touch.normalizedPosition;
            NSRect bounds = [self bounds];
            
            touchPoints.append(Ark::TouchPoint
            {
                .id = reinterpret_cast<Ark::UInt64>(touch.identity),
                .x = static_cast<Ark::float32>(location.x * bounds.size.width),
                .y = static_cast<Ark::float32>(location.y * bounds.size.height),
                .normalizedX = static_cast<Ark::float32>(location.x),
                .normalizedY = static_cast<Ark::float32>(location.y),
                .pressure = 1.0f,
            });
        }
        
        if (touchPoints.getCount() > 0)
        {
            dispatcher->enqueueEvent(Ark::TouchMovedEvent
            {
                .windowId = self.windowId,
                .touches = Ark::move(touchPoints),
            });
        }
    }
}

- (void)touchesEndedWithEvent:(NSEvent*)event
{
    Ark::EventDispatcher* dispatcher = self.eventDispatcher;
    if (dispatcher != nullptr)
    {
        Ark::Collections::Array<Ark::TouchPoint> touchPoints;
        NSSet* touches = [event touchesMatchingPhase:NSTouchPhaseEnded inView:self];
        
        for (NSTouch* touch in touches)
        {
            NSPoint location = touch.normalizedPosition;
            NSRect bounds = [self bounds];
            
            touchPoints.append(Ark::TouchPoint
            {
                .id = reinterpret_cast<Ark::UInt64>(touch.identity),
                .x = static_cast<Ark::float32>(location.x * bounds.size.width),
                .y = static_cast<Ark::float32>(location.y * bounds.size.height),
                .normalizedX = static_cast<Ark::float32>(location.x),
                .normalizedY = static_cast<Ark::float32>(location.y),
                .pressure = 1.0f,
            });
        }
        
        if (touchPoints.getCount() > 0)
        {
            dispatcher->enqueueEvent(Ark::TouchEndedEvent
            {
                .windowId = self.windowId,
                .touches = Ark::move(touchPoints),
            });
        }
    }
}

- (void)touchesCancelledWithEvent:(NSEvent*)event
{
    Ark::EventDispatcher* dispatcher = self.eventDispatcher;
    if (dispatcher != nullptr)
    {
        Ark::Collections::Array<Ark::TouchPoint> touchPoints;
        NSSet* touches = [event touchesMatchingPhase:NSTouchPhaseCancelled inView:self];
        
        for (NSTouch* touch in touches)
        {
            NSPoint location = touch.normalizedPosition;
            NSRect bounds = [self bounds];
            
            touchPoints.append(Ark::TouchPoint
            {
                .id = reinterpret_cast<Ark::UInt64>(touch.identity),
                .x = static_cast<Ark::float32>(location.x * bounds.size.width),
                .y = static_cast<Ark::float32>(location.y * bounds.size.height),
                .normalizedX = static_cast<Ark::float32>(location.x),
                .normalizedY = static_cast<Ark::float32>(location.y),
                .pressure = 1.0f,
            });
        }
        
        if (touchPoints.getCount() > 0)
        {
            dispatcher->enqueueEvent(Ark::TouchCancelledEvent
            {
                .windowId = self.windowId,
                .touches = Ark::move(touchPoints),
            });
        }
    }
}

- (void)mouseEntered:(NSEvent*)event
{
    Ark::EventDispatcher* dispatcher = self.eventDispatcher;
    if (dispatcher != nullptr)
    {
        dispatcher->enqueueEvent(Ark::WindowMouseEnterEvent{.windowId = reinterpret_cast<Ark::Display::WindowId>([self window])});
    }
}

- (void)mouseExited:(NSEvent*)event
{
    Ark::EventDispatcher* dispatcher = self.eventDispatcher;
    if (dispatcher != nullptr)
    {
        dispatcher->enqueueEvent(Ark::WindowMouseLeaveEvent{.windowId = reinterpret_cast<Ark::Display::WindowId>([self window])});
    }
}

- (void)updateTrackingAreas
{
    [super updateTrackingAreas];
    
    for (NSTrackingArea* area in [self trackingAreas])
    {
        [self removeTrackingArea:area];
    }
    
    NSTrackingAreaOptions options = NSTrackingMouseEnteredAndExited | 
                                     NSTrackingActiveAlways |
                                     NSTrackingInVisibleRect;
    
    NSTrackingArea* trackingArea = [[NSTrackingArea alloc] initWithRect:[self bounds]
                                                                options:options
                                                                  owner:self
                                                                userInfo:nil];
    [self addTrackingArea:trackingArea];
}

- (void)insertText:(id)string replacementRange:(NSRange)replacementRange
{
    if ([string isKindOfClass:[NSAttributedString class]])
    {
        string = [string string];
    }
    
    [_markedText setString:@""];
    _markedRange = NSMakeRange(NSNotFound, 0);
    
    Ark::EventDispatcher* dispatcher = self.eventDispatcher;
    if (dispatcher != nullptr)
    {
        dispatcher->enqueueEvent(Ark::TextInputEvent
        {
            .windowId = self.windowId,
            .text = Ark::String([string UTF8String]),
        });
    }
}

- (void)setMarkedText:(id)string selectedRange:(NSRange)selectedRange replacementRange:(NSRange)replacementRange
{
    if ([string isKindOfClass:[NSAttributedString class]])
    {
        string = [string string];
    }
    
    [_markedText setString:string];
    _markedRange = NSMakeRange(0, [string length]);
    
    Ark::EventDispatcher* dispatcher = self.eventDispatcher;
    if (dispatcher != nullptr && [string length] > 0)
    {
        dispatcher->enqueueEvent(Ark::TextEditingEvent
        {
            .windowId = self.windowId,
            .text = Ark::String([string UTF8String]),
            .start = static_cast<Ark::int32>(selectedRange.location),
            .length = static_cast<Ark::int32>(selectedRange.length),
        });
    }
}

- (void)unmarkText
{
    [_markedText setString:@""];
    _markedRange = NSMakeRange(NSNotFound, 0);
}

- (NSRange)selectedRange
{
    return NSMakeRange(NSNotFound, 0);
}

- (NSRange)markedRange
{
    return _markedRange;
}

- (BOOL)hasMarkedText
{
    return _markedRange.location != NSNotFound;
}

- (NSAttributedString*)attributedSubstringForProposedRange:(NSRange)range actualRange:(NSRangePointer)actualRange
{
    return nil;
}

- (NSArray<NSAttributedStringKey>*)validAttributesForMarkedText
{
    return @[];
}

- (NSRect)firstRectForCharacterRange:(NSRange)range actualRange:(NSRangePointer)actualRange
{
    NSRect rect = [self bounds];
    NSPoint origin = [[self window] convertRectToScreen:rect].origin;
    return NSMakeRect(origin.x, origin.y, 0, 0);
}

- (NSUInteger)characterIndexForPoint:(NSPoint)point
{
    return NSNotFound;
}

- (void)doCommandBySelector:(SEL)selector
{
}

// NSDraggingDestination protocol methods
- (NSDragOperation)draggingEntered:(id<NSDraggingInfo>)sender
{
    NSPasteboard* pboard = [sender draggingPasteboard];
    NSArray* types = [pboard types];
    if ([types containsObject:NSPasteboardTypeFileURL])
    {
        // Defer event dispatch to avoid reentrancy issues
        Ark::Display::WindowId windowId = self.windowId;
        dispatch_async(dispatch_get_main_queue(), ^{
            Ark::EventDispatcher* dispatcher = self.eventDispatcher;
            if (dispatcher != nullptr)
            {
                dispatcher->enqueueEvent(Ark::DropBeginEvent
                {
                    .windowId = windowId,
                });
            }
        });
        return NSDragOperationCopy;
    }
    return NSDragOperationNone;
}

- (NSDragOperation)draggingUpdated:(id<NSDraggingInfo>)sender
{
    NSPasteboard* pboard = [sender draggingPasteboard];
    NSArray* types = [pboard types];
    if ([types containsObject:NSPasteboardTypeFileURL])
    {
        return NSDragOperationCopy;
    }
    return NSDragOperationNone;
}

- (void)draggingExited:(id<NSDraggingInfo>)sender
{
    ARK_UNUSED(sender);
    // Note: We don't send DropCompleteEvent here because the drag was cancelled
    // DropCompleteEvent is only sent after a successful drop in performDragOperation
}

- (BOOL)performDragOperation:(id<NSDraggingInfo>)sender
{
    NSPasteboard* pboard = [sender draggingPasteboard];
    NSArray* types = [pboard types];
    
    NSArray* files = nil;
    if ([types containsObject:NSPasteboardTypeFileURL])
    {
        // Modern API: use file URLs
        files = [pboard readObjectsForClasses:@[[NSURL class]] options:nil];
        if (files != nil && [files count] > 0)
        {
            // Convert NSURL array to NSString array
            NSMutableArray* filePaths = [NSMutableArray arrayWithCapacity:[files count]];
            for (NSURL* url in files)
            {
                NSString* path = [url path];
                if (path != nil)
                {
                    [filePaths addObject:path];
                }
            }
            files = filePaths;
        }
    }
    
    if (files == nil || [files count] == 0)
    {
        return NO;
    }

    // Capture values needed for event dispatch
    Ark::Display::WindowId windowId = self.windowId;
    NSPoint location = [sender draggingLocation];
    NSWindow* window = [self window];
    CGFloat scale = window != nil ? [window backingScaleFactor] : 1.0;
    NSRect bounds = [self bounds];
    Ark::float32 x = static_cast<Ark::float32>(location.x * scale);
    Ark::float32 y = static_cast<Ark::float32>((bounds.size.height - location.y) * scale);
    
    // Convert file paths to C++ strings while still in the drag context
    // Store them in a vector to ensure they're valid when the async block executes
    Ark::Collections::Array<Ark::String> filePathStrings;
    filePathStrings.reserve([files count]);
    for (NSString* filePath in files)
    {
        const char* utf8Path = [filePath UTF8String];
        if (utf8Path != nullptr)
        {
            filePathStrings.append(Ark::String(utf8Path));
        }
    }

    // Defer event dispatch to avoid reentrancy issues
    dispatch_async(dispatch_get_main_queue(), ^{
        Ark::EventDispatcher* dispatcher = self.eventDispatcher;
        if (dispatcher == nullptr)
        {
            return;
        }

        // Send DropFileEvent for each file
        for (auto const& filePath : filePathStrings)
        {
            dispatcher->enqueueEvent(Ark::DropFileEvent
            {
                .windowId = windowId,
                .file = filePath,
                .x = x,
                .y = y,
            });
        }

        // Send DropCompleteEvent after all files
        dispatcher->enqueueEvent(Ark::DropCompleteEvent
        {
            .windowId = windowId,
        });
    });

    return YES;
}

- (void)draggingEnded:(id<NSDraggingInfo>)sender
{
    ARK_UNUSED(sender);
    // This is called after performDragOperation completes
    // All events have already been sent, so nothing to do here
}

@end

@interface ArkWindowDelegate : NSObject<NSWindowDelegate>
@property(nonatomic, assign) Ark::Display::WindowId windowId;
@property(nonatomic, assign) Ark::EventDispatcher* eventDispatcher;
@end

@implementation ArkWindowDelegate

- (void)windowWillClose:(NSNotification*)notification
{
    Ark::EventDispatcher* dispatcher = self.eventDispatcher;
    if (dispatcher != nullptr)
    {
        dispatcher->enqueueEvent(Ark::WindowCloseRequestedEvent{.windowId = self.windowId});
        dispatcher->enqueueEvent(Ark::WindowDestroyedEvent{.windowId = self.windowId});
        dispatcher->enqueueEvent(Ark::WindowClosedEvent{.windowId = self.windowId});
    }
}

- (void)windowDidResize:(NSNotification*)notification
{
    NSWindow* win = (NSWindow*)notification.object;
    Ark::EventDispatcher* dispatcher = self.eventDispatcher;
    
    if (win != nil && dispatcher != nullptr)
    {
        NSRect contentRect = [[win contentView] frame];
        Ark::uint32 w = static_cast<Ark::uint32>(contentRect.size.width);
        Ark::uint32 h = static_cast<Ark::uint32>(contentRect.size.height);
        
        dispatcher->enqueueEvent(Ark::WindowResizedEvent
        {
            .windowId = self.windowId,
            .width = w,
            .height = h,
        });
        
        NSSize pixelSize = [win convertRectToBacking:contentRect].size;
        dispatcher->enqueueEvent(Ark::WindowPixelSizeChangedEvent
        {
            .windowId = self.windowId,
            .width = static_cast<Ark::uint32>(pixelSize.width),
            .height = static_cast<Ark::uint32>(pixelSize.height),
        });
    }
}

- (void)windowDidMove:(NSNotification*)notification
{
    NSWindow* win = (NSWindow*)notification.object;
    Ark::EventDispatcher* dispatcher = self.eventDispatcher;
    
    if (win != nil && dispatcher != nullptr)
    {
        NSRect frame = [win frame];
        NSRect screenFrame = [[NSScreen mainScreen] frame];
        CGFloat screenHeight = screenFrame.size.height;
        Ark::int32 x = static_cast<Ark::int32>(frame.origin.x);
        Ark::int32 y = static_cast<Ark::int32>(screenHeight - frame.origin.y - frame.size.height);
        
        dispatcher->enqueueEvent(Ark::WindowMovedEvent
        {
            .windowId = self.windowId,
            .x = x,
            .y = y,
        });
    }
}

- (void)windowDidBecomeKey:(NSNotification*)notification
{
    NSWindow* win = (NSWindow*)notification.object;
    if (win != nil)
    {
        NSView* contentView = [win contentView];
        if (contentView != nil && [contentView isKindOfClass:[ArkContentView class]])
        {
            // Make the content view first responder to ensure keyboard input works
            // This is especially important for layer-backed views (Metal/OpenGL)
            [win makeFirstResponder:contentView];
        }
    }
    
    Ark::EventDispatcher* dispatcher = self.eventDispatcher;
    if (dispatcher != nullptr)
    {
        dispatcher->enqueueEvent(Ark::WindowFocusGainedEvent{.windowId = self.windowId});
    }
}

- (void)windowDidResignKey:(NSNotification*)notification
{
    Ark::EventDispatcher* dispatcher = self.eventDispatcher;
    if (dispatcher != nullptr)
    {
        dispatcher->enqueueEvent(Ark::WindowFocusLostEvent{.windowId = self.windowId});
    }
}

- (void)windowDidMiniaturize:(NSNotification*)notification
{
    Ark::EventDispatcher* dispatcher = self.eventDispatcher;
    if (dispatcher != nullptr)
    {
        dispatcher->enqueueEvent(Ark::WindowMinimizedEvent{.windowId = self.windowId});
    }
}

- (void)windowDidDeminiaturize:(NSNotification*)notification
{
    Ark::EventDispatcher* dispatcher = self.eventDispatcher;
    if (dispatcher != nullptr)
    {
        dispatcher->enqueueEvent(Ark::WindowRestoredEvent{.windowId = self.windowId});
    }
}

- (void)windowDidChangeScreen:(NSNotification*)notification
{
    Ark::EventDispatcher* dispatcher = self.eventDispatcher;
    if (dispatcher != nullptr)
    {
        dispatcher->enqueueEvent(Ark::WindowDisplayChangedEvent
        {
            .windowId = self.windowId,
            .displayId = 0,
        });
    }
}

- (void)windowDidChangeBackingProperties:(NSNotification*)notification
{
    NSWindow* win = (NSWindow*)notification.object;
    Ark::EventDispatcher* dispatcher = self.eventDispatcher;
    
    if (win != nil && dispatcher != nullptr)
    {
        CGFloat scale = [win backingScaleFactor];
        dispatcher->enqueueEvent(Ark::WindowDisplayScaleChangedEvent
        {
            .windowId = self.windowId,
            .scale = static_cast<Ark::float32>(scale),
        });
    }
}

- (void)windowDidEnterFullScreen:(NSNotification*)notification
{
    Ark::EventDispatcher* dispatcher = self.eventDispatcher;
    if (dispatcher != nullptr)
    {
        dispatcher->enqueueEvent(Ark::WindowEnteredFullscreenEvent{.windowId = self.windowId});
    }
}

- (void)windowDidExitFullScreen:(NSNotification*)notification
{
    Ark::EventDispatcher* dispatcher = self.eventDispatcher;
    if (dispatcher != nullptr)
    {
        dispatcher->enqueueEvent(Ark::WindowLeftFullscreenEvent{.windowId = self.windowId});
    }
}

- (void)windowDidChangeOcclusionState:(NSNotification*)notification
{
    NSWindow* win = (NSWindow*)notification.object;
    Ark::EventDispatcher* dispatcher = self.eventDispatcher;
    
    if (win != nil && dispatcher != nullptr)
    {
        if ([win occlusionState] & NSWindowOcclusionStateVisible)
        {
            dispatcher->enqueueEvent(Ark::WindowExposedEvent{.windowId = self.windowId});
        }
        else
        {
            dispatcher->enqueueEvent(Ark::WindowOccludedEvent{.windowId = self.windowId});
        }
    }
}

- (void)windowDidBecomeVisible:(NSNotification*)notification
{
    Ark::EventDispatcher* dispatcher = self.eventDispatcher;
    if (dispatcher != nullptr)
    {
        dispatcher->enqueueEvent(Ark::WindowShownEvent{.windowId = self.windowId});
    }
}

- (void)windowDidBecomeInvisible:(NSNotification*)notification
{
    Ark::EventDispatcher* dispatcher = self.eventDispatcher;
    if (dispatcher != nullptr)
    {
        dispatcher->enqueueEvent(Ark::WindowHiddenEvent{.windowId = self.windowId});
    }
}

- (void)windowDidZoom:(NSNotification*)notification
{
    NSWindow* win = (NSWindow*)notification.object;
    Ark::EventDispatcher* dispatcher = self.eventDispatcher;
    
    if (win != nil && dispatcher != nullptr)
    {
        if ([win isZoomed])
        {
            dispatcher->enqueueEvent(Ark::WindowMaximizedEvent{.windowId = self.windowId});
        }
        else
        {
            dispatcher->enqueueEvent(Ark::WindowRestoredEvent{.windowId = self.windowId});
        }
    }
}

@end

#include "Ark/Display/Platform/MacOS/Window.hpp"

namespace Ark::Display::Platform::MacOS
{
    class MacWindow final : public Window
    {
    public:
        explicit MacWindow(WindowCreateInfo const& info)
        {
            @autoreleasepool
            {
                if (NSApp == nil)
                {
                    [NSApplication sharedApplication];
                    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
                    [NSApp finishLaunching];
                }

                NSUInteger styleMask = NSWindowStyleMaskTitled | 
                                      NSWindowStyleMaskClosable | 
                                      NSWindowStyleMaskMiniaturizable;

                if ((static_cast<uint64>(info.flags) & static_cast<uint64>(WindowFlags::Resizable)) != 0)
                {
                    styleMask |= NSWindowStyleMaskResizable;
                }

                if ((static_cast<uint64>(info.flags) & static_cast<uint64>(WindowFlags::Borderless)) != 0)
                {
                    styleMask = NSWindowStyleMaskBorderless;
                }

                NSRect contentRect = NSMakeRect(0, 0, info.width, info.height);
                
                window = [[NSWindow alloc] initWithContentRect:contentRect
                                                      styleMask:styleMask
                                                        backing:NSBackingStoreBuffered
                                                          defer:NO];

                [window setReleasedWhenClosed:NO];
                [window setAcceptsMouseMovedEvents:YES];
                [window setTitle:[NSString stringWithUTF8String:info.title.asPointer()]];

                WindowId winId = reinterpret_cast<WindowId>(window);

                delegate = [[ArkWindowDelegate alloc] init];
                delegate.windowId = winId;
                delegate.eventDispatcher = info.eventDispatcher;
                [window setDelegate:delegate];

                ArkContentView* contentView = [[ArkContentView alloc] initWithFrame:contentRect];
                contentView.windowId = winId;
                contentView.eventDispatcher = info.eventDispatcher;
                
                if ((static_cast<uint64>(info.flags) & static_cast<uint64>(WindowFlags::AcceptTouchEvents)) != 0)
                {
                    [contentView setAllowedTouchTypes:NSTouchTypeMaskDirect | NSTouchTypeMaskIndirect];
                    [contentView setWantsRestingTouches:YES];
                }
                
                if (g_inputDriverEventCallback != nullptr)
                {
                    contentView.inputDriverEventCallback = g_inputDriverEventCallback;
                }
                
                [window setContentView:contentView];
                [contentView release];

                if (info.parentId)
                {
                    parentWindowId = info.parentId;
                    NSWindow* parentNSWindow = reinterpret_cast<NSWindow*>(*info.parentId);
                    if (parentNSWindow != nullptr)
                    {
                        [parentNSWindow addChildWindow:window ordered:NSWindowAbove];
                    }
                }

                if ((static_cast<uint64>(info.flags) & static_cast<uint64>(WindowFlags::AlwaysOnTop)) != 0)
                {
                    [window setLevel:NSFloatingWindowLevel];
                }

                if ((static_cast<uint64>(info.flags) & static_cast<uint64>(WindowFlags::Transparent)) != 0)
                {
                    [window setOpaque:NO];
                    [window setBackgroundColor:[NSColor clearColor]];
                }

                positionWindow(info);

                if ((static_cast<uint64>(info.flags) & static_cast<uint64>(WindowFlags::Hidden)) == 0)
                {
                    [window makeKeyAndOrderFront:nil];
                    [NSApp activateIgnoringOtherApps:YES];
                }
            }
        }

        ~MacWindow() override
        {
            @autoreleasepool
            {
                if (mouseGrabbed)
                {
                    CGAssociateMouseAndMouseCursorPosition(YES);
                }

                if (window != nullptr)
                {
                    [window setDelegate:nil];
                    [window close];
                    [window release];
                    window = nullptr;
                }

                if (delegate != nullptr)
                {
                    [delegate release];
                    delegate = nullptr;
                }
            }
        }

        WindowId getId() const override
        {
            return reinterpret_cast<WindowId>(window);
        }

        WindowFlags getFlags() const override
        {
            WindowFlags flags = WindowFlags::None;
            
            if (window == nullptr)
            {
                return flags;
            }

            @autoreleasepool
            {
                NSUInteger mask = [window styleMask];
                
                if ((mask & NSWindowStyleMaskFullScreen) != 0)
                {
                    flags |= WindowFlags::Fullscreen;
                }
                
                if (![window isVisible])
                {
                    flags |= WindowFlags::Hidden;
                }
                
                if ((mask & NSWindowStyleMaskTitled) == 0)
                {
                    flags |= WindowFlags::Borderless;
                }
                
                if ((mask & NSWindowStyleMaskResizable) != 0)
                {
                    flags |= WindowFlags::Resizable;
                }
                
                if ([window isMiniaturized])
                {
                    flags |= WindowFlags::Minimized;
                }
                
                if ([window isZoomed])
                {
                    flags |= WindowFlags::Maximized;
                }
                
                if ([window level] == NSFloatingWindowLevel)
                {
                    flags |= WindowFlags::AlwaysOnTop;
                }
                
                if (mouseGrabbed)
                {
                    flags |= WindowFlags::MouseGrabbed;
                }
                
                if (keyboardGrabbed)
                {
                    flags |= WindowFlags::KeyboardGrabbed;
                }
            }
            
            return flags;
        }

        void* getNativeHandle() const override
        {
            return (__bridge void*)window;
        }

        void show() override
        {
            if (window == nullptr)
            {
                return;
            }

            @autoreleasepool
            {
                [window makeKeyAndOrderFront:nil];
                [NSApp activateIgnoringOtherApps:YES];
                
                // Ensure the content view becomes first responder for keyboard input
                // This is critical for layer-backed views (Metal/OpenGL) to receive input events properly
                NSView* contentView = [window contentView];
                if (contentView != nil)
                {
                    [window makeFirstResponder:contentView];
                }
            }
        }

        void hide() override
        {
            if (window == nullptr)
            {
                return;
            }

            @autoreleasepool
            {
                [window orderOut:nil];
            }
        }

        void setTitle(String const& title) override
        {
            if (window == nullptr)
            {
                return;
            }

            @autoreleasepool
            {
                [window setTitle:[NSString stringWithUTF8String:title.asPointer()]];
            }
        }

        String getTitle() const override
        {
            if (window == nullptr)
            {
                return String();
            }

            @autoreleasepool
            {
                char const* utf8 = [[window title] UTF8String];
                return String(utf8 != nullptr ? utf8 : "");
            }
        }

        void setPosition(int32 x, int32 y) override
        {
            if (window == nullptr)
            {
                return;
            }

            @autoreleasepool
            {
                NSRect frame = [window frame];
                NSRect screenFrame = [[NSScreen mainScreen] frame];
                CGFloat screenHeight = screenFrame.size.height;
                CGFloat newY = screenHeight - y - frame.size.height;
                
                [window setFrameOrigin:NSMakePoint(x, newY)];
            }
        }

        void getPosition(int32& x, int32& y) const override
        {
            if (window == nullptr)
            {
                x = 0;
                y = 0;
                return;
            }

            @autoreleasepool
            {
                NSRect frame = [window frame];
                NSRect screenFrame = [[NSScreen mainScreen] frame];
                CGFloat screenHeight = screenFrame.size.height;
                
                x = static_cast<int32>(frame.origin.x);
                y = static_cast<int32>(screenHeight - frame.origin.y - frame.size.height);
            }
        }

        void setSize(uint32 width, uint32 height) override
        {
            if (window == nullptr)
            {
                return;
            }

            @autoreleasepool
            {
                NSRect frame = [window frame];
                NSSize newSize = NSMakeSize(width, height);
                NSRect newFrame = [window frameRectForContentRect:NSMakeRect(frame.origin.x, frame.origin.y, newSize.width, newSize.height)];
                
                [window setFrame:newFrame display:YES animate:NO];
            }
        }

        void getSize(uint32& width, uint32& height) const override
        {
            if (window == nullptr)
            {
                width = 0;
                height = 0;
                return;
            }

            @autoreleasepool
            {
                NSRect contentRect = [[window contentView] frame];
                width = static_cast<uint32>(contentRect.size.width);
                height = static_cast<uint32>(contentRect.size.height);
            }
        }

        void getSizeInPixels(uint32& width, uint32& height) const override
        {
            if (window == nullptr)
            {
                width = 0;
                height = 0;
                return;
            }

            @autoreleasepool
            {
                NSRect contentRect = [[window contentView] frame];
                CGFloat scale = [window backingScaleFactor];
                
                width = static_cast<uint32>(contentRect.size.width * scale);
                height = static_cast<uint32>(contentRect.size.height * scale);
            }
        }

        void getBordersSize(int32& left, int32& top, int32& right, int32& bottom) const override
        {
            if (window == nullptr)
            {
                left = top = right = bottom = 0;
                return;
            }

            @autoreleasepool
            {
                NSRect frameRect = [window frame];
                NSRect contentRect = [window contentRectForFrameRect:frameRect];
                
                left = static_cast<int32>(contentRect.origin.x - frameRect.origin.x);
                bottom = static_cast<int32>(contentRect.origin.y - frameRect.origin.y);
                right = static_cast<int32>((frameRect.origin.x + frameRect.size.width) - (contentRect.origin.x + contentRect.size.width));
                top = static_cast<int32>((frameRect.origin.y + frameRect.size.height) - (contentRect.origin.y + contentRect.size.height));
            }
        }

        float32 getDisplayScale() const override
        {
            if (window == nullptr)
            {
                return 1.0f;
            }

            @autoreleasepool
            {
                return static_cast<float32>([window backingScaleFactor]);
            }
        }

        void setMinimumSize(uint32 width, uint32 height) override
        {
            if (window == nullptr)
            {
                return;
            }

            @autoreleasepool
            {
                [window setContentMinSize:NSMakeSize(width, height)];
            }
        }

        void getMinimumSize(uint32& width, uint32& height) const override
        {
            if (window == nullptr)
            {
                width = 0;
                height = 0;
                return;
            }

            @autoreleasepool
            {
                NSSize minSize = [window contentMinSize];
                width = static_cast<uint32>(minSize.width);
                height = static_cast<uint32>(minSize.height);
            }
        }

        void setMaximumSize(uint32 width, uint32 height) override
        {
            if (window == nullptr)
            {
                return;
            }

            @autoreleasepool
            {
                [window setContentMaxSize:NSMakeSize(width, height)];
            }
        }

        void getMaximumSize(uint32& width, uint32& height) const override
        {
            if (window == nullptr)
            {
                width = 0;
                height = 0;
                return;
            }

            @autoreleasepool
            {
                NSSize maxSize = [window contentMaxSize];
                width = static_cast<uint32>(maxSize.width);
                height = static_cast<uint32>(maxSize.height);
            }
        }

        void setAspectRatio(uint32 width, uint32 height) override
        {
            if (window == nullptr)
            {
                return;
            }

            @autoreleasepool
            {
                [window setContentAspectRatio:NSMakeSize(width, height)];
            }
        }

        void getAspectRatio(uint32& width, uint32& height) const override
        {
            if (window == nullptr)
            {
                width = 0;
                height = 0;
                return;
            }

            @autoreleasepool
            {
                NSSize aspectRatio = [window contentAspectRatio];
                width = static_cast<uint32>(aspectRatio.width);
                height = static_cast<uint32>(aspectRatio.height);
            }
        }

        void setOpacity(float32 opacity) override
        {
            if (window == nullptr)
            {
                return;
            }

            @autoreleasepool
            {
                [window setAlphaValue:opacity];
                [window setOpaque:(opacity >= 1.0f)];
            }
        }

        float32 getOpacity() const override
        {
            if (window == nullptr)
            {
                return 1.0f;
            }

            @autoreleasepool
            {
                return static_cast<float32>([window alphaValue]);
            }
        }

        void setResizable(bool resizable) override
        {
            if (window == nullptr)
            {
                return;
            }

            @autoreleasepool
            {
                NSUInteger style = [window styleMask];
                
                if (resizable)
                {
                    style |= NSWindowStyleMaskResizable;
                }
                else
                {
                    style &= ~NSWindowStyleMaskResizable;
                }
                
                [window setStyleMask:style];
            }
        }

        void setFocusable(bool focusable) override
        {
            canBecomeKey = focusable;
        }

        void minimize() override
        {
            if (window == nullptr)
            {
                return;
            }

            @autoreleasepool
            {
                [window miniaturize:nil];
            }
        }

        void maximize() override
        {
            if (window == nullptr)
            {
                return;
            }

            @autoreleasepool
            {
                [window zoom:nil];
            }
        }

        void restore() override
        {
            if (window == nullptr)
            {
                return;
            }

            @autoreleasepool
            {
                if ([window isMiniaturized])
                {
                    [window deminiaturize:nil];
                }
                else if ([window isZoomed])
                {
                    [window zoom:nil];
                }
            }
        }

        void raise() override
        {
            if (window == nullptr)
            {
                return;
            }

            @autoreleasepool
            {
                [window makeKeyAndOrderFront:nil];
                [window orderFrontRegardless];
            }
        }

        void flash() override
        {
            @autoreleasepool
            {
                [NSApp requestUserAttention:NSInformationalRequest];
            }
        }

        void setAlwaysOnTop(bool enabled) override
        {
            if (window == nullptr)
            {
                return;
            }

            @autoreleasepool
            {
                [window setLevel:enabled ? NSFloatingWindowLevel : NSNormalWindowLevel];
            }
        }

        void setBordered(bool bordered) override
        {
            if (window == nullptr)
            {
                return;
            }

            @autoreleasepool
            {
                NSUInteger style = [window styleMask];
                
                if (bordered)
                {
                    style |= NSWindowStyleMaskTitled;
                }
                else
                {
                    style &= ~NSWindowStyleMaskTitled;
                }
                
                [window setStyleMask:style];
            }
        }

        void setFullscreen(bool enabled, [[maybe_unused]] Option<Mode> mode) override
        {
            if (window == nullptr)
            {
                return;
            }

            @autoreleasepool
            {
                bool isFullscreen = ([window styleMask] & NSWindowStyleMaskFullScreen) != 0;
                
                if (enabled != isFullscreen)
                {
                    [window toggleFullScreen:nil];
                }
            }
        }

        bool isFullscreen() const override
        {
            if (window == nullptr)
            {
                return false;
            }

            @autoreleasepool
            {
                return ([window styleMask] & NSWindowStyleMaskFullScreen) != 0;
            }
        }

        void setMouseGrab(bool grabbed) override
        {
            mouseGrabbed = grabbed;
            
            if (grabbed)
            {
                CGAssociateMouseAndMouseCursorPosition(NO);
            }
            else
            {
                CGAssociateMouseAndMouseCursorPosition(YES);
            }
        }

        bool getMouseGrab() const override
        {
            return mouseGrabbed;
        }

        void setKeyboardGrab(bool grabbed) override
        {
            keyboardGrabbed = grabbed;
        }

        bool getKeyboardGrab() const override
        {
            return keyboardGrabbed;
        }

        bool isMinimized() const override
        {
            if (window == nullptr)
            {
                return false;
            }

            @autoreleasepool
            {
                return [window isMiniaturized];
            }
        }

        bool isMaximized() const override
        {
            if (window == nullptr)
            {
                return false;
            }

            @autoreleasepool
            {
                return [window isZoomed];
            }
        }

        bool isVisible() const override
        {
            if (window == nullptr)
            {
                return false;
            }

            @autoreleasepool
            {
                return [window isVisible];
            }
        }

        bool hasFocus() const override
        {
            if (window == nullptr)
            {
                return false;
            }

            @autoreleasepool
            {
                return [window isKeyWindow];
            }
        }

        Option<WindowId> getParentId() const override
        {
            return parentWindowId;
        }

        bool isOpen() const override
        {
            return window != nullptr && delegate.windowId != 0;
        }

        void close() override
        {
            if (window == nullptr)
            {
                return;
            }

            @autoreleasepool
            {
                [window performClose:nil];
            }
        }

        void presentSurface(struct Surface const& surface) override
        {
            if (window == nullptr)
            {
                return;
            }
            @autoreleasepool
            {
                NSView* view = [window contentView];
                if ([view isKindOfClass:[ArkContentView class]])
                {
                    ArkContentView* cv = (ArkContentView*)view;
                    [cv updateWithSurface:surface];
                }
            }
        }

        void invalidate() override
        {
            if (window == nullptr)
            {
                return;
            }

            @autoreleasepool
            {
                [[window contentView] setNeedsDisplay:YES];
            }
        }

        // Set input driver event callback for this window
        void setInputDriverEventCallback(void (*callback)(void* event))
        {
            if (window == nullptr)
            {
                return;
            }

            @autoreleasepool
            {
                NSView* view = [window contentView];
                if ([view isKindOfClass:[ArkContentView class]])
                {
                    ArkContentView* cv = (ArkContentView*)view;
                    cv.inputDriverEventCallback = callback;
                }
            }
        }

    private:
        void positionWindow(WindowCreateInfo const& info)
        {
            @autoreleasepool
            {
                if (info.position == WindowPosition::Custom && info.positionX && info.positionY)
                {
                    setPosition(*info.positionX, *info.positionY);
                }
                else if (info.position == WindowPosition::Centered)
                {
                    NSScreen* targetScreen = [NSScreen mainScreen];
                    
                    if (info.display)
                    {
                        for (NSScreen* screen in [NSScreen screens])
                        {
                            NSNumber* screenNumber = [screen.deviceDescription objectForKey:@"NSScreenNumber"];
                            if (screenNumber != nullptr && [screenNumber unsignedIntValue] == *info.display)
                            {
                                targetScreen = screen;
                                break;
                            }
                        }
                    }
                    
                    NSRect visibleFrame = [targetScreen visibleFrame];
                    NSRect windowFrame = [window frame];
                    
                    CGFloat x = visibleFrame.origin.x + (visibleFrame.size.width - windowFrame.size.width) * 0.5f;
                    CGFloat y = visibleFrame.origin.y + (visibleFrame.size.height - windowFrame.size.height) * 0.5f;
                    
                    [window setFrameOrigin:NSMakePoint(x, y)];
                }
            }
        }

    private:
        NSWindow* window = nullptr;
        ArkWindowDelegate* delegate = nullptr;
        Option<WindowId> parentWindowId;
        bool mouseGrabbed = false;
        bool keyboardGrabbed = false;
        bool canBecomeKey = true;
    };

    Window* createWindow(WindowCreateInfo const& info)
    {
        return new MacWindow(info);
    }
}
