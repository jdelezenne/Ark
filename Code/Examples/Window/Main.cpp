#include "Ark/Application/Application.hpp"
#include "Ark/Application/Event.hpp"
#include "Ark/Application/EventLoop.hpp"
#include "Ark/Display/Window.hpp"
#include "Ark/System/EntryPoint.hpp"

bool arkMain(Ark::Collections::Array<Ark::String> const&)
{
    Ark::EventLoop eventLoop;

    Ark::Display::WindowCreateInfo info{
        .title = "Ark Window",
        .width = 800,
        .height = 600,
        .position = Ark::Display::WindowPosition::Centered,
        .flags = Ark::Display::WindowFlags::Resizable,
        .eventDispatcher = &eventLoop.getDispatcher(),
    };

    auto window = Ark::UniquePointer<Ark::Display::Window>(Ark::Display::Window::create(info));
    if (window == nullptr)
    {
        return false;
    }

    window->show();

    bool shouldQuit = false;
    Ark::Display::WindowId windowId = window->getId();

    eventLoop.getDispatcher().subscribe<Ark::WindowCloseRequestedEvent>(
        [&](Ark::WindowCloseRequestedEvent const& event)
        {
            if (event.windowId == windowId)
            {
                shouldQuit = true;
            }
        });

    while (!shouldQuit && window->isOpen())
    {
        eventLoop.pollEvents();
    }

    return true;
}
