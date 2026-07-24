#pragma once

#include "Ark/Math/Color.hpp"
#include "Ark/Math/Color32.hpp"

namespace Ark::Math
{
    class Random;
}

namespace Ark
{
    /// Generates a random Color with RGB values in [min, max] and alpha = 1.0.
    /// @param rng The random number generator.
    /// @param minComponent Minimum value for each RGB component [0.0, 1.0].
    /// @param maxComponent Maximum value for each RGB component [0.0, 1.0].
    /// @return A random Color with full opacity.
    Color randomColorRGB(Ark::Math::Random& rng, float32 minComponent = 0.0f, float32 maxComponent = 1.0f);

    /// Generates a random Color with RGBA values in [min, max].
    /// @param rng The random number generator.
    /// @param minComponent Minimum value for each RGBA component [0.0, 1.0].
    /// @param maxComponent Maximum value for each RGBA component [0.0, 1.0].
    /// @return A random Color with random alpha.
    Color randomColorRGBA(Ark::Math::Random& rng, float32 minComponent = 0.0f, float32 maxComponent = 1.0f);

    /// Generates a random Color32 with RGB values in [min, max] and alpha = 255.
    /// @param rng The random number generator.
    /// @param minComponent Minimum value for each RGB component [0, 255].
    /// @param maxComponent Maximum value for each RGB component [0, 255].
    /// @return A random Color32 with full opacity.
    Color32 randomColor32RGB(Ark::Math::Random& rng, uint8 minComponent = 0, uint8 maxComponent = 255);

    /// Generates a random Color32 with RGBA values in [min, max].
    /// @param rng The random number generator.
    /// @param minComponent Minimum value for each RGBA component [0, 255].
    /// @param maxComponent Maximum value for each RGBA component [0, 255].
    /// @return A random Color32 with random alpha.
    Color32 randomColor32RGBA(Ark::Math::Random& rng, uint8 minComponent = 0, uint8 maxComponent = 255);

    /// Generates a random Color using HSV color space.
    /// @param rng The random number generator.
    /// @param hueMin Minimum hue value [0.0, 1.0].
    /// @param hueMax Maximum hue value [0.0, 1.0].
    /// @param satMin Minimum saturation value [0.0, 1.0].
    /// @param satMax Maximum saturation value [0.0, 1.0].
    /// @param valMin Minimum value/brightness [0.0, 1.0].
    /// @param valMax Maximum value/brightness [0.0, 1.0].
    /// @return A random Color generated from HSV space.
    Color randomColorHSV(Ark::Math::Random& rng,
                         float32 hueMin = 0.0f,
                         float32 hueMax = 1.0f,
                         float32 satMin = 0.0f,
                         float32 satMax = 1.0f,
                         float32 valMin = 0.0f,
                         float32 valMax = 1.0f);
}
