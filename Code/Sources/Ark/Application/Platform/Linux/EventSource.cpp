#if defined(ARK_PLATFORM_LINUX) && !defined(ARK_PLATFORM_WEBASSEMBLY)

#include "Ark/Application/Platform/Linux/EventSource.hpp"

#include "Ark/Display/Platform/Linux/Window.hpp"
#include "Ark/Display/Platform/Linux/X11Context.hpp"
#include "Ark/System/Clipboard.hpp"

#include <sys/select.h>
#include <unistd.h>

namespace Ark::Platform::Linux
{
    void LinuxEventSource::pollEvents(Collections::Array<Event>& outEvents)
    {
        static uint64 lastClipboardSequence = 0;
        uint64 sequence = System::Clipboard::getSequenceNumber();
        if (sequence != 0 && sequence != lastClipboardSequence)
        {
            lastClipboardSequence = sequence;
            outEvents.append(ClipboardUpdatedEvent{});
        }

        Display::Platform::Linux::X11Context& ctx = Display::Platform::Linux::X11Context::get();
        if (!ctx.isValid())
        {
            return;
        }

        while (XPending(ctx.display) > 0)
        {
            XEvent event{};
            XNextEvent(ctx.display, &event);
            Display::Platform::Linux::dispatchXEvent(event);
        }
    }

    void LinuxEventSource::waitEvents(Collections::Array<Event>& outEvents, uint32 timeoutMs)
    {
        Display::Platform::Linux::X11Context& ctx = Display::Platform::Linux::X11Context::get();
        if (!ctx.isValid())
        {
            return;
        }

        if (XPending(ctx.display) > 0)
        {
            pollEvents(outEvents);
            return;
        }

        int fd = ConnectionNumber(ctx.display);
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(fd, &fds);

        timeval tv{};
        timeval* tvPtr = nullptr;
        if (timeoutMs != 0)
        {
            tv.tv_sec = static_cast<time_t>(timeoutMs / 1000u);
            tv.tv_usec = static_cast<suseconds_t>((timeoutMs % 1000u) * 1000u);
            tvPtr = &tv;
        }

        int result = select(fd + 1, &fds, nullptr, nullptr, tvPtr);
        if (result > 0)
        {
            pollEvents(outEvents);
        }
    }
}

#endif
