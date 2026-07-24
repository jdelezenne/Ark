#include "Ark/Math/ColorRandomUtilities.hpp"

#include "Ark/Math/Intrinsics.hpp"
#include "Ark/Math/Random.hpp"

namespace Ark
{
    Color randomColorRGB(Math::Random& rng, float32 minComponent, float32 maxComponent)
    {
        return Color(rng.nextFloat32(minComponent, maxComponent),
                     rng.nextFloat32(minComponent, maxComponent),
                     rng.nextFloat32(minComponent, maxComponent),
                     1.0f);
    }

    Color randomColorRGBA(Math::Random& rng, float32 minComponent, float32 maxComponent)
    {
        return Color(rng.nextFloat32(minComponent, maxComponent),
                     rng.nextFloat32(minComponent, maxComponent),
                     rng.nextFloat32(minComponent, maxComponent),
                     rng.nextFloat32(minComponent, maxComponent));
    }

    Color32 randomColor32RGB(Math::Random& rng, uint8 minComponent, uint8 maxComponent)
    {
        return Color32(static_cast<uint8>(rng.nextUInt32(minComponent, maxComponent)),
                       static_cast<uint8>(rng.nextUInt32(minComponent, maxComponent)),
                       static_cast<uint8>(rng.nextUInt32(minComponent, maxComponent)),
                       255);
    }

    Color32 randomColor32RGBA(Math::Random& rng, uint8 minComponent, uint8 maxComponent)
    {
        return Color32(static_cast<uint8>(rng.nextUInt32(minComponent, maxComponent)),
                       static_cast<uint8>(rng.nextUInt32(minComponent, maxComponent)),
                       static_cast<uint8>(rng.nextUInt32(minComponent, maxComponent)),
                       static_cast<uint8>(rng.nextUInt32(minComponent, maxComponent)));
    }

    Color randomColorHSV(Math::Random& rng,
                         float32 hueMin,
                         float32 hueMax,
                         float32 satMin,
                         float32 satMax,
                         float32 valMin,
                         float32 valMax)
    {
        float32 h = rng.nextFloat32(hueMin, hueMax);
        float32 s = rng.nextFloat32(satMin, satMax);
        float32 v = rng.nextFloat32(valMin, valMax);

        float32 c = v * s;
        float32 x = c * (1.0f - Math::abs(Math::mod(h * 6.0f, 2.0f) - 1.0f));
        float32 m = v - c;

        float32 r = 0.0f;
        float32 g = 0.0f;
        float32 b = 0.0f;

        float32 hSector = h * 6.0f;

        if (hSector < 1.0f)
        {
            r = c;
            g = x;
            b = 0.0f;
        }
        else if (hSector < 2.0f)
        {
            r = x;
            g = c;
            b = 0.0f;
        }
        else if (hSector < 3.0f)
        {
            r = 0.0f;
            g = c;
            b = x;
        }
        else if (hSector < 4.0f)
        {
            r = 0.0f;
            g = x;
            b = c;
        }
        else if (hSector < 5.0f)
        {
            r = x;
            g = 0.0f;
            b = c;
        }
        else
        {
            r = c;
            g = 0.0f;
            b = x;
        }

        return Color(r + m, g + m, b + m, 1.0f);
    }
}
