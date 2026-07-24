#include "Ark/Testing/Test.hpp"

#include "Ark/Application/Event.hpp"
#include "Ark/Application/EventDispatcher.hpp"

ARK_TEST_CASE("EventDispatcher delivers typed subscriptions", "[application][events]")
{
    Ark::EventDispatcher dispatcher;
    int hits = 0;

    dispatcher.subscribe<Ark::QuitEvent>(
        [&](Ark::QuitEvent const&)
        {
            ++hits;
        });

    dispatcher.enqueueEvent(Ark::QuitEvent{});
    dispatcher.enqueueEvent(Ark::LocaleChangedEvent{});
    dispatcher.dispatchPendingEvents();

    REQUIRE(hits == 1);
}

ARK_TEST_CASE("EventDispatcher uses Display orientation type", "[application][events]")
{
    Ark::EventDispatcher dispatcher;
    Ark::Display::Orientation seen = Ark::Display::Orientation::Unknown;

    dispatcher.subscribe<Ark::DisplayOrientationChangedEvent>(
        [&](Ark::DisplayOrientationChangedEvent const& event)
        {
            seen = event.orientation;
        });

    dispatcher.enqueueEvent(Ark::DisplayOrientationChangedEvent{
        .displayId = 1,
        .orientation = Ark::Display::Orientation::Portrait,
    });
    dispatcher.dispatchPendingEvents();

    REQUIRE(seen == Ark::Display::Orientation::Portrait);
}
