#include "Ark/Math/Color.hpp"
#include "Ark/Math/Color32.hpp"
#include "Ark/Math/Constants.hpp"
#include "Ark/Math/Functions.hpp"
#include "Ark/Math/Vector3.hpp"
#include "Ark/Math/Vector4.hpp"

namespace Ark
{
    Color Color::fromColor32(Color32 const& value)
    {
        return {
            value.r / 255.0f,
            value.g / 255.0f,
            value.b / 255.0f,
            value.a / 255.0f};
    }

    Color::Color(Math::Vector3 const& vector, float32 a)
        : r{vector.x}
        , g{vector.y}
        , b{vector.z}
        , a{a}
    {
    }

    Color::Color(Math::Vector4 const& vector)
        : r{vector.x}
        , g{vector.y}
        , b{vector.z}
        , a{vector.w}
    {
    }

    Color32 Color::toUInt8() const
    {
        return {
            static_cast<uint8>(r * 255.0f),
            static_cast<uint8>(g * 255.0f),
            static_cast<uint8>(b * 255.0f),
            static_cast<uint8>(a * 255.0f),
        };
    }

    Math::Vector3 Color::toVector3() const
    {
        return {r, g, b};
    }

    Math::Vector4 Color::toVector4() const
    {
        return {r, g, b, a};
    }

    uint32 Color::toRgba() const
    {
        return toUInt8().toRgba();
    }

    uint32 Color::toArgb() const
    {
        return toUInt8().toArgb();
    }

    void Color::clamp()
    {
        r = Math::clamp(r, 0.0f, 1.0f);
        g = Math::clamp(g, 0.0f, 1.0f);
        b = Math::clamp(b, 0.0f, 1.0f);
        a = Math::clamp(a, 0.0f, 1.0f);
    }

    Color Color::clamped() const
    {
        Color result = *this;
        result.clamp();
        return result;
    }

    Color Color::min(Color const& a, Color const& b)
    {
        return {
            Math::min(a.r, b.r),
            Math::min(a.g, b.g),
            Math::min(a.b, b.b),
            Math::min(a.a, b.a),
        };
    }

    Color Color::max(Color const& a, Color const& b)
    {
        return {
            Math::max(a.r, b.r),
            Math::max(a.g, b.g),
            Math::max(a.b, b.b),
            Math::max(a.a, b.a),
        };
    }

    Color Color::lerp(Color const& start, Color const& end, float32 amount)
    {
        return {
            Math::lerp(start.r, end.r, amount),
            Math::lerp(start.g, end.g, amount),
            Math::lerp(start.b, end.b, amount),
            Math::lerp(start.a, end.a, amount),
        };
    }

    float Color::getLuma() const
    {
        return 0.212f * r + 0.701f * g + 0.087f * b;
    }

    Color Color::applyGamma(float32 gamma) const
    {
        return {
            Math::pow(r, gamma),
            Math::pow(g, gamma),
            Math::pow(b, gamma),
            a,
        };
    }

    Color Color::fromHSV(float32 h, float32 s, float32 v, float32 a)
    {
        float32 r = 0;
        float32 g = 0;
        float32 b = 0;

        if (s == 0)
        {
            r = Math::clamp(v, 0.0f, 1.0f);
            g = b = r;
        }
        else
        {
            h = Math::mod(h, 1.0f);
            int32 hi = int32(h * 6.0f);

            float f = h * 6 - hi;
            float p = v * (1 - s);
            float q = v * (1 - f * s);
            float t = v * (1 - (1 - f) * s);

            switch (hi)
            {
                case 0:
                    r = v;
                    g = t;
                    b = p;
                    break;

                case 1:
                    r = q;
                    g = v;
                    b = p;
                    break;

                case 2:
                    r = p;
                    g = v;
                    b = t;
                    break;

                case 3:
                    r = p;
                    g = q;
                    b = v;
                    break;

                case 4:
                    r = t;
                    g = p;
                    b = v;
                    break;

                case 5:
                    r = v;
                    g = p;
                    b = q;
                    break;
            }
        }

        return {r, g, b, a};
    }

    Math::Vector3 Color::toHSV(float32 defaultHue, float32 defaultSaturation) const
    {
        float32 const ma = Math::max(r, Math::max(g, b));
        float32 const mi = Math::min(r, Math::min(g, b));
        float32 const c = ma - mi;

        float hp = defaultHue * 6;
        if (c > Math::ZeroTolerance<float32>)
        {
            if (Math::abs(ma - r) < Math::ZeroTolerance<float32>)
            {
                hp = Math::modulo((g - b) / c, 6.0f);
            }
            else if (Math::abs(ma - g) < Math::ZeroTolerance<float32>)
            {
                hp = (b - r) / c + 2;
            }
            else
            {
                hp = (r - g) / c + 4;
            }
        }

        float32 const h = hp / 6.0f;
        float32 const v = ma;
        float32 const s = v > Math::ZeroTolerance<float32> ? c / v : defaultSaturation;

        return {h, s, v};
    }

    Color Color::operator-() const
    {
        return {
            -r,
            -g,
            -b,
            -a,
        };
    }

    Color Color::operator+(Color const& color) const
    {
        return {
            r + color.r,
            g + color.g,
            b + color.b,
            a + color.a,
        };
    }

    Color Color::operator+(float32 color) const
    {
        return {
            r + color,
            g + color,
            b + color,
            a + color,
        };
    }

    Color& Color::operator+=(Color const& color)
    {
        r += color.r;
        g += color.g;
        b += color.b;
        a += color.a;
        return *this;
    }

    Color& Color::operator+=(float32 color)
    {
        r += color;
        g += color;
        b += color;
        a += color;
        return *this;
    }

    Color Color::operator-(Color const& color) const
    {
        return {
            r - color.r,
            g - color.g,
            b - color.b,
            a - color.a,
        };
    }

    Color Color::operator-(float32 color) const
    {
        return {
            r - color,
            g - color,
            b - color,
            a - color,
        };
    }

    Color& Color::operator-=(Color const& color)
    {
        r -= color.r;
        g -= color.g;
        b -= color.b;
        a -= color.a;
        return *this;
    }

    Color& Color::operator-=(float32 color)
    {
        r -= color;
        g -= color;
        b -= color;
        a -= color;
        return *this;
    }

    Color Color::operator*(Color const& color) const
    {
        return {
            r * color.r,
            g * color.g,
            b * color.b,
            a * color.a,
        };
    }

    Color Color::operator*(float32 color) const
    {
        return {
            r * color,
            g * color,
            b * color,
            a * color,
        };
    }

    Color& Color::operator*=(Color const& color)
    {
        r *= color.r;
        g *= color.g;
        b *= color.b;
        a *= color.a;
        return *this;
    }

    Color& Color::operator*=(float32 color)
    {
        r *= color;
        g *= color;
        b *= color;
        a *= color;
        return *this;
    }

    Color Color::operator/(Color const& color) const
    {
        return {
            r / color.r,
            g / color.g,
            b / color.b,
            a / color.a,
        };
    }

    Color Color::operator/(float32 color) const
    {
        return {
            r / color,
            g / color,
            b / color,
            a / color,
        };
    }

    Color& Color::operator/=(Color const& color)
    {
        r /= color.r;
        g /= color.g;
        b /= color.b;
        a /= color.a;
        return *this;
    }

    Color& Color::operator/=(float32 color)
    {
        r /= color;
        g /= color;
        b /= color;
        a /= color;
        return *this;
    }
}
