#include "Ark/Application/Application.hpp"
#include "Ark/Application/Event.hpp"
#include "Ark/Application/EventLoop.hpp"
#include "Ark/Collections/Array.hpp"
#include "Ark/Core/Types.hpp"
#include "Ark/Display/Surface.hpp"
#include "Ark/Display/Window.hpp"
#include "Ark/Math/Color32.hpp"
#include "Ark/Math/Constants.hpp"
#include "Ark/Math/Matrix4.hpp"
#include "Ark/Math/Types.hpp"
#include "Ark/Math/Vector3.hpp"
#include "Ark/Math/Vector4.hpp"
#include "Ark/System/Clock.hpp"
#include "Ark/System/EntryPoint.hpp"

using namespace Ark;

namespace
{
    struct Vertex3D final
    {
        Math::Vector3 position;
        Math::Vector3 normal;
    };

    struct VertexScreen final
    {
        float32 x;
        float32 y;
        float32 depth;
    };

    struct Face final
    {
        uint32 i0;
        uint32 i1;
        uint32 i2;
        uint32 i3;
        Math::Vector3 normal;
        Color32 baseColor;
    };

    inline float32 edgeFunction(float32 ax, float32 ay, float32 bx, float32 by, float32 cx, float32 cy)
    {
        return (cx - ax) * (by - ay) - (cy - ay) * (bx - ax);
    }

    inline uint8 shadeChannel(uint8 channel, float32 intensity)
    {
        float32 scaled = static_cast<float32>(channel) * intensity;
        if (scaled < 0.0f)
        {
            scaled = 0.0f;
        }
        if (scaled > 255.0f)
        {
            scaled = 255.0f;
        }
        return static_cast<uint8>(scaled);
    }

    inline Color32 shadeColor(Color32 const& color, float32 intensity)
    {
        return {
            shadeChannel(color.r, intensity),
            shadeChannel(color.g, intensity),
            shadeChannel(color.b, intensity),
            color.a,
        };
    }

    inline void clearFrame(Collections::Array<uint8>& pixels, Collections::Array<float32>& depth, uint32 width, uint32 height, Color32 const& clearColor)
    {
        uint8* p = pixels.asMutablePointer();
        for (uint32 i = 0; i < width * height; ++i)
        {
            uint32 const o = i * 4;
            p[o + 0] = clearColor.r;
            p[o + 1] = clearColor.g;
            p[o + 2] = clearColor.b;
            p[o + 3] = clearColor.a;
            depth[i] = 1.0e30f;
        }
    }

    inline void drawTriangle(Collections::Array<uint8>& pixels,
                             Collections::Array<float32>& depth,
                             uint32 width,
                             uint32 height,
                             VertexScreen const& v0,
                             VertexScreen const& v1,
                             VertexScreen const& v2,
                             Color32 const& color)
    {
        float32 minXf = Math::min(v0.x, Math::min(v1.x, v2.x));
        float32 maxXf = Math::max(v0.x, Math::max(v1.x, v2.x));
        float32 minYf = Math::min(v0.y, Math::min(v1.y, v2.y));
        float32 maxYf = Math::max(v0.y, Math::max(v1.y, v2.y));

        int32 minX = static_cast<int32>(Math::floor(minXf));
        int32 maxX = static_cast<int32>(Math::ceil(maxXf));
        int32 minY = static_cast<int32>(Math::floor(minYf));
        int32 maxY = static_cast<int32>(Math::ceil(maxYf));

        if (minX < 0)
        {
            minX = 0;
        }
        if (minY < 0)
        {
            minY = 0;
        }
        if (maxX >= static_cast<int32>(width))
        {
            maxX = static_cast<int32>(width) - 1;
        }
        if (maxY >= static_cast<int32>(height))
        {
            maxY = static_cast<int32>(height) - 1;
        }

        float32 const area = edgeFunction(v0.x, v0.y, v1.x, v1.y, v2.x, v2.y);
        if (Math::abs(area) <= Math::ZeroTolerance<float32>)
        {
            return;
        }

        uint8* p = pixels.asMutablePointer();
        float32 const invArea = 1.0f / area;

        for (int32 y = minY; y <= maxY; ++y)
        {
            for (int32 x = minX; x <= maxX; ++x)
            {
                float32 const px = static_cast<float32>(x) + 0.5f;
                float32 const py = static_cast<float32>(y) + 0.5f;

                float32 const w0 = edgeFunction(v1.x, v1.y, v2.x, v2.y, px, py);
                float32 const w1 = edgeFunction(v2.x, v2.y, v0.x, v0.y, px, py);
                float32 const w2 = edgeFunction(v0.x, v0.y, v1.x, v1.y, px, py);

                bool inside = (w0 >= 0.0f && w1 >= 0.0f && w2 >= 0.0f) ||
                              (w0 <= 0.0f && w1 <= 0.0f && w2 <= 0.0f);
                if (!inside)
                {
                    continue;
                }

                float32 const b0 = w0 * invArea;
                float32 const b1 = w1 * invArea;
                float32 const b2 = w2 * invArea;
                float32 const z = b0 * v0.depth + b1 * v1.depth + b2 * v2.depth;

                uint32 const idx = static_cast<uint32>(y) * width + static_cast<uint32>(x);
                if (z >= depth[idx])
                {
                    continue;
                }

                depth[idx] = z;
                uint32 const o = idx * 4;
                p[o + 0] = color.r;
                p[o + 1] = color.g;
                p[o + 2] = color.b;
                p[o + 3] = color.a;
            }
        }
    }
}

bool arkMain(Collections::Array<String> const&)
{
    uint32 windowWidth = 960;
    uint32 windowHeight = 720;

    EventLoop eventLoop;

    Display::WindowCreateInfo info{
        .title = "Ark Rotating Cube",
        .width = windowWidth,
        .height = windowHeight,
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
    Display::WindowId const windowId = window->getId();

    eventLoop.getDispatcher().subscribe<WindowCloseRequestedEvent>(
        [&](WindowCloseRequestedEvent const& event)
        {
            if (event.windowId == windowId)
            {
                shouldQuit = true;
            }
        });

    eventLoop.getDispatcher().subscribe<WindowPixelSizeChangedEvent>(
        [&](WindowPixelSizeChangedEvent const& event)
        {
            if (event.windowId == windowId)
            {
                windowWidth = event.width;
                windowHeight = event.height;
            }
        });

    Collections::Array<uint8> surfacePixels;
    Collections::Array<float32> depthBuffer;

    auto ensureBuffers = [&]()
    {
        surfacePixels.resize(windowWidth * windowHeight * 4);
        depthBuffer.resize(windowWidth * windowHeight);
    };

    ensureBuffers();

    static Vertex3D const vertices[8] = {
        {{-1.0f, -1.0f, -1.0f}, Math::Vector3::Zero},
        {{1.0f, -1.0f, -1.0f}, Math::Vector3::Zero},
        {{1.0f, 1.0f, -1.0f}, Math::Vector3::Zero},
        {{-1.0f, 1.0f, -1.0f}, Math::Vector3::Zero},
        {{-1.0f, -1.0f, 1.0f}, Math::Vector3::Zero},
        {{1.0f, -1.0f, 1.0f}, Math::Vector3::Zero},
        {{1.0f, 1.0f, 1.0f}, Math::Vector3::Zero},
        {{-1.0f, 1.0f, 1.0f}, Math::Vector3::Zero},
    };

    static Face const faces[6] = {
        {4, 5, 6, 7, Math::Vector3::UnitZ, Color32{220, 64, 64, 255}},
        {0, 1, 2, 3, -Math::Vector3::UnitZ, Color32{64, 180, 255, 255}},
        {0, 4, 7, 3, -Math::Vector3::UnitX, Color32{80, 220, 120, 255}},
        {1, 5, 6, 2, Math::Vector3::UnitX, Color32{240, 190, 70, 255}},
        {3, 7, 6, 2, Math::Vector3::UnitY, Color32{190, 100, 240, 255}},
        {0, 4, 5, 1, -Math::Vector3::UnitY, Color32{250, 250, 250, 255}},
    };

    float64 const startTime = System::Clock::getTimeSeconds();

    while (!shouldQuit && window->isOpen())
    {
        eventLoop.pollEvents();

        if (windowWidth == 0 || windowHeight == 0)
        {
            continue;
        }

        if (surfacePixels.getCount() != windowWidth * windowHeight * 4)
        {
            ensureBuffers();
        }

        float32 const t = static_cast<float32>(System::Clock::getTimeSeconds() - startTime);
        float32 const yaw = t * 0.9f;
        float32 const pitch = t * 0.6f;

        Math::Matrix4 const world =
            Math::Matrix4::rotationY(yaw) *
            Math::Matrix4::rotationX(pitch) *
            Math::Matrix4::scaling(0.85f);

        float32 const aspect = static_cast<float32>(windowWidth) / static_cast<float32>(windowHeight);
        Math::Vector3 const eye =
            (Math::CurrentCoordinateSystem == Math::CoordinateSystem::LeftHanded)
                ? Math::Vector3{0.0f, 0.0f, -4.0f}
                : Math::Vector3{0.0f, 0.0f, 4.0f};
        Math::Matrix4 const view = Math::Matrix4::lookAt(eye, Math::Vector3::Zero, Math::Vector3::UnitY);
        Math::Matrix4 const projection = Math::Matrix4::perspectiveFieldOfView(Math::degreesToRadians(60.0f), aspect, 0.1f, 100.0f);

        Math::Vector3 const lightDir = Math::Vector3(0.4f, 0.7f, -1.0f).getNormalized();

        clearFrame(surfacePixels, depthBuffer, windowWidth, windowHeight, Color32{18, 24, 34, 255});

        VertexScreen projected[8];
        Math::Vector3 viewPositions[8];

        for (uint32 i = 0; i < 8; ++i)
        {
            Math::Vector4 const worldPos4 = Math::Matrix4::transformPoint(vertices[i].position, world);

            Math::Vector4 const viewPos4 = Math::Matrix4::transformVector(worldPos4, view);
            viewPositions[i] = {viewPos4.x, viewPos4.y, viewPos4.z};
            Math::Vector4 clip = Math::Matrix4::transformVector(viewPos4, projection);
            if (Math::abs(clip.w) <= Math::ZeroTolerance<float32>)
            {
                clip.w = 1.0f;
            }

            float32 const invW = 1.0f / clip.w;
            float32 const ndcX = clip.x * invW;
            float32 const ndcY = clip.y * invW;
            float32 const ndcZ = clip.z * invW;

            projected[i].x = (ndcX * 0.5f + 0.5f) * static_cast<float32>(windowWidth);
            projected[i].y = (1.0f - (ndcY * 0.5f + 0.5f)) * static_cast<float32>(windowHeight);
            projected[i].depth = ndcZ;
        }

        for (Face const& face : faces)
        {
            Math::Vector3 const worldNormal = Math::Matrix4::transformVector(face.normal, world).getNormalized();
            Math::Vector3 const viewNormal = Math::Matrix4::transformVector(worldNormal, view).getNormalized();
            Math::Vector3 const viewCenter =
                (viewPositions[face.i0] + viewPositions[face.i1] + viewPositions[face.i2] + viewPositions[face.i3]) / 4.0f;
            Math::Vector3 const toCamera = -viewCenter;
            if (Math::Vector3::dot(viewNormal, toCamera) <= 0.0f)
            {
                continue;
            }

            float32 const nDotL = Math::Vector3::dot(worldNormal, -lightDir);
            float32 const intensity = Math::clamp(0.25f + nDotL * 0.75f, 0.0f, 1.0f);
            Color32 const shaded = shadeColor(face.baseColor, intensity);

            drawTriangle(surfacePixels, depthBuffer, windowWidth, windowHeight, projected[face.i0], projected[face.i1], projected[face.i2], shaded);
            drawTriangle(surfacePixels, depthBuffer, windowWidth, windowHeight, projected[face.i0], projected[face.i2], projected[face.i3], shaded);
        }

        Display::Surface surface{
            .width = windowWidth,
            .height = windowHeight,
            .format = Display::PixelFormat::R8G8B8A8,
            .strideBytes = windowWidth * 4,
            .pixels = surfacePixels,
        };

        window->presentSurface(surface);
    }
    return true;
}
