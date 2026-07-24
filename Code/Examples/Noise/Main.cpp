#include "Ark/Application/Application.hpp"
#include "Ark/Application/Event.hpp"
#include "Ark/Application/EventLoop.hpp"
#include "Ark/Core/Types.hpp"
#include "Ark/Display/Surface.hpp"
#include "Ark/Display/Window.hpp"
#include "Ark/Math/PcgRandom.hpp"
#include "Ark/System/Clock.hpp"
#include "Ark/System/EntryPoint.hpp"

using namespace Ark;

bool arkMain(Collections::Array<String> const&)
{
    constexpr uint32 width = 800;
    constexpr uint32 height = 600;
    constexpr uint32 bytesPerPixel = 4; // RGBA

    EventLoop eventLoop;

    Display::WindowCreateInfo info{
        .title = "Ark Noise Example",
        .width = width,
        .height = height,
        .position = Display::WindowPosition::Centered,
        .flags = Display::WindowFlags::Resizable,
        .eventDispatcher = &eventLoop.getDispatcher(),
    };

    auto window = UniquePointer<Display::Window>(Display::Window::create(info));
    if (window == nullptr)
    {
        return false;
    }

    window->show();

    bool shouldQuit = false;
    Display::WindowId windowId = window->getId();

    eventLoop.getDispatcher().subscribe<WindowCloseRequestedEvent>(
        [&](WindowCloseRequestedEvent const& event)
        {
            if (event.windowId == windowId)
            {
                shouldQuit = true;
            }
        });

    // Create surface buffer
    uint32 surfaceWidth = width;
    uint32 surfaceHeight = height;
    uint32 surfaceStride = surfaceWidth * bytesPerPixel;
    Collections::Array<uint8> surfacePixels;
    surfacePixels.resize(surfaceWidth * surfaceHeight * bytesPerPixel);

    // Random number generator
    Math::PcgRandom rng;

    // Time tracking for animation
    float64 lastTime = System::Clock::getTimeSeconds();

    while (!shouldQuit && window->isOpen())
    {
        eventLoop.pollEvents();

        // Get current time
        float64 currentTime = System::Clock::getTimeSeconds();
        float64 deltaTime = currentTime - lastTime;

        // Use time as seed offset for animated noise
        // This creates a time-based noise pattern that changes smoothly
        uint64 timeSeed = static_cast<uint64>(currentTime * 1000.0);
        rng.setSeed(timeSeed);

        // Generate noise pattern
        uint8* pixels = surfacePixels.asMutablePointer();
        for (uint32 y = 0; y < surfaceHeight; ++y)
        {
            for (uint32 x = 0; x < surfaceWidth; ++x)
            {
                uint32 index = (y * surfaceWidth + x) * bytesPerPixel;

                // Generate random RGB values
                // Using time-based seed ensures the pattern changes over time
                uint8 r = static_cast<uint8>(rng.nextUInt32(0, 255));
                uint8 g = static_cast<uint8>(rng.nextUInt32(0, 255));
                uint8 b = static_cast<uint8>(rng.nextUInt32(0, 255));
                uint8 a = 255; // Fully opaque

                pixels[index + 0] = r; // R
                pixels[index + 1] = g; // G
                pixels[index + 2] = b; // B
                pixels[index + 3] = a; // A
            }
        }

        // Create surface and present it
        Display::Surface surface{
            .width = surfaceWidth,
            .height = surfaceHeight,
            .format = Display::PixelFormat::R8G8B8A8,
            .strideBytes = surfaceStride,
            .pixels = surfacePixels,
        };

        window->presentSurface(surface);

        lastTime = currentTime;

        // Small sleep to avoid spinning too fast (optional)
        // You could use a frame rate limiter here if desired
    }

    return true;
}
