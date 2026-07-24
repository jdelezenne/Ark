#include "Ark/Strings/String.hpp"
#include "Ark/System/Console.hpp"
#include "Ark/System/EntryPoint.hpp"
#include "Ark/System/Time.hpp"
#include "Ark/System/Timer.hpp"

#include <atomic>

bool arkMain(Ark::Collections::Array<Ark::String> const&)
{
    using Ark::System::Time::delay;

    std::atomic<int> ticks{0};

    auto id = Ark::System::Timer::addTimer(
        100, [&](Ark::uint32) -> Ark::uint32
        {
            int n = ++ticks;
            Ark::String msg = Ark::String::format("[Timer] tick {}", n);
            Ark::System::Console::println(msg);
            return (n < 3) ? 100 : 0; // repeat twice, then cancel
        });

    if (id == 0)
    {
        Ark::System::Console::println("[Timer] failed to schedule");
        return false;
    }
    {
        Ark::String msg = Ark::String::format("[Timer] scheduled id={}", id);
        Ark::System::Console::println(msg);
    }

    while (ticks.load() < 3)
    {
        delay(20);
    }

    {
        Ark::String msg = Ark::String::format("[Timer] done ticks={}", ticks.load());
        Ark::System::Console::println(msg);
    }
    return true;
}
