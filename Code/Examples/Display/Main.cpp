#include "Ark/Display/Monitor.hpp"
#include "Ark/Strings/String.hpp"
#include "Ark/System/Console.hpp"
#include "Ark/System/EntryPoint.hpp"

using namespace Ark;

bool arkMain(Ark::Collections::Array<Ark::String> const&)
{
    using Ark::System::Console;

    auto monitorIds = Display::Monitors::enumerate();
    Console::println("Monitors detected:");

    auto primary = Display::Monitors::getPrimary();
    if (primary)
    {
        Console::printlnFormat("  Primary: {}", static_cast<unsigned int>(*primary));
    }

    for (usize i = 0; i < monitorIds.getCount(); ++i)
    {
        auto id = monitorIds[i];
        auto name = Display::Monitors::getName(id);
        auto info = Display::Monitors::getInfo(id);

        Console::printlnFormat("- Monitor {}: {}", static_cast<unsigned int>(id), name);
        Console::printlnFormat("    Bounds: {}x{} at {},{}", static_cast<int>(info.bounds.width), static_cast<int>(info.bounds.height), static_cast<int>(info.bounds.x), static_cast<int>(info.bounds.y));
        Console::printlnFormat("    Usable: {}x{} at {},{}", static_cast<int>(info.usableBounds.width), static_cast<int>(info.usableBounds.height), static_cast<int>(info.usableBounds.x), static_cast<int>(info.usableBounds.y));
        Console::printlnFormat("    Scale: {}", info.contentScale);

        auto desktop = Display::Monitors::getDesktopMode(id);
        if (desktop)
        {
            Console::printlnFormat("    Desktop Mode: {}x{} @ {} Hz", static_cast<unsigned int>(desktop->width), static_cast<unsigned int>(desktop->height), desktop->refreshRate);
        }

        auto modes = Display::Monitors::getFullscreenModes(id);
        Console::printlnFormat("    Modes (up to 10): count={}", static_cast<unsigned int>(modes.getCount()));
        for (usize mi = 0; mi < modes.getCount() && mi < 10; ++mi)
        {
            auto const& m = modes[mi];
            Console::printlnFormat("      - {}x{} @ {} Hz", static_cast<unsigned int>(m.width), static_cast<unsigned int>(m.height), m.refreshRate);
        }
    }

    return true;
}
