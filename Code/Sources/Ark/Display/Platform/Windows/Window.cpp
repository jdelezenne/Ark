#include "Ark/Display/Platform/Windows/Window.hpp"
#include "Ark/Application/Event.hpp"
#include "Ark/Application/EventDispatcher.hpp"
#include "Ark/Core/Platform/Windows/Platform.hpp"
#include "Ark/Display/Surface.hpp"
#include "Ark/Strings/Unicode.hpp"
#include "Ark/System/Mouse.hpp"
#include "Ark/System/Platform/Windows/KeyMapping.hpp"

#include <cstring>
#include <imm.h>
#include <shellapi.h>
#include <windowsx.h>

#pragma comment(lib, "imm32.lib")

namespace Ark::Display::Platform::Windows
{
    class WinWindow;

    // Forward declaration for paint handler
    static void HandleWindowPaint(WinWindow* window, HDC hdc, PAINTSTRUCT const& ps);

    static LRESULT CALLBACK ArkWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        WinWindow* self = reinterpret_cast<WinWindow*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
        Ark::EventDispatcher* dispatcher = (self != nullptr) ? self->eventDispatcher : nullptr;

        if (self == nullptr && msg != WM_NCCREATE)
        {
            return DefWindowProcW(hwnd, msg, wParam, lParam);
        }

        // Get window ID directly from hwnd (getId() does: static_cast<WindowId>(reinterpret_cast<UIntPtr>(hwnd)))
        WindowId windowId = static_cast<WindowId>(reinterpret_cast<UIntPtr>(hwnd));

        switch (msg)
        {
            case WM_NCCREATE:
            {
                CREATESTRUCTW const* create = reinterpret_cast<CREATESTRUCTW const*>(lParam);
                SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(create->lpCreateParams));
                break;
            }

            case WM_CLOSE:
            {
                if (dispatcher != nullptr)
                {
                    dispatcher->enqueueEvent(Ark::WindowCloseRequestedEvent{.windowId = windowId});
                }
                return 0;
            }

            case WM_DESTROY:
            {
                if (dispatcher != nullptr)
                {
                    dispatcher->enqueueEvent(Ark::WindowDestroyedEvent{.windowId = windowId});
                    dispatcher->enqueueEvent(Ark::WindowClosedEvent{.windowId = windowId});
                }
                break;
            }

            case WM_SIZE:
            {
                UINT width = LOWORD(lParam);
                UINT height = HIWORD(lParam);

                if (dispatcher != nullptr)
                {
                    dispatcher->enqueueEvent(Ark::WindowResizedEvent{
                        .windowId = windowId,
                        .width = width,
                        .height = height,
                    });

                    dispatcher->enqueueEvent(Ark::WindowPixelSizeChangedEvent{
                        .windowId = windowId,
                        .width = width,
                        .height = height,
                    });

                    if (wParam == SIZE_MINIMIZED)
                    {
                        dispatcher->enqueueEvent(Ark::WindowMinimizedEvent{.windowId = windowId});
                    }
                    else if (wParam == SIZE_MAXIMIZED)
                    {
                        dispatcher->enqueueEvent(Ark::WindowMaximizedEvent{.windowId = windowId});
                    }
                    else if (wParam == SIZE_RESTORED)
                    {
                        dispatcher->enqueueEvent(Ark::WindowRestoredEvent{.windowId = windowId});
                    }
                }
                break;
            }

            case WM_MOVE:
            {
                int x = static_cast<short>(LOWORD(lParam));
                int y = static_cast<short>(HIWORD(lParam));

                if (dispatcher != nullptr)
                {
                    dispatcher->enqueueEvent(Ark::WindowMovedEvent{
                        .windowId = windowId,
                        .x = x,
                        .y = y,
                    });
                }
                break;
            }

            case WM_SHOWWINDOW:
            {
                if (dispatcher != nullptr)
                {
                    if (wParam)
                    {
                        dispatcher->enqueueEvent(Ark::WindowShownEvent{.windowId = windowId});
                    }
                    else
                    {
                        dispatcher->enqueueEvent(Ark::WindowHiddenEvent{.windowId = windowId});
                    }
                }
                break;
            }

            case WM_PAINT:
            {
                if (self != nullptr)
                {
                    PAINTSTRUCT ps;
                    HDC hdc = BeginPaint(hwnd, &ps);
                    if (hdc != NULL)
                    {
                        HandleWindowPaint(self, hdc, ps);
                        EndPaint(hwnd, &ps);
                    }
                }

                if (dispatcher != nullptr)
                {
                    dispatcher->enqueueEvent(Ark::WindowExposedEvent{.windowId = windowId});
                }
                return 0;
            }

            case WM_SETFOCUS:
            {
                if (dispatcher != nullptr)
                {
                    dispatcher->enqueueEvent(Ark::WindowFocusGainedEvent{.windowId = windowId});
                }
                break;
            }

            case WM_KILLFOCUS:
            {
                if (dispatcher != nullptr)
                {
                    dispatcher->enqueueEvent(Ark::WindowFocusLostEvent{.windowId = windowId});
                }
                break;
            }

            case WM_ENTERSIZEMOVE:
            case WM_ENTERMENULOOP:
            {
                break;
            }

            case WM_EXITSIZEMOVE:
            case WM_EXITMENULOOP:
            {
                break;
            }

            case WM_DISPLAYCHANGE:
            {
                if (dispatcher != nullptr)
                {
                    dispatcher->enqueueEvent(Ark::DisplayModeChangedEvent{.displayId = 0});
                }
                break;
            }

            case WM_DPICHANGED:
            {
                if (dispatcher != nullptr)
                {
                    UINT dpiX = HIWORD(wParam);
                    float scale = static_cast<float>(dpiX) / 96.0f;
                    dispatcher->enqueueEvent(Ark::WindowDisplayScaleChangedEvent{.windowId = windowId, .scale = scale});
                }
                break;
            }

            case WM_KEYDOWN:
            case WM_SYSKEYDOWN:
            {
                if (dispatcher != nullptr)
                {
                    bool isRepeat = (lParam & 0x40000000) != 0;
                    int scancode = (lParam >> 16) & 0xFF;

                    Key key = Ark::Platform::Windows::mapVirtualKeyToKey(static_cast<unsigned int>(wParam));

                    dispatcher->enqueueEvent(Ark::KeyDownEvent{
                        .windowId = windowId,
                        .keyboardId = 0,
                        .scancode = static_cast<Scancode>(scancode),
                        .key = key,
                        .modifiers = KeyModifiers::None,
                        .repeat = isRepeat,
                    });
                }
                break;
            }

            case WM_KEYUP:
            case WM_SYSKEYUP:
            {
                if (dispatcher != nullptr)
                {
                    int scancode = (lParam >> 16) & 0xFF;

                    Key key = Ark::Platform::Windows::mapVirtualKeyToKey(static_cast<unsigned int>(wParam));

                    dispatcher->enqueueEvent(Ark::KeyUpEvent{
                        .windowId = windowId,
                        .keyboardId = 0,
                        .scancode = static_cast<Scancode>(scancode),
                        .key = key,
                        .modifiers = KeyModifiers::None,
                    });
                }
                break;
            }

            case WM_CHAR:
            {
                if (dispatcher != nullptr && wParam > 0 && wParam < 0x10000)
                {
                    wchar_t wch[2] = {static_cast<wchar_t>(wParam), L'\0'};
                    Ark::String text = Ark::Unicode::fromWide(wch);

                    if (!text.isEmpty())
                    {
                        dispatcher->enqueueEvent(Ark::TextInputEvent{
                            .windowId = windowId,
                            .text = Ark::move(text),
                        });
                    }
                }
                break;
            }

            case WM_IME_COMPOSITION:
            {
                if (dispatcher != nullptr && (lParam & GCS_COMPSTR))
                {
                    HIMC imc = ImmGetContext(hwnd);
                    if (imc)
                    {
                        LONG len = ImmGetCompositionStringW(imc, GCS_COMPSTR, nullptr, 0);
                        if (len > 0)
                        {
                            wchar_t* wstr = new wchar_t[len / sizeof(wchar_t) + 1];
                            ImmGetCompositionStringW(imc, GCS_COMPSTR, wstr, len);
                            wstr[len / sizeof(wchar_t)] = 0;

                            Ark::String text = Ark::Unicode::fromWide(wstr);
                            LONG cursorPos = ImmGetCompositionStringW(imc, GCS_CURSORPOS, nullptr, 0);

                            dispatcher->enqueueEvent(Ark::TextEditingEvent{
                                .windowId = windowId,
                                .text = Ark::move(text),
                                .start = static_cast<int32>(cursorPos),
                                .length = static_cast<int32>(len / sizeof(wchar_t)),
                            });

                            delete[] wstr;
                        }
                        ImmReleaseContext(hwnd, imc);
                    }
                }
                break;
            }

            case WM_MOUSEMOVE:
            {
                if (dispatcher != nullptr)
                {
                    int x = GET_X_LPARAM(lParam);
                    int y = GET_Y_LPARAM(lParam);

                    static int lastX = x;
                    static int lastY = y;

                    dispatcher->enqueueEvent(Ark::MouseMotionEvent{
                        .windowId = windowId,
                        .mouseId = 0,
                        .x = x,
                        .y = y,
                        .dx = x - lastX,
                        .dy = y - lastY,
                        .state = MouseButtons::None,
                    });

                    lastX = x;
                    lastY = y;
                }
                break;
            }

            case WM_LBUTTONDOWN:
            case WM_MBUTTONDOWN:
            case WM_RBUTTONDOWN:
            case WM_XBUTTONDOWN:
            {
                if (dispatcher != nullptr)
                {
                    MouseButton button = MouseButton::Left;
                    if (msg == WM_LBUTTONDOWN)
                        button = MouseButton::Left;
                    else if (msg == WM_MBUTTONDOWN)
                        button = MouseButton::Middle;
                    else if (msg == WM_RBUTTONDOWN)
                        button = MouseButton::Right;
                    else if (msg == WM_XBUTTONDOWN)
                        button = static_cast<MouseButton>(static_cast<uint32>(MouseButton::Button4) + GET_XBUTTON_WPARAM(wParam) - 1);

                    int x = GET_X_LPARAM(lParam);
                    int y = GET_Y_LPARAM(lParam);

                    dispatcher->enqueueEvent(Ark::MouseButtonDownEvent{
                        .windowId = windowId,
                        .mouseId = 0,
                        .button = button,
                        .clicks = 1,
                        .x = x,
                        .y = y,
                    });
                }
                break;
            }

            case WM_LBUTTONUP:
            case WM_MBUTTONUP:
            case WM_RBUTTONUP:
            case WM_XBUTTONUP:
            {
                if (dispatcher != nullptr)
                {
                    MouseButton button = MouseButton::Left;
                    if (msg == WM_LBUTTONUP)
                        button = MouseButton::Left;
                    else if (msg == WM_MBUTTONUP)
                        button = MouseButton::Middle;
                    else if (msg == WM_RBUTTONUP)
                        button = MouseButton::Right;
                    else if (msg == WM_XBUTTONUP)
                        button = static_cast<MouseButton>(static_cast<uint32>(MouseButton::Button4) + GET_XBUTTON_WPARAM(wParam) - 1);

                    int x = GET_X_LPARAM(lParam);
                    int y = GET_Y_LPARAM(lParam);

                    dispatcher->enqueueEvent(Ark::MouseButtonUpEvent{
                        .windowId = windowId,
                        .mouseId = 0,
                        .button = button,
                        .clicks = 1,
                        .x = x,
                        .y = y,
                    });
                }
                break;
            }

            case WM_MOUSEWHEEL:
            {
                if (dispatcher != nullptr)
                {
                    float delta = static_cast<float>(GET_WHEEL_DELTA_WPARAM(wParam)) / WHEEL_DELTA;

                    dispatcher->enqueueEvent(Ark::MouseWheelEvent{
                        .windowId = windowId,
                        .mouseId = 0,
                        .x = 0.0f,
                        .y = delta,
                        .direction = MouseWheelDirection::Normal,
                    });
                }
                break;
            }

            case WM_MOUSEHWHEEL:
            {
                if (dispatcher != nullptr)
                {
                    float delta = static_cast<float>(GET_WHEEL_DELTA_WPARAM(wParam)) / WHEEL_DELTA;

                    dispatcher->enqueueEvent(Ark::MouseWheelEvent{
                        .windowId = windowId,
                        .mouseId = 0,
                        .x = delta,
                        .y = 0.0f,
                        .direction = Ark::MouseWheelDirection::Normal,
                    });
                }
                break;
            }

            case WM_GESTURE:
            {
                if (dispatcher != nullptr)
                {
                    GESTUREINFO gi{};
                    gi.cbSize = sizeof(GESTUREINFO);
                    if (GetGestureInfo(reinterpret_cast<HGESTUREINFO>(lParam), &gi))
                    {
                        switch (gi.dwID)
                        {
                            case GID_ZOOM:
                            {
                                float factor = static_cast<float>(static_cast<int>(gi.ullArguments)) / 65536.0f;
                                float magnitude = factor - 1.0f;
                                dispatcher->enqueueEvent(Ark::GestureMagnifyEvent{
                                    .windowId = windowId,
                                    .magnitude = magnitude,
                                });
                                break;
                            }
                            case GID_ROTATE:
                            {
                                float rotation = static_cast<float>(static_cast<int>(gi.ullArguments)) / 65536.0f;
                                dispatcher->enqueueEvent(Ark::GestureRotateEvent{
                                    .windowId = windowId,
                                    .rotation = rotation,
                                });
                                break;
                            }
                            case GID_PAN:
                            {
                                POINT pt = {gi.ptsLocation.x, gi.ptsLocation.y};
                                ScreenToClient(hwnd, &pt);
                                static int lastX = pt.x;
                                static int lastY = pt.y;
                                dispatcher->enqueueEvent(Ark::GesturePanEvent{
                                    .windowId = windowId,
                                    .x = pt.x,
                                    .y = pt.y,
                                    .dx = pt.x - lastX,
                                    .dy = pt.y - lastY,
                                });
                                lastX = pt.x;
                                lastY = pt.y;
                                break;
                            }
                            case GID_TWOFINGERTAP:
                            {
                                dispatcher->enqueueEvent(Ark::GestureTwoFingerTapEvent{.windowId = windowId});
                                break;
                            }
                            default:
                                break;
                        }
                    }
                    CloseGestureInfoHandle(reinterpret_cast<HGESTUREINFO>(lParam));
                }
                return 0;
            }

            case WM_TOUCH:
            {
                if (dispatcher != nullptr)
                {
                    UINT numInputs = LOWORD(wParam);
                    HTOUCHINPUT hTouchInput = reinterpret_cast<HTOUCHINPUT>(lParam);

                    TOUCHINPUT* pInputs = new TOUCHINPUT[numInputs];
                    if (GetTouchInputInfo(hTouchInput, numInputs, pInputs, sizeof(TOUCHINPUT)))
                    {
                        for (UINT i = 0; i < numInputs; ++i)
                        {
                            POINT pt = {pInputs[i].x / 100, pInputs[i].y / 100};
                            ScreenToClient(hwnd, &pt);

                            Ark::TouchPoint touchPoint;
                            touchPoint.id = static_cast<UInt64>(pInputs[i].dwID);
                            touchPoint.x = static_cast<float32>(pt.x);
                            touchPoint.y = static_cast<float32>(pt.y);
                            touchPoint.normalizedX = touchPoint.x;
                            touchPoint.normalizedY = touchPoint.y;
                            touchPoint.pressure = 1.0f;

                            Ark::Collections::Array<Ark::TouchPoint> touches;
                            touches.append(touchPoint);

                            if (pInputs[i].dwFlags & TOUCHEVENTF_DOWN)
                            {
                                dispatcher->enqueueEvent(Ark::TouchBeganEvent{.windowId = windowId, .touches = Ark::move(touches)});
                            }
                            else if (pInputs[i].dwFlags & TOUCHEVENTF_MOVE)
                            {
                                dispatcher->enqueueEvent(Ark::TouchMovedEvent{.windowId = windowId, .touches = Ark::move(touches)});
                            }
                            else if (pInputs[i].dwFlags & TOUCHEVENTF_UP)
                            {
                                dispatcher->enqueueEvent(Ark::TouchEndedEvent{.windowId = windowId, .touches = Ark::move(touches)});
                            }
                        }
                        CloseTouchInputHandle(hTouchInput);
                    }
                    delete[] pInputs;
                }
                return 0;
            }

            case WM_DROPFILES:
            {
                if (dispatcher != nullptr)
                {
                    HDROP hDrop = reinterpret_cast<HDROP>(wParam);
                    POINT pt{};
                    DragQueryPoint(hDrop, &pt);
                    UINT count = DragQueryFileW(hDrop, 0xFFFFFFFF, nullptr, 0);

                    dispatcher->enqueueEvent(Ark::DropBeginEvent{.windowId = windowId});

                    for (UINT i = 0; i < count; ++i)
                    {
                        wchar_t wpath[MAX_PATH] = L"";
                        if (DragQueryFileW(hDrop, i, wpath, MAX_PATH) > 0)
                        {
                            Ark::String file = Ark::Unicode::fromWide(wpath);

                            dispatcher->enqueueEvent(Ark::DropFileEvent{
                                .windowId = windowId,
                                .file = Ark::move(file),
                                .x = static_cast<Ark::float32>(pt.x),
                                .y = static_cast<Ark::float32>(pt.y),
                            });
                        }
                    }

                    dispatcher->enqueueEvent(Ark::DropCompleteEvent{.windowId = windowId});
                    DragFinish(hDrop);
                }
                return 0;
            }

            case WM_SETTINGCHANGE:
            {
                if (dispatcher != nullptr)
                {
                    if (lParam && lstrcmpW(reinterpret_cast<LPCWSTR>(lParam), L"intl") == 0)
                    {
                        dispatcher->enqueueEvent(Ark::LocaleChangedEvent{});
                    }
                }
                break;
            }

            case WM_THEMECHANGED:
            case WM_DWMCOLORIZATIONCOLORCHANGED:
            {
                if (dispatcher != nullptr)
                {
                    dispatcher->enqueueEvent(Ark::ThemeChangedEvent{});
                }
                break;
            }

            default:
                break;
        }

        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }

    static void EnsureWindowClassRegistered()
    {
        static bool registered = false;
        if (registered)
        {
            return;
        }

        WNDCLASSW wc{};
        wc.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = ArkWindowProc;
        wc.hInstance = GetModuleHandleW(NULL);
        wc.hCursor = LoadCursorW(NULL, reinterpret_cast<LPCWSTR>(IDC_ARROW));
        wc.lpszClassName = L"ArkWindowClass";
        RegisterClassW(&wc);
        registered = true;
    }

    class WinWindow final : public Window
    {
        friend void HandleWindowPaint(WinWindow* window, HDC hdc, PAINTSTRUCT const& ps);

    private:
        HWND hwnd = NULL;
        Option<WindowId> parentWindowId;
        uint32 minW = 0, minH = 0;
        uint32 maxW = 0, maxH = 0;
        uint32 arW = 0, arH = 0;
        bool mouseGrabbed = false;
        bool keyboardGrabbed = false;
        Ark::EventDispatcher* eventDispatcher = nullptr;

        // Cached surface for rendering
        struct SurfaceData
        {
            uint8* pixels = nullptr;
            uint32 width = 0;
            uint32 height = 0;
            uint32 strideBytes = 0;
        } cachedSurface;

    public:
        explicit WinWindow(WindowCreateInfo const& info)
        {
            parentWindowId = info.parentId;
            eventDispatcher = info.eventDispatcher;

            DWORD style = WS_OVERLAPPEDWINDOW;
            if ((static_cast<uint64>(info.flags) & static_cast<uint64>(WindowFlags::Borderless)) != 0)
            {
                style = WS_POPUP;
            }
            if ((static_cast<uint64>(info.flags) & static_cast<uint64>(WindowFlags::Resizable)) != 0)
            {
                style |= WS_THICKFRAME;
            }

            auto wtitle = Ark::Unicode::toWide(info.title);

            HWND parentHwnd = NULL;
            if (info.parentId)
            {
                parentHwnd = reinterpret_cast<HWND>(static_cast<UIntPtr>(*info.parentId));
            }

            EnsureWindowClassRegistered();

            hwnd = CreateWindowW(
                L"ArkWindowClass",
                wtitle.asPointer(),
                style,
                CW_USEDEFAULT,
                CW_USEDEFAULT,
                static_cast<int>(info.width),
                static_cast<int>(info.height),
                parentHwnd,
                NULL,
                GetModuleHandleW(NULL),
                this);

            if (hwnd != NULL)
            {
                // Enable gestures (zoom/rotate/pan/two-finger tap) and touch
                GESTURECONFIG gc[4];
                gc[0].dwID = GID_ZOOM;
                gc[0].dwWant = GC_ZOOM;
                gc[0].dwBlock = 0;
                gc[1].dwID = GID_ROTATE;
                gc[1].dwWant = GC_ROTATE;
                gc[1].dwBlock = 0;
                gc[2].dwID = GID_PAN;
                gc[2].dwWant = GC_PAN;
                gc[2].dwBlock = 0;
                gc[3].dwID = GID_TWOFINGERTAP;
                gc[3].dwWant = GC_TWOFINGERTAP;
                gc[3].dwBlock = 0;
                SetGestureConfig(hwnd, 0, 4, gc, sizeof(GESTURECONFIG));

                if ((static_cast<uint64>(info.flags) & static_cast<uint64>(WindowFlags::AcceptTouchEvents)) != 0)
                {
                    RegisterTouchWindow(hwnd, 0);
                }

                // Allow shell drag-and-drop
                DragAcceptFiles(hwnd, TRUE);

                int targetX = CW_USEDEFAULT;
                int targetY = CW_USEDEFAULT;

                if (info.position == WindowPosition::Custom && info.positionX && info.positionY)
                {
                    targetX = *info.positionX;
                    targetY = *info.positionY;
                }
                else if (info.position == WindowPosition::Centered)
                {
                    RECT work{};
                    if (info.display)
                    {
                        HMONITOR mon = reinterpret_cast<HMONITOR>(static_cast<UIntPtr>(*info.display));
                        MONITORINFO mi{};
                        mi.cbSize = sizeof(mi);
                        if (GetMonitorInfoW(mon, &mi))
                        {
                            work = mi.rcWork;
                        }
                    }
                    else
                    {
                        HMONITOR mon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTOPRIMARY);
                        MONITORINFO mi{};
                        mi.cbSize = sizeof(mi);
                        if (GetMonitorInfoW(mon, &mi))
                        {
                            work = mi.rcWork;
                        }
                    }

                    int workW = work.right - work.left;
                    int workH = work.bottom - work.top;
                    targetX = work.left + (workW - static_cast<int>(info.width)) / 2;
                    targetY = work.top + (workH - static_cast<int>(info.height)) / 2;
                }

                if (targetX != CW_USEDEFAULT && targetY != CW_USEDEFAULT)
                {
                    MoveWindow(hwnd, targetX, targetY, static_cast<int>(info.width), static_cast<int>(info.height), TRUE);
                }
            }
        }

        ~WinWindow() override
        {
            setMouseGrab(false);

            // Free cached surface
            if (cachedSurface.pixels != nullptr)
            {
                delete[] cachedSurface.pixels;
                cachedSurface.pixels = nullptr;
            }

            if (hwnd != NULL)
            {
                DestroyWindow(hwnd);
            }
        }

        WindowId getId() const override
        {
            return static_cast<WindowId>(reinterpret_cast<UIntPtr>(hwnd));
        }

        WindowFlags getFlags() const override
        {
            WindowFlags f = WindowFlags::None;
            LONG style = GetWindowLongW(hwnd, GWL_STYLE);

            if ((style & WS_BORDER) == 0)
                f |= WindowFlags::Borderless;
            if ((style & WS_THICKFRAME) != 0)
                f |= WindowFlags::Resizable;
            if (isFullscreen())
                f |= WindowFlags::Fullscreen;
            if (isMinimized())
                f |= WindowFlags::Minimized;
            if (isMaximized())
                f |= WindowFlags::Maximized;
            if (!isVisible())
                f |= WindowFlags::Hidden;

            return f;
        }

        void* getNativeHandle() const override
        {
            return hwnd;
        }

        void show() override
        {
            ShowWindow(hwnd, SW_SHOW);
        }

        void hide() override
        {
            ShowWindow(hwnd, SW_HIDE);
        }

        void setTitle(String const& title) override
        {
            auto wtitle = Ark::Unicode::toWide(title);
            SetWindowTextW(hwnd, wtitle.asPointer());
        }

        String getTitle() const override
        {
            wchar_t wtitle[256] = L"";
            GetWindowTextW(hwnd, wtitle, 256);
            return Ark::Unicode::fromWide(wtitle);
        }

        void setPosition(int32 x, int32 y) override
        {
            RECT r;
            GetWindowRect(hwnd, &r);
            MoveWindow(hwnd, x, y, r.right - r.left, r.bottom - r.top, TRUE);
        }

        void getPosition(int32& x, int32& y) const override
        {
            RECT r;
            GetWindowRect(hwnd, &r);
            x = r.left;
            y = r.top;
        }

        void setSize(uint32 width, uint32 height) override
        {
            RECT r;
            GetWindowRect(hwnd, &r);
            MoveWindow(hwnd, r.left, r.top, static_cast<int>(width), static_cast<int>(height), TRUE);
        }

        void getSize(uint32& width, uint32& height) const override
        {
            RECT r;
            GetClientRect(hwnd, &r);
            width = static_cast<uint32>(r.right - r.left);
            height = static_cast<uint32>(r.bottom - r.top);
        }

        void getSizeInPixels(uint32& width, uint32& height) const override
        {
            getSize(width, height);
        }

        void getBordersSize(int32& left, int32& top, int32& right, int32& bottom) const override
        {
            RECT wr{};
            GetWindowRect(hwnd, &wr);
            RECT cr{};
            GetClientRect(hwnd, &cr);
            POINT tl{cr.left, cr.top};
            ClientToScreen(hwnd, &tl);
            POINT br{cr.right, cr.bottom};
            ClientToScreen(hwnd, &br);
            left = tl.x - wr.left;
            top = tl.y - wr.top;
            right = wr.right - br.x;
            bottom = wr.bottom - br.y;
        }

        float32 getDisplayScale() const override
        {
            HMONITOR mon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
            MONITORINFOEXW info{};
            info.cbSize = sizeof(info);
            UINT dpiX = 96;

            if (GetMonitorInfoW(mon, &info))
            {
                HDC hdc = CreateDCW(L"DISPLAY", info.szDevice, NULL, NULL);
                if (hdc != NULL)
                {
                    dpiX = GetDeviceCaps(hdc, LOGPIXELSX);
                    DeleteDC(hdc);
                }
            }
            return static_cast<float32>(dpiX) / 96.0f;
        }

        void setMinimumSize(uint32 width, uint32 height) override
        {
            minW = width;
            minH = height;
        }

        void getMinimumSize(uint32& width, uint32& height) const override
        {
            width = minW;
            height = minH;
        }

        void setMaximumSize(uint32 width, uint32 height) override
        {
            maxW = width;
            maxH = height;
        }

        void getMaximumSize(uint32& width, uint32& height) const override
        {
            width = maxW;
            height = maxH;
        }

        void setAspectRatio(uint32 width, uint32 height) override
        {
            arW = width;
            arH = height;
        }

        void getAspectRatio(uint32& width, uint32& height) const override
        {
            width = arW;
            height = arH;
        }

        void setOpacity(float32 opacity) override
        {
            LONG ex = GetWindowLongW(hwnd, GWL_EXSTYLE);
            SetWindowLongW(hwnd, GWL_EXSTYLE, ex | WS_EX_LAYERED);
            BYTE alpha = static_cast<BYTE>(opacity * 255.0f);
            SetLayeredWindowAttributes(hwnd, 0, alpha, LWA_ALPHA);
        }

        float32 getOpacity() const override
        {
            BYTE alpha = 255;
            COLORREF cr = 0;
            DWORD flags = 0;
            GetLayeredWindowAttributes(hwnd, &cr, &alpha, &flags);
            return static_cast<float32>(alpha) / 255.0f;
        }

        void setResizable(bool resizable) override
        {
            DWORD style = GetWindowLongW(hwnd, GWL_STYLE);
            if (resizable)
                style |= WS_THICKFRAME;
            else
                style &= ~WS_THICKFRAME;
            SetWindowLongW(hwnd, GWL_STYLE, style);
        }

        void setFocusable(bool focusable) override
        {
            LONG ex = GetWindowLongW(hwnd, GWL_EXSTYLE);
            if (focusable)
                ex &= ~WS_EX_NOACTIVATE;
            else
                ex |= WS_EX_NOACTIVATE;
            SetWindowLongW(hwnd, GWL_EXSTYLE, ex);
        }

        void minimize() override
        {
            ShowWindow(hwnd, SW_MINIMIZE);
        }

        void maximize() override
        {
            ShowWindow(hwnd, SW_MAXIMIZE);
        }

        void restore() override
        {
            ShowWindow(hwnd, SW_RESTORE);
        }

        void raise() override
        {
            SetForegroundWindow(hwnd);
        }

        void flash() override
        {
            FLASHWINFO fw{sizeof(FLASHWINFO), hwnd, FLASHW_ALL | FLASHW_TIMERNOFG, 3, 0};
            FlashWindowEx(&fw);
        }

        void setAlwaysOnTop(bool enabled) override
        {
            SetWindowPos(hwnd, enabled ? HWND_TOPMOST : HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        }

        void setBordered(bool bordered) override
        {
            DWORD style = GetWindowLongW(hwnd, GWL_STYLE);
            if (bordered)
                style |= WS_OVERLAPPEDWINDOW;
            else
                style &= ~WS_OVERLAPPEDWINDOW;
            SetWindowLongW(hwnd, GWL_STYLE, style);
        }

        void setFullscreen(bool enabled, [[maybe_unused]] Option<Mode> mode) override
        {
            if (!enabled)
            {
                setBordered(true);
                ShowWindow(hwnd, SW_RESTORE);
                return;
            }

            setBordered(false);
            ShowWindow(hwnd, SW_MAXIMIZE);
        }

        bool isFullscreen() const override
        {
            LONG style = GetWindowLongW(hwnd, GWL_STYLE);
            return (style & WS_OVERLAPPEDWINDOW) == 0;
        }

        void setMouseGrab(bool grabbed) override
        {
            mouseGrabbed = grabbed;

            if (grabbed)
            {
                RECT rect;
                GetClientRect(hwnd, &rect);
                POINT ul = {rect.left, rect.top};
                POINT lr = {rect.right, rect.bottom};
                ClientToScreen(hwnd, &ul);
                ClientToScreen(hwnd, &lr);
                SetRect(&rect, ul.x, ul.y, lr.x, lr.y);
                ClipCursor(&rect);
            }
            else
            {
                ClipCursor(NULL);
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
            return IsIconic(hwnd) != 0;
        }

        bool isMaximized() const override
        {
            return IsZoomed(hwnd) != 0;
        }

        bool isVisible() const override
        {
            return IsWindowVisible(hwnd) != 0;
        }

        bool hasFocus() const override
        {
            return GetForegroundWindow() == hwnd;
        }

        Option<WindowId> getParentId() const override
        {
            return parentWindowId;
        }

        bool isOpen() const override
        {
            return hwnd != NULL && IsWindow(hwnd) != 0;
        }

        void close() override
        {
            if (hwnd != NULL)
            {
                DestroyWindow(hwnd);
                hwnd = NULL;
            }
        }

        void presentSurface(struct Surface const& surface) override
        {
            if (surface.pixels.isEmpty() || surface.width == 0 || surface.height == 0)
            {
                return;
            }

            uint32 width = surface.width;
            uint32 height = surface.height;
            uint32 stride = (surface.strideBytes != 0) ? surface.strideBytes : width * 4;
            usize surfaceSize = height * stride;

            // Free old cached surface if size changed
            if (cachedSurface.pixels != nullptr &&
                (cachedSurface.width != width || cachedSurface.height != height || cachedSurface.strideBytes != stride))
            {
                delete[] cachedSurface.pixels;
                cachedSurface.pixels = nullptr;
            }

            // Allocate new cached surface if needed
            if (cachedSurface.pixels == nullptr)
            {
                cachedSurface.pixels = new uint8[surfaceSize];
                if (cachedSurface.pixels == nullptr)
                {
                    return;
                }
                cachedSurface.width = width;
                cachedSurface.height = height;
                cachedSurface.strideBytes = stride;
            }

            // Copy surface data
            memcpy(cachedSurface.pixels, surface.pixels.asPointer(), surfaceSize);

            // Invalidate window to trigger repaint
            if (hwnd != NULL)
            {
                InvalidateRect(hwnd, nullptr, FALSE);
            }
        }

        void invalidate() override
        {
            if (hwnd != NULL)
            {
                InvalidateRect(hwnd, nullptr, FALSE);
            }
        }

    public:
        void handlePaint(HDC hdc, PAINTSTRUCT const& ps)
        {
            if (cachedSurface.pixels == nullptr || cachedSurface.width == 0 || cachedSurface.height == 0)
            {
                return;
            }

            uint32 width = cachedSurface.width;
            uint32 height = cachedSurface.height;

            if (width > 0 && height > 0)
            {
                BITMAPINFO bmi = {};
                bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
                bmi.bmiHeader.biWidth = static_cast<LONG>(width);
                bmi.bmiHeader.biHeight = -static_cast<LONG>(height); // Negative for top-down DIB
                bmi.bmiHeader.biPlanes = 1;
                bmi.bmiHeader.biBitCount = 32;
                bmi.bmiHeader.biCompression = BI_RGB;

                StretchDIBits(
                    hdc,
                    static_cast<int>(ps.rcPaint.left),
                    static_cast<int>(ps.rcPaint.top),
                    static_cast<int>(ps.rcPaint.right - ps.rcPaint.left),
                    static_cast<int>(ps.rcPaint.bottom - ps.rcPaint.top),
                    0,
                    0,
                    static_cast<int>(width),
                    static_cast<int>(height),
                    static_cast<const void*>(cachedSurface.pixels),
                    static_cast<const BITMAPINFO*>(&bmi),
                    DIB_RGB_COLORS,
                    SRCCOPY);
            }
        }
    };

    // Paint handler implementation (must be after WinWindow class definition)
    static void HandleWindowPaint(WinWindow* window, HDC hdc, PAINTSTRUCT const& ps)
    {
        if (window == nullptr)
        {
            return;
        }
        window->handlePaint(hdc, ps);
    }

    Window* createWindow(WindowCreateInfo const& info)
    {
        return new WinWindow(info);
    }
}
