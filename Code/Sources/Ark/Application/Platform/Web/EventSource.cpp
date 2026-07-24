#if defined(ARK_PLATFORM_WEBASSEMBLY)

#include "Ark/Application/Platform/Web/EventSource.hpp"

#include <emscripten/html5.h>

namespace Ark::Platform::Web
{
    namespace
    {
        static Collections::Array<Event> gQueuedEvents;
        static bool gCallbacksRegistered = false;

        static EM_BOOL onMouseEvent(int eventType, EmscriptenMouseEvent const* event, void* /*userData*/)
        {
            if (event == nullptr)
            {
                return EM_FALSE;
            }

            Display::WindowId const windowId = 1;
            switch (eventType)
            {
                case EMSCRIPTEN_EVENT_MOUSEMOVE:
                    gQueuedEvents.append(MouseMotionEvent{
                        .windowId = windowId,
                        .x = static_cast<int32>(event->targetX),
                        .y = static_cast<int32>(event->targetY),
                        .dx = static_cast<int32>(event->movementX),
                        .dy = static_cast<int32>(event->movementY),
                    });
                    return EM_TRUE;
                case EMSCRIPTEN_EVENT_MOUSEDOWN:
                    gQueuedEvents.append(MouseButtonDownEvent{
                        .windowId = windowId,
                        .button = (event->button == 2) ? MouseButton::Right : ((event->button == 1) ? MouseButton::Middle : MouseButton::Left),
                        .x = static_cast<int32>(event->targetX),
                        .y = static_cast<int32>(event->targetY),
                    });
                    return EM_TRUE;
                case EMSCRIPTEN_EVENT_MOUSEUP:
                    gQueuedEvents.append(MouseButtonUpEvent{
                        .windowId = windowId,
                        .button = (event->button == 2) ? MouseButton::Right : ((event->button == 1) ? MouseButton::Middle : MouseButton::Left),
                        .x = static_cast<int32>(event->targetX),
                        .y = static_cast<int32>(event->targetY),
                    });
                    return EM_TRUE;
                default:
                    return EM_FALSE;
            }
        }

        static EM_BOOL onWheelEvent(int /*eventType*/, EmscriptenWheelEvent const* event, void* /*userData*/)
        {
            if (event == nullptr)
            {
                return EM_FALSE;
            }
            gQueuedEvents.append(MouseWheelEvent{
                .windowId = 1,
                .x = static_cast<float32>(event->deltaX),
                .y = static_cast<float32>(-event->deltaY),
            });
            return EM_TRUE;
        }

        static EM_BOOL onKeyEvent(int eventType, EmscriptenKeyboardEvent const* event, void* /*userData*/)
        {
            if (event == nullptr)
            {
                return EM_FALSE;
            }

            Key key = Key::Unknown;
            if (event->key[0] != '\0' && event->key[1] == '\0')
            {
                char ch = event->key[0];
                if (ch >= 'a' && ch <= 'z')
                {
                    key = static_cast<Key>(static_cast<uint32>(Key::A) + static_cast<uint32>(ch - 'a'));
                }
                else if (ch >= 'A' && ch <= 'Z')
                {
                    key = static_cast<Key>(static_cast<uint32>(Key::A) + static_cast<uint32>(ch - 'A'));
                }
                else if (ch >= '0' && ch <= '9')
                {
                    key = static_cast<Key>(static_cast<uint32>(Key::Digit0) + static_cast<uint32>(ch - '0'));
                }
            }

            KeyModifiers modifiers = KeyModifiers::None;
            if (event->shiftKey)
            {
                modifiers |= KeyModifiers::Shift;
            }
            if (event->ctrlKey)
            {
                modifiers |= KeyModifiers::Control;
            }
            if (event->altKey)
            {
                modifiers |= KeyModifiers::Alt;
            }
            if (event->metaKey)
            {
                modifiers |= KeyModifiers::Cmd;
            }

            if (eventType == EMSCRIPTEN_EVENT_KEYDOWN)
            {
                gQueuedEvents.append(KeyDownEvent{
                    .windowId = 1,
                    .key = key,
                    .modifiers = modifiers,
                    .repeat = event->repeat != 0,
                });
            }
            else if (eventType == EMSCRIPTEN_EVENT_KEYUP)
            {
                gQueuedEvents.append(KeyUpEvent{
                    .windowId = 1,
                    .key = key,
                    .modifiers = modifiers,
                });
            }
            return EM_TRUE;
        }

        static void ensureCallbacks()
        {
            if (gCallbacksRegistered)
            {
                return;
            }
            gCallbacksRegistered = true;

            emscripten_set_mousemove_callback("#canvas", nullptr, EM_TRUE, onMouseEvent);
            emscripten_set_mousedown_callback("#canvas", nullptr, EM_TRUE, onMouseEvent);
            emscripten_set_mouseup_callback("#canvas", nullptr, EM_TRUE, onMouseEvent);
            emscripten_set_wheel_callback("#canvas", nullptr, EM_TRUE, onWheelEvent);
            emscripten_set_keydown_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr, EM_TRUE, onKeyEvent);
            emscripten_set_keyup_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr, EM_TRUE, onKeyEvent);
        }

        static void drainQueue(Collections::Array<Event>& outEvents)
        {
            for (usize i = 0; i < gQueuedEvents.getCount(); ++i)
            {
                outEvents.append(gQueuedEvents[i]);
            }
            gQueuedEvents.clear();
        }
    }

    void WebEventSource::pollEvents(Collections::Array<Event>& outEvents)
    {
        ensureCallbacks();
        drainQueue(outEvents);
    }

    void WebEventSource::waitEvents(Collections::Array<Event>& outEvents, uint32 /*timeoutMs*/)
    {
        // Browser main loop delivers input asynchronously; drain whatever is queued.
        pollEvents(outEvents);
    }
}

#endif
