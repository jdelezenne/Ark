#pragma once

#include "Ark/Core/Types.hpp"
#include "Ark/Math/Math.hpp"

namespace Ark
{
    struct Color32;

    /// Represents a color with floating-point channels (0.0 - 1.0).
    struct Color final
    {
        union
        {
            struct
            {
                float32 r; ///< Red channel value (0.0 - 1.0).
                float32 g; ///< Green channel value (0.0 - 1.0).
                float32 b; ///< Blue channel value (0.0 - 1.0).
                float32 a; ///< Alpha channel value (0.0 - 1.0).
            };

            float32 data[4]; ///< Array of channel values [r, g, b, a] in [0.0, 1.0].
        };

        /// Constructs a black color with full opacity.
        constexpr Color()
            : r{0.0f}
            , g{0.0f}
            , b{0.0f}
            , a{1.0f}
        {
        }

        /// Constructs a color from individual channel values.
        /// @param r Red channel (0.0 - 1.0).
        /// @param g Green channel (0.0 - 1.0).
        /// @param b Blue channel (0.0 - 1.0).
        /// @param a Alpha channel (0.0 - 1.0, default 1.0).
        constexpr Color(float32 r, float32 g, float32 b, float32 a = 1.0f)
            : r{r}
            , g{g}
            , b{b}
            , a{a}
        {
        }

        /// Constructs a color from an existing color with a new alpha.
        /// @param color The source color (rgb are copied).
        /// @param a The new alpha value.
        constexpr Color(Color const& color, float32 a)
            : r{color.r}
            , g{color.g}
            , b{color.b}
            , a{a}
        {
        }

        /// Constructs a grayscale color with the given value for all channels including alpha.
        /// @param value The value applied to all four channels.
        constexpr explicit Color(float32 value)
            : r{value}
            , g{value}
            , b{value}
            , a{value}
        {
        }

        /// Constructs a color from a Vector3 (rgb) and an alpha value.
        /// @param vector The rgb components.
        /// @param a The alpha channel (0.0 - 1.0).
        Color(Math::Vector3 const& vector, float32 a);

        /// Constructs a color from a Vector4 (rgba).
        /// @param vector The rgba components.
        Color(Math::Vector4 const& vector);

        /// Gets the red channel value of the color.
        inline float32 red() const
        {
            return r;
        }

        /// Gets the green channel value of the color.
        inline float32 green() const
        {
            return g;
        }

        /// Gets the blue channel value of the color.
        inline float32 blue() const
        {
            return b;
        }

        /// Gets the alpha channel value of the color.
        inline float32 alpha() const
        {
            return a;
        }

        /// Creates a Color from floating-point values (0.0 - 1.0).
        static Color constexpr fromUInt8(uint8 r, uint8 g, uint8 b, uint8 a = 255);

        /// Creates a Color from a 32-bit RGBA value.
        static Color constexpr fromRgba(uint32 rgba);

        /// Creates a Color from a 32-bit RGBA value.
        static Color constexpr fromArgb(uint32 argb);

        /// Creates a Color from a Color32 value.
        static Color fromColor32(Color32 const& value);

        /// Converts the Color to a Color32.
        Color32 toUInt8() const;

        /// Converts this color to a Vector3 (rgb components).
        Math::Vector3 toVector3() const;

        /// Converts this color to a Vector4 (rgba components).
        Math::Vector4 toVector4() const;

        /// Returns the color as a packed 32-bit RGBA value.
        uint32 toRgba() const;

        /// Returns the color as a packed 32-bit ARGB value.
        uint32 toArgb() const;

        /// Clamps all channel values to [0.0, 1.0] in place.
        void clamp();

        /// Returns a clamped copy of this color with all channels in [0.0, 1.0].
        Color clamped() const;

        /// Returns the component-wise minimum of two colors.
        static Color min(Color const& a, Color const& b);

        /// Returns the component-wise maximum of two colors.
        static Color max(Color const& a, Color const& b);

        /// Linearly interpolates between two colors.
        /// @param start The starting color (amount == 0).
        /// @param end The ending color (amount == 1).
        /// @param amount The interpolation factor in [0, 1].
        /// @return The interpolated color.
        static Color lerp(Color const& start, Color const& end, float32 amount);

        /// Computes the luminance (luma) of this color.
        /// @return The luminance value.
        float getLuma() const;

        /// Applies gamma correction to this color.
        /// @param gamma The gamma exponent.
        /// @return The gamma-corrected color.
        Color applyGamma(float32 gamma) const;

        /// Creates a Color from HSV (hue, saturation, value) components.
        /// @param h Hue in [0, 360].
        /// @param s Saturation in [0, 1].
        /// @param v Value in [0, 1].
        /// @param a Alpha in [0, 1] (default 1.0).
        /// @return The corresponding Color.
        static Color fromHSV(float32 h, float32 s, float32 v, float32 a = 1.0f);

        /// Converts this color to HSV components.
        /// @param defaultHue The hue to return when saturation is zero.
        /// @param defaultSaturation The saturation to return when value is zero.
        /// @return A Vector3 containing hue, saturation, and value.
        Math::Vector3 toHSV(float32 defaultHue = 0, float32 defaultSaturation = 0) const;

        inline float32& operator[](int index)
        {
            return data[index];
        }

        inline const float32& operator[](int index) const
        {
            return data[index];
        }

        inline bool operator==(Color const& color) const
        {
            return (r == color.r && g == color.g && b == color.b && a == color.a);
        }

        inline bool operator!=(Color const& color) const
        {
            return (r != color.r || g != color.g || b != color.b || a != color.a);
        }

        Color operator-() const;

        Color operator+(Color const& color) const;
        Color operator+(float32 scalar) const;
        Color& operator+=(Color const& color);
        Color& operator+=(float32 scalar);

        Color operator-(Color const& color) const;
        Color operator-(float32 scalar) const;
        Color& operator-=(Color const& color);
        Color& operator-=(float32 scalar);

        Color operator*(Color const& color) const;
        Color& operator*=(Color const& color);

        Color operator*(float32 scalar) const;
        Color& operator*=(float32 scalar);

        Color operator/(Color const& color) const;
        Color operator/(float32 scalar) const;
        Color& operator/=(Color const& color);
        Color& operator/=(float32 scalar);
    };

    Color constexpr Color::fromUInt8(uint8 r, uint8 g, uint8 b, uint8 a)
    {
        return {
            static_cast<float32>(r) / 255.f,
            static_cast<float32>(g) / 255.f,
            static_cast<float32>(b) / 255.f,
            static_cast<float32>(a) / 255.f,
        };
    }
    Color constexpr Color::fromRgba(uint32 rgba)
    {
        return {
            ((rgba & 0xFF000000) >> 24) / 255.f,
            ((rgba & 0x00FF0000) >> 16) / 255.f,
            ((rgba & 0x0000FF00) >> 8) / 255.f,
            (rgba & 0x000000FF) / 255.f,
        };
    }

    Color constexpr Color::fromArgb(uint32 argb)
    {
        return {
            ((argb & 0x00FF0000) >> 16) / 255.f,
            ((argb & 0x0000FF00) >> 8) / 255.f,
            (argb & 0x000000FF) / 255.f,
            ((argb & 0xFF000000) >> 24) / 255.f,
        };
    }
}
