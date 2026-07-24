#if defined(ARK_PLATFORM_WINDOWS)

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <shellapi.h>

#include "Ark/Application/Event.hpp"
#include "Ark/Application/Platform/Windows/EventSource.hpp"

namespace Ark::Platform::Windows
{
    WindowsEventSource::WindowsEventSource()
        : lastClipboardSequence(0)
        , isMonitoringTheme(false)
        , isMonitoringClipboard(false)
    {
        if (OpenClipboard(nullptr))
        {
            lastClipboardSequence = GetClipboardSequenceNumber();
            CloseClipboard();
        }
    }

    WindowsEventSource::~WindowsEventSource() = default;

    void WindowsEventSource::pollEvents(Collections::Array<Ark::Event>& outEvents)
    {
        if (OpenClipboard(nullptr))
        {
            DWORD currentSequence = GetClipboardSequenceNumber();
            if (currentSequence != lastClipboardSequence)
            {
                lastClipboardSequence = currentSequence;
                outEvents.append(Ark::ClipboardUpdatedEvent{});
            }
            CloseClipboard();
        }

        MSG msg;
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
            {
                outEvents.append(Ark::QuitEvent{});
            }

            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    void WindowsEventSource::waitEvents(Collections::Array<Ark::Event>& outEvents, uint32 timeoutMs)
    {
        DWORD result = MsgWaitForMultipleObjects(0, nullptr, FALSE, timeoutMs == 0 ? INFINITE : timeoutMs, QS_ALLEVENTS);

        if (result == WAIT_OBJECT_0)
        {
            pollEvents(outEvents);
        }
    }
}

#endif
