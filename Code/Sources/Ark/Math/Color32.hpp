#pragma once

#include "Ark/Core/Types.hpp"
#include "Ark/Math/Math.hpp"

namespace Ark
{
    struct Color;

    /// Represents a color with 8-bit unsigned integer channels (0 - 255).
    struct Color32 final
    {
        union
        {
            struct
            {
                uint8 r; ///> Red channel value (0 - 255).
                uint8 g; ///> Green channel value (0 - 255).
                uint8 b; ///> Blue channel value (0 - 255).
                uint8 a; ///> Alpha channel value (0 - 255).
            };

            uint8 data[4]; ///> Array of channel values (0 - 255).
            uint32 value;  ///> Combined color value (rgbA format).
        };

        constexpr Color32()
            : r{0}
            , g{0}
            , b{0}
            , a{1}
        {
        }

        constexpr Color32(uint8 r, uint8 g, uint8 b, uint8 a = 255)
            : r{r}
            , g{g}
            , b{b}
            , a{a}
        {
        }

        constexpr Color32(uint8 color)
            : r{color}
            , g{color}
            , b{color}
            , a{color}
        {
        }

        /// Gets the red channel value of the color.
        constexpr uint8 red() const
        {
            return r;
        }

        /// Gets the green channel value of the color.
        constexpr uint8 green() const
        {
            return g;
        }

        /// Gets the blue channel value of the color.
        constexpr uint8 blue() const
        {
            return b;
        }

        /// Gets the alpha channel value of the color.
        constexpr uint8 alpha() const
        {
            return a;
        }

        /// Creates a Color32 from floating-point values (0.0 - 1.0).
        static constexpr Color32 fromFloat(float32 r, float32 g, float32 b, float32 a = 1.0f);

        /// Creates a Color32 from clamped floating-point values (0.0 - 1.0).
        static constexpr Color32 fromFloatClamped(float32 r, float32 g, float32 b, float32 a = 1.0f);

        /// Creates a Color32 from a 32-bit rgbA value.
        static constexpr Color32 fromRgba(uint32 rgba);

        /// Creates a Color32 from a 32-bit Argb value.
        static constexpr Color32 fromArgb(uint32 argb);

        /// Creates a Color32 from a Color value.
        static Color32 fromColor(Color const& value);

        /// Converts the Color32 to a Color.
        Color toFloat() const;

        /// Converts the Color32 to a Color.
        Math::Vector3 toVector3() const;

        /// Converts the Color32 to a Vector4.
        Math::Vector4 toVector4() const;

        static constexpr uint32 toRgba(uint8 r, uint8 g, uint8 b, uint8 a)
        {
            return static_cast<uint32>((r << 24) |
                                       (g << 16) |
                                       (b << 8) |
                                       a);
        }

        static constexpr uint32 toRgbx(uint8 r, uint8 g, uint8 b)
        {
            return static_cast<uint32>((r << 24) |
                                       (g << 16) |
                                       (b << 8));
        }

        static constexpr uint32 toBgra(uint8 r, uint8 g, uint8 b, uint8 a)
        {
            return static_cast<uint32>((b << 0) |
                                       (g << 8) |
                                       (r << 16) |
                                       (a << 24));
        }

        static constexpr uint32 toArgb(uint8 r, uint8 g, uint8 b, uint8 a)
        {
            return static_cast<uint32>((a << 24) |
                                       (r << 16) |
                                       (g << 8) |
                                       b);
        }

        static constexpr uint32 toXrgb(uint8 r, uint8 g, uint8 b)
        {
            return static_cast<uint32>((r << 16) |
                                       (g << 8) |
                                       b);
        }

        /// Converts the Color32 to a 32-bit rgbA value.
        constexpr uint32 toRgba() const
        {
            return toRgba(r, g, b, a);
        }

        /// Converts the Color32 to a 32-bit rgbX value (no alpha).
        constexpr uint32 toRgbx() const
        {
            return toRgbx(r, g, b);
        }

        /// Converts the Color32 to a 32-bit Argb value.
        constexpr uint32 toArgb() const
        {
            return toArgb(r, g, b, a);
        }

        /// Converts the Color32 to a 32-bit Xrgb value (no alpha).
        constexpr uint32 toXrgb() const
        {
            return toXrgb(r, g, b);
        }

        inline uint8& operator[](int index)
        {
            return data[index];
        }

        inline const uint8& operator[](int index) const
        {
            return data[index];
        }

        inline bool operator==(Color32 const& color) const
        {
            return (r == color.r && g == color.g && b == color.b && a == color.a);
        }

        inline bool operator!=(Color32 const& color) const
        {
            return (r != color.r || g != color.g || b != color.b || a != color.a);
        }

        Color32 operator+(Color32 const& color) const;
        Color32& operator+=(Color32 const& color);

        Color32 operator-(Color32 const& color) const;
        Color32& operator-=(Color32 const& color);

        Color32 operator*(Color32 const& color) const;
        Color32& operator*=(Color32 const& color);
    };

    constexpr Color32 Color32::fromFloat(float32 r, float32 g, float32 b, float32 a)
    {
        return {
            static_cast<uint8>(r * 255.0f),
            static_cast<uint8>(g * 255.0f),
            static_cast<uint8>(b * 255.0f),
            static_cast<uint8>(a * 255.0f),
        };
    }

    constexpr Color32 Color32::fromFloatClamped(float32 r, float32 g, float32 b, float32 a)
    {
        return {
            static_cast<uint8>(Math::clamp(r * 255.0f, 0.0f, 255.0f)),
            static_cast<uint8>(Math::clamp(g * 255.0f, 0.0f, 255.0f)),
            static_cast<uint8>(Math::clamp(b * 255.0f, 0.0f, 255.0f)),
            static_cast<uint8>(Math::clamp(a * 255.0f, 0.0f, 255.0f)),
        };
    }

    constexpr Color32 Color32::fromRgba(uint32 rgba)
    {
        return {
            static_cast<uint8>((rgba & 0xff000000) >> 24),
            static_cast<uint8>((rgba & 0x00ff0000) >> 16),
            static_cast<uint8>((rgba & 0x0000ff00) >> 8),
            static_cast<uint8>(rgba & 0x000000ff),
        };
    }

    constexpr Color32 Color32::fromArgb(uint32 argb)
    {
        return {
            static_cast<uint8>((argb & 0x00ff0000) >> 16),
            static_cast<uint8>((argb & 0x0000ff00) >> 8),
            static_cast<uint8>(argb & 0x000000ff),
            static_cast<uint8>((argb & 0xff000000) >> 24),
        };
    }
}
