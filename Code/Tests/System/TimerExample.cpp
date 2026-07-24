#include "Ark/Testing/Test.hpp"

#include "Ark/System/Time.hpp"
#include "Ark/System/Timer.hpp"

#include <atomic>

using Ark::System::Time::delay;

ARK_TEST_CASE("Timer example - repeating timer cancels via callback", "[timer]")
{
    std::atomic<int> ticks{0};

    Ark::System::Timer::TimerId id = Ark::System::Timer::addTimer(50, [&](Ark::uint32) -> Ark::uint32
                                                                  {
                                                                      int count = ++ticks;
                                                                      return (count < 3) ? 50 : 0; // repeat twice, then cancel
                                                                  });

    REQUIRE(id != 0);

    for (int i = 0; i < 20 && ticks.load() < 3; ++i)
    {
        delay(20);
    }

    REQUIRE(ticks.load() == 3);
}

ARK_TEST_CASE("Timer example - removeTimer cancels pending single-shot", "[timer]")
{
    std::atomic<bool> fired{false};

    Ark::System::Timer::TimerId id = Ark::System::Timer::addTimer(200, [&](Ark::uint32) -> Ark::uint32
                                                                  {
                                                                      fired.store(true);
                                                                      return 0; // single-shot
                                                                  });

    REQUIRE(id != 0);

    bool removed = Ark::System::Timer::removeTimer(id);
    REQUIRE(removed);

    delay(250);
    REQUIRE(fired.load() == false);
}
