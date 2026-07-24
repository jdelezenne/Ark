#include "Ark/Application/Application.hpp"
#include "Ark/Application/Event.hpp"
#include "Ark/Application/EventLoop.hpp"
#include "Ark/Display/Window.hpp"
#include "Ark/Logging/FormattedConsoleTarget.hpp"
#include "Ark/Logging/Logger.hpp"
#include "Ark/Logging/Macros.hpp"
#include "Ark/Logging/PatternFormatter.hpp"
#include "Ark/Logging/Service.hpp"
#include "Ark/System/EntryPoint.hpp"
#include "Ark/System/Formatters.hpp"

using namespace Ark;

ARK_LOG_DECLARE_CATEGORY(Events);
ARK_LOG_DEFINE_CATEGORY_LEVEL(Events, Debug)

bool arkMain(Ark::Collections::Array<Ark::String> const&)
{
    auto* logService = Logging::Service::createService();
    auto* logger = logService->addLogger("Events");
    logger->setFormatter(makeShared<Logging::PatternFormatter>("[%t] %l: %m"));
    logger->addTarget(makeShared<Logging::FormattedConsoleTarget>());

    EventLoop eventLoop;

    Display::WindowCreateInfo info{
        .title = "Ark Event System Demo - Watch the console!",
        .width = 1024,
        .height = 768,
        .position = Display::WindowPosition::Centered,
        .flags = Display::WindowFlags::Resizable | Display::WindowFlags::AcceptTouchEvents,
        .eventDispatcher = &eventLoop.getDispatcher(),
    };

    auto window = UniquePointer<Display::Window>(Display::Window::create(info));
    if (window == nullptr)
    {
        ARK_LOG_ERROR_FORMAT(Events, "Failed to create window");
        return false;
    }

    window->show();

    bool shouldQuit = false;
    Display::WindowId windowId = window->getId();

    ARK_LOG_INFO_FORMAT(Events, "Event system initialized. Try:");
    ARK_LOG_INFO_FORMAT(Events, "  - Resize/move/minimize/maximize the window");
    ARK_LOG_INFO_FORMAT(Events, "  - Type text (regular or with IME for Chinese/Japanese)");
    ARK_LOG_INFO_FORMAT(Events, "  - Click mouse buttons and move the mouse");
    ARK_LOG_INFO_FORMAT(Events, "  - Scroll the mouse wheel");
    ARK_LOG_INFO_FORMAT(Events, "  - Use trackpad gestures (pinch, rotate)");
    ARK_LOG_INFO_FORMAT(Events, "  - Place fingers on trackpad (raw touch events)");
    ARK_LOG_INFO_FORMAT(Events, "  - Copy/paste text (clipboard events)");
    ARK_LOG_INFO_FORMAT(Events, "  - Change system theme to Dark/Light mode");
    ARK_LOG_INFO_FORMAT(Events, "  - Press Cmd+Q to see quit event");

    eventLoop.getDispatcher().subscribe<WindowCloseRequestedEvent>(
        [&](WindowCloseRequestedEvent const& event)
        {
            if (event.windowId == windowId)
            {
                ARK_LOG_INFO_FORMAT(Events, "WindowCloseRequested");
                shouldQuit = true;
            }
        });

    eventLoop.getDispatcher().subscribe<WindowClosedEvent>(
        [&](WindowClosedEvent const& event)
        {
            if (event.windowId == windowId)
            {
                ARK_LOG_INFO_FORMAT(Events, "WindowClosed");
            }
        });

    eventLoop.getDispatcher().subscribe<WindowShownEvent>(
        [](WindowShownEvent const& event)
        {
            ARK_LOG_INFO_FORMAT(Events, "WindowShown");
        });

    eventLoop.getDispatcher().subscribe<WindowHiddenEvent>(
        [](WindowHiddenEvent const& event)
        {
            ARK_LOG_INFO_FORMAT(Events, "WindowHidden");
        });

    eventLoop.getDispatcher().subscribe<WindowResizedEvent>(
        [](WindowResizedEvent const& event)
        {
            ARK_LOG_INFO_FORMAT(Events, "WindowResized: {}x{}", event.width, event.height);
        });

    eventLoop.getDispatcher().subscribe<WindowPixelSizeChangedEvent>(
        [](WindowPixelSizeChangedEvent const& event)
        {
            ARK_LOG_DEBUG_FORMAT(Events, "WindowPixelSizeChanged: {}x{}", event.width, event.height);
        });

    eventLoop.getDispatcher().subscribe<WindowMovedEvent>(
        [](WindowMovedEvent const& event)
        {
            ARK_LOG_INFO_FORMAT(Events, "WindowMoved: ({}, {})", event.x, event.y);
        });

    eventLoop.getDispatcher().subscribe<WindowFocusGainedEvent>(
        [](WindowFocusGainedEvent const& event)
        {
            ARK_LOG_INFO_FORMAT(Events, "WindowFocusGained");
        });

    eventLoop.getDispatcher().subscribe<WindowFocusLostEvent>(
        [](WindowFocusLostEvent const& event)
        {
            ARK_LOG_INFO_FORMAT(Events, "WindowFocusLost");
        });

    eventLoop.getDispatcher().subscribe<WindowMinimizedEvent>(
        [](WindowMinimizedEvent const& event)
        {
            ARK_LOG_INFO_FORMAT(Events, "WindowMinimized");
        });

    eventLoop.getDispatcher().subscribe<WindowRestoredEvent>(
        [](WindowRestoredEvent const& event)
        {
            ARK_LOG_INFO_FORMAT(Events, "WindowRestored");
        });

    eventLoop.getDispatcher().subscribe<WindowMaximizedEvent>(
        [](WindowMaximizedEvent const& event)
        {
            ARK_LOG_INFO_FORMAT(Events, "WindowMaximized");
        });

    eventLoop.getDispatcher().subscribe<WindowEnteredFullscreenEvent>(
        [](WindowEnteredFullscreenEvent const& event)
        {
            ARK_LOG_INFO_FORMAT(Events, "WindowEnteredFullscreen");
        });

    eventLoop.getDispatcher().subscribe<WindowLeftFullscreenEvent>(
        [](WindowLeftFullscreenEvent const& event)
        {
            ARK_LOG_INFO_FORMAT(Events, "WindowLeftFullscreen");
        });

    eventLoop.getDispatcher().subscribe<WindowDisplayChangedEvent>(
        [](WindowDisplayChangedEvent const& event)
        {
            ARK_LOG_INFO_FORMAT(Events, "WindowDisplayChanged: display={}", event.displayId);
        });

    eventLoop.getDispatcher().subscribe<WindowDisplayScaleChangedEvent>(
        [](WindowDisplayScaleChangedEvent const& event)
        {
            ARK_LOG_INFO_FORMAT(Events, "WindowDisplayScaleChanged: scale={}", event.scale);
        });

    eventLoop.getDispatcher().subscribe<WindowExposedEvent>(
        [](WindowExposedEvent const& event)
        {
            ARK_LOG_DEBUG_FORMAT(Events, "WindowExposed");
        });

    eventLoop.getDispatcher().subscribe<WindowOccludedEvent>(
        [](WindowOccludedEvent const& event)
        {
            ARK_LOG_DEBUG_FORMAT(Events, "WindowOccluded");
        });

    eventLoop.getDispatcher().subscribe<WindowDestroyedEvent>(
        [](WindowDestroyedEvent const& event)
        {
            ARK_LOG_INFO_FORMAT(Events, "WindowDestroyed");
        });

    eventLoop.getDispatcher().subscribe<KeyDownEvent>(
        [](KeyDownEvent const& event)
        {
            ARK_LOG_INFO_FORMAT(Events, "KeyDown: scancode={}, repeat={}", static_cast<uint32>(event.scancode), event.repeat);
        });

    eventLoop.getDispatcher().subscribe<KeyUpEvent>(
        [](KeyUpEvent const& event)
        {
            ARK_LOG_INFO_FORMAT(Events, "KeyUp: scancode={}", static_cast<uint32>(event.scancode));
        });

    eventLoop.getDispatcher().subscribe<TextInputEvent>(
        [](TextInputEvent const& event)
        {
            ARK_LOG_INFO_FORMAT(Events, "TextInput: '{}'", event.text);
        });

    eventLoop.getDispatcher().subscribe<TextEditingEvent>(
        [](TextEditingEvent const& event)
        {
            ARK_LOG_INFO_FORMAT(Events, "TextEditing (IME): '{}', range=({}, {})", event.text, event.start, event.length);
        });

    eventLoop.getDispatcher().subscribe<QuitEvent>(
        [](QuitEvent const& event)
        {
            ARK_LOG_INFO_FORMAT(Events, "Quit: Application is terminating");
        });

    eventLoop.getDispatcher().subscribe<LocaleChangedEvent>(
        [](LocaleChangedEvent const& event)
        {
            ARK_LOG_INFO_FORMAT(Events, "LocaleChanged: System locale was changed");
        });

    eventLoop.getDispatcher().subscribe<ThemeChangedEvent>(
        [](ThemeChangedEvent const& event)
        {
            ARK_LOG_INFO_FORMAT(Events, "ThemeChanged: System theme was changed (light/dark mode)");
        });

    eventLoop.getDispatcher().subscribe<ClipboardUpdatedEvent>(
        [](ClipboardUpdatedEvent const& event)
        {
            ARK_LOG_INFO_FORMAT(Events, "ClipboardUpdated: Clipboard contents changed");
        });

    eventLoop.getDispatcher().subscribe<MouseMotionEvent>(
        [](MouseMotionEvent const& event)
        {
            ARK_LOG_DEBUG_FORMAT(Events, "MouseMotion: ({}, {}), delta=({}, {})", event.x, event.y, event.dx, event.dy);
        });

    eventLoop.getDispatcher().subscribe<MouseButtonDownEvent>(
        [](MouseButtonDownEvent const& event)
        {
            ARK_LOG_INFO_FORMAT(Events, "MouseButtonDown: button={}, clicks={}, position=({}, {})", event.button, event.clicks, event.x, event.y);
        });

    eventLoop.getDispatcher().subscribe<MouseButtonUpEvent>(
        [](MouseButtonUpEvent const& event)
        {
            ARK_LOG_INFO_FORMAT(Events, "MouseButtonUp: button={}, position=({}, {})", event.button, event.x, event.y);
        });

    eventLoop.getDispatcher().subscribe<MouseWheelEvent>(
        [](MouseWheelEvent const& event)
        {
            ARK_LOG_INFO_FORMAT(Events, "MouseWheel: delta=({}, {})", event.x, event.y);
        });

    eventLoop.getDispatcher().subscribe<GestureMagnifyEvent>(
        [](GestureMagnifyEvent const& event)
        {
            ARK_LOG_INFO_FORMAT(Events, "GestureMagnify: magnitude={}", event.magnitude);
        });

    eventLoop.getDispatcher().subscribe<GestureRotateEvent>(
        [](GestureRotateEvent const& event)
        {
            ARK_LOG_INFO_FORMAT(Events, "GestureRotate: rotation={} degrees", event.rotation);
        });

    eventLoop.getDispatcher().subscribe<TouchBeganEvent>(
        [](TouchBeganEvent const& event)
        {
            ARK_LOG_INFO_FORMAT(Events, "TouchBegan: {} touch(es)", event.touches.getCount());
            for (auto const& touch : event.touches)
            {
                ARK_LOG_DEBUG_FORMAT(Events, "  Touch ID={}, position=({:.2f}, {:.2f}), normalized=({:.3f}, {:.3f})", touch.id, touch.x, touch.y, touch.normalizedX, touch.normalizedY);
            }
        });

    eventLoop.getDispatcher().subscribe<TouchMovedEvent>(
        [](TouchMovedEvent const& event)
        {
            ARK_LOG_DEBUG_FORMAT(Events, "TouchMoved: {} touch(es)", event.touches.getCount());
        });

    eventLoop.getDispatcher().subscribe<TouchEndedEvent>(
        [](TouchEndedEvent const& event)
        {
            ARK_LOG_INFO_FORMAT(Events, "TouchEnded: {} touch(es)", event.touches.getCount());
        });

    eventLoop.getDispatcher().subscribe<TouchCancelledEvent>(
        [](TouchCancelledEvent const& event)
        {
            ARK_LOG_INFO_FORMAT(Events, "TouchCancelled: {} touch(es)", event.touches.getCount());
        });

    eventLoop.getDispatcher().subscribe<WindowMouseEnterEvent>(
        [](WindowMouseEnterEvent const& event)
        {
            ARK_LOG_DEBUG_FORMAT(Events, "WindowMouseEnter");
        });

    eventLoop.getDispatcher().subscribe<WindowMouseLeaveEvent>(
        [](WindowMouseLeaveEvent const& event)
        {
            ARK_LOG_DEBUG_FORMAT(Events, "WindowMouseLeave");
        });

    ARK_LOG_INFO_FORMAT(Events, "Starting event loop...");

    while (!shouldQuit && window->isOpen())
    {
        eventLoop.pollEvents();
    }

    ARK_LOG_INFO_FORMAT(Events, "Application shutting down");
    logService->flushAll();
    logService->removeAll();

    return true;
}
