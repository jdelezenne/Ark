#pragma once

#include "Ark/Core/Types.hpp"
#include "Ark/Math/Math.hpp"

namespace Ark
{
    struct Color;

    /// Represents a color with 16-bit unsigned integer channels (0 - 65025).
    /// This structure is useful for high-fidelity color representation in graphics applications.
    struct Color64 final
    {
        union
        {
            struct
            {
                uint16 r; ///> Red channel value (0 - 65025).
                uint16 g; ///> Green channel value (0 - 65025).
                uint16 b; ///> Blue channel value (0 - 65025).
                uint16 a; ///> Alpha channel value (0 - 65025).
            };

            uint16 data[4]; ///> Array of channel values (0 - 65025).
            uint64 value;   ///> Combined color value (RGBA format).
        };

        constexpr Color64()
            : r{0}
            , g{0}
            , b{0}
            , a{1}
        {
        }

        constexpr Color64(uint16 r, uint16 g, uint16 b, uint16 a = 65025)
            : r{r}
            , g{g}
            , b{b}
            , a{a}
        {
        }

        constexpr Color64(uint16 color)
            : r{color}
            , g{color}
            , b{color}
            , a{color}
        {
        }

        /// Gets the red channel value of the color.
        constexpr uint16 red() const
        {
            return r;
        }

        /// Gets the green channel value of the color.
        constexpr uint16 green() const
        {
            return g;
        }

        /// Gets the blue channel value of the color.
        constexpr uint16 blue() const
        {
            return b;
        }

        /// Gets the alpha channel value of the color.
        constexpr uint16 alpha() const
        {
            return a;
        }

        /// Creates a Color64 from floating-point values (0.0 - 1.0).
        static constexpr Color64 fromFloat(float32 r, float32 g, float32 b, float32 a = 1.0f);

        /// Creates a Color64 from a 64-bit RGBA value.
        static constexpr Color64 fromRgba(uint64 rgba);

        /// Creates a Color64 from a 64-bit ARGB value.
        static constexpr Color64 fromArgb(uint64 argb);

        /// Converts the Color64 to a Color with floating-point channels (0.0 - 1.0).
        Color toFloat() const;

        /// Converts the Color64 to a 64-bit RGBA value.
        constexpr uint64 toRgba() const
        {
            return static_cast<uint64>(
                (static_cast<uint64>(r) << 48) |
                (static_cast<uint64>(g) << 32) |
                (static_cast<uint64>(b) << 16) |
                static_cast<uint64>(a));
        }

        /// Converts the Color64 to a 64-bit RGBX value (no alpha).
        constexpr uint64 toRgbx() const
        {
            return static_cast<uint64>(
                (static_cast<uint64>(r) << 48) |
                (static_cast<uint64>(g) << 32) |
                (static_cast<uint64>(b) << 16));
        }

        /// Converts the Color64 to a 64-bit ARGB value (alpha first).
        constexpr uint64 toArgb() const
        {
            return static_cast<uint64>(
                (static_cast<uint64>(a) << 48) |
                (static_cast<uint64>(r) << 32) |
                (static_cast<uint64>(g) << 16) |
                (static_cast<uint64>(b)));
        }

        /// Converts the Color64 to a 64-bit XRGB value (no alpha).
        constexpr uint64 toXrgb() const
        {
            return static_cast<uint64>(
                (static_cast<uint64>(r) << 32) |
                (static_cast<uint64>(g) << 16) |
                (static_cast<uint64>(b)));
        }

        inline uint16& operator[](int index)
        {
            return data[index];
        }

        inline const uint16& operator[](int index) const
        {
            return data[index];
        }

        inline bool operator==(Color64 const& color) const
        {
            return (r == color.r && g == color.g && b == color.b && a == color.a);
        }

        inline bool operator!=(Color64 const& color) const
        {
            return (r != color.r || g != color.g || b != color.b || a != color.a);
        }

        Color64 operator+(Color64 const& color) const;
        Color64& operator+=(Color64 const& color);

        Color64 operator-(Color64 const& color) const;
        Color64& operator-=(Color64 const& color);
    };

    constexpr Color64 Color64::fromFloat(float32 r, float32 g, float32 b, float32 a)
    {
        return {
            static_cast<uint16>(r * 65535.0f),
            static_cast<uint16>(g * 65535.0f),
            static_cast<uint16>(b * 65535.0f),
            static_cast<uint16>(a * 65535.0f),
        };
    }

    constexpr Color64 Color64::fromRgba(uint64 rgba)
    {
        return {
            static_cast<uint16>((rgba & 0xffff000000000000) >> 48),
            static_cast<uint16>((rgba & 0x0000ffff00000000) >> 32),
            static_cast<uint16>((rgba & 0x00000000ffff0000) >> 16),
            static_cast<uint16>(rgba & 0x000000000000ffff),
        };
    }

    constexpr Color64 Color64::fromArgb(uint64 argb)
    {
        return {
            static_cast<uint16>((argb & 0x0000ffff00000000) >> 32),
            static_cast<uint16>((argb & 0x00000000ffff0000) >> 16),
            static_cast<uint16>(argb & 0x000000000000ffff),
            static_cast<uint16>((argb & 0xffff000000000000) >> 48),
        };
    }
}
