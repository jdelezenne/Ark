#if defined(ARK_PLATFORM_WEBASSEMBLY)

#include "Ark/Display/Platform/Web/Window.hpp"

#include "Ark/Application/Event.hpp"
#include "Ark/Application/EventDispatcher.hpp"
#include "Ark/Display/Surface.hpp"

#include <emscripten/emscripten.h>
#include <emscripten/html5.h>

extern "C"
{
EM_JS(void, ark_web_init_canvas, (int width, int height), {
    if (!Module.canvas)
    {
        var c = document.getElementById('canvas');
        if (!c)
        {
            c = document.createElement('canvas');
            c.id = 'canvas';
            document.body.appendChild(c);
        }
        Module.canvas = c;
    }
    Module.canvas.width = width | 0;
    Module.canvas.height = height | 0;
    Module.ctx2d = Module.canvas.getContext('2d', {alpha: true, desynchronized: true});
});

EM_JS(void, ark_web_resize_canvas, (int width, int height), {
    if (Module.canvas)
    {
        Module.canvas.width = width | 0;
        Module.canvas.height = height | 0;
    }
});

EM_JS(void, ark_web_set_title, (char const* title), {
    if (title)
    {
        document.title = UTF8ToString(title);
    }
});

EM_JS(void, ark_web_set_canvas_visible, (int visible), {
    if (Module.canvas)
    {
        Module.canvas.style.display = visible ? 'block' : 'none';
    }
});

EM_JS(int, ark_web_request_fullscreen, (), {
    var c = Module.canvas;
    if (!c)
        return 0;
    if (c.requestFullscreen)
        c.requestFullscreen();
    else if (c.webkitRequestFullscreen)
        c.webkitRequestFullscreen();
    return 1;
});

EM_JS(int, ark_web_exit_fullscreen, (), {
    if (document.exitFullscreen)
        document.exitFullscreen();
    else if (document.webkitExitFullscreen)
        document.webkitExitFullscreen();
    return 1;
});

EM_JS(void, ark_web_present_rgba, (unsigned char const* ptr, int width, int height, int strideBytes), {
    const ctx = Module.ctx2d || (Module.canvas && Module.canvas.getContext('2d'));
    if (!ctx)
        return;
    const w = width | 0, h = height | 0, stride = strideBytes | 0;
    if (w <= 0 || h <= 0)
        return;
    let imgData;
    if (stride === (w << 2))
    {
        const view = HEAPU8.subarray(ptr, ptr + (w * h * 4));
        const data = new Uint8ClampedArray(view);
        imgData = new ImageData(data, w, h);
    }
    else
    {
        const data = new Uint8ClampedArray(w * h * 4);
        let dst = 0;
        let src = ptr | 0;
        for (let y = 0; y < h; ++y)
        {
            const row = HEAPU8.subarray(src, src + (w << 2));
            data.set(row, dst);
            src += stride;
            dst += (w << 2);
        }
        imgData = new ImageData(data, w, h);
    }
    ctx.putImageData(imgData, 0, 0);
});
}

namespace Ark::Display::Platform::Web
{
    class WebWindow final : public Window
    {
    public:
        explicit WebWindow(WindowCreateInfo const& info)
            : title(info.title)
            , width(info.width == 0 ? 1280u : info.width)
            , height(info.height == 0 ? 720u : info.height)
            , flags(info.flags)
            , eventDispatcher(info.eventDispatcher)
            , parentWindowId(info.parentId)
        {
            ark_web_init_canvas(static_cast<int>(width), static_cast<int>(height));
            if (!title.isEmpty())
            {
                ark_web_set_title(title.asPointer());
            }
            visible = (flags & WindowFlags::Hidden) == WindowFlags::None;
            ark_web_set_canvas_visible(visible ? 1 : 0);
            if ((flags & WindowFlags::Fullscreen) != WindowFlags::None)
            {
                setFullscreen(true, none);
            }
        }

        WindowId getId() const override
        {
            return 1;
        }

        WindowFlags getFlags() const override
        {
            return flags;
        }

        void* getNativeHandle() const override
        {
            return nullptr;
        }

        void show() override
        {
            visible = true;
            flags &= ~WindowFlags::Hidden;
            ark_web_set_canvas_visible(1);
            post(WindowShownEvent{.windowId = getId()});
        }

        void hide() override
        {
            visible = false;
            flags |= WindowFlags::Hidden;
            ark_web_set_canvas_visible(0);
            post(WindowHiddenEvent{.windowId = getId()});
        }

        void setTitle(String const& newTitle) override
        {
            title = newTitle;
            ark_web_set_title(title.asPointer());
        }

        String getTitle() const override
        {
            return title;
        }

        void setPosition(int32 /*x*/, int32 /*y*/) override
        {
        }

        void getPosition(int32& x, int32& y) const override
        {
            x = 0;
            y = 0;
        }

        void setSize(uint32 w, uint32 h) override
        {
            width = w == 0 ? 1 : w;
            height = h == 0 ? 1 : h;
            ark_web_resize_canvas(static_cast<int>(width), static_cast<int>(height));
            post(WindowResizedEvent{.windowId = getId(), .width = width, .height = height});
        }

        void getSize(uint32& w, uint32& h) const override
        {
            w = width;
            h = height;
        }

        void getSizeInPixels(uint32& w, uint32& h) const override
        {
            w = width;
            h = height;
        }

        void getBordersSize(int32& l, int32& t, int32& r, int32& b) const override
        {
            l = t = r = b = 0;
        }

        float32 getDisplayScale() const override
        {
            double scale = emscripten_get_device_pixel_ratio();
            return scale > 0.0 ? static_cast<float32>(scale) : 1.0f;
        }

        void setMinimumSize(uint32 w, uint32 h) override
        {
            minW = w;
            minH = h;
        }

        void getMinimumSize(uint32& w, uint32& h) const override
        {
            w = minW;
            h = minH;
        }

        void setMaximumSize(uint32 w, uint32 h) override
        {
            maxW = w;
            maxH = h;
        }

        void getMaximumSize(uint32& w, uint32& h) const override
        {
            w = maxW;
            h = maxH;
        }

        void setAspectRatio(uint32 w, uint32 h) override
        {
            arW = w;
            arH = h;
        }

        void getAspectRatio(uint32& w, uint32& h) const override
        {
            w = arW;
            h = arH;
        }

        void setOpacity(float32 value) override
        {
            opacity = value;
        }

        float32 getOpacity() const override
        {
            return opacity;
        }

        void setResizable(bool resizable) override
        {
            if (resizable)
            {
                flags |= WindowFlags::Resizable;
            }
            else
            {
                flags &= ~WindowFlags::Resizable;
            }
        }

        void setFocusable(bool focusable) override
        {
            if (focusable)
            {
                flags &= ~WindowFlags::NotFocusable;
            }
            else
            {
                flags |= WindowFlags::NotFocusable;
            }
        }

        void minimize() override
        {
        }

        void maximize() override
        {
        }

        void restore() override
        {
            if (fullscreen)
            {
                setFullscreen(false, none);
            }
        }

        void raise() override
        {
            focused = true;
        }

        void flash() override
        {
        }

        void setAlwaysOnTop(bool enabled) override
        {
            if (enabled)
            {
                flags |= WindowFlags::AlwaysOnTop;
            }
            else
            {
                flags &= ~WindowFlags::AlwaysOnTop;
            }
        }

        void setBordered(bool bordered) override
        {
            if (bordered)
            {
                flags &= ~WindowFlags::Borderless;
            }
            else
            {
                flags |= WindowFlags::Borderless;
            }
        }

        void setFullscreen(bool enabled, Option<Mode> /*mode*/) override
        {
            if (enabled)
            {
                ark_web_request_fullscreen();
                flags |= WindowFlags::Fullscreen;
                post(WindowEnteredFullscreenEvent{.windowId = getId()});
            }
            else
            {
                ark_web_exit_fullscreen();
                flags &= ~WindowFlags::Fullscreen;
                post(WindowLeftFullscreenEvent{.windowId = getId()});
            }
            fullscreen = enabled;
        }

        bool isFullscreen() const override
        {
            return fullscreen;
        }

        void setMouseGrab(bool grabbed) override
        {
            mouseGrabbed = grabbed;
            if (grabbed)
            {
                emscripten_request_pointerlock("#canvas", EM_TRUE);
            }
            else
            {
                emscripten_exit_pointerlock();
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
            return false;
        }

        bool isMaximized() const override
        {
            return false;
        }

        bool isVisible() const override
        {
            return visible;
        }

        bool hasFocus() const override
        {
            return focused;
        }

        Option<WindowId> getParentId() const override
        {
            return parentWindowId;
        }

        bool isOpen() const override
        {
            return open;
        }

        void close() override
        {
            if (!open)
            {
                return;
            }
            open = false;
            hide();
            post(WindowCloseRequestedEvent{.windowId = getId()});
            post(WindowClosedEvent{.windowId = getId()});
            post(WindowDestroyedEvent{.windowId = getId()});
        }

        void presentSurface(Surface const& surface) override
        {
            if (surface.pixels.isEmpty() || surface.width == 0 || surface.height == 0)
            {
                return;
            }
            if (width != surface.width || height != surface.height)
            {
                setSize(surface.width, surface.height);
            }
            int stride = (surface.strideBytes != 0) ? static_cast<int>(surface.strideBytes) : static_cast<int>(surface.width * 4);
            ark_web_present_rgba(surface.pixels.asPointer(), static_cast<int>(surface.width), static_cast<int>(surface.height), stride);
        }

        void invalidate() override
        {
            post(WindowExposedEvent{.windowId = getId()});
        }

    private:
        template <typename T>
        void post(T&& event)
        {
            if (eventDispatcher != nullptr)
            {
                eventDispatcher->enqueueEvent(static_cast<T&&>(event));
            }
        }

        String title;
        uint32 width = 1280;
        uint32 height = 720;
        uint32 minW = 0;
        uint32 minH = 0;
        uint32 maxW = 0;
        uint32 maxH = 0;
        uint32 arW = 0;
        uint32 arH = 0;
        float32 opacity = 1.0f;
        WindowFlags flags = WindowFlags::None;
        EventDispatcher* eventDispatcher = nullptr;
        Option<WindowId> parentWindowId;
        bool visible = true;
        bool focused = true;
        bool open = true;
        bool fullscreen = false;
        bool mouseGrabbed = false;
        bool keyboardGrabbed = false;
    };

    Window* createWindow(WindowCreateInfo const& info)
    {
        return new WebWindow(info);
    }
}

#endif
