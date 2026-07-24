#include "Ark/Math/Color32.hpp"
#include "Ark/Math/Color.hpp"
#include "Ark/Math/Vector3.hpp"
#include "Ark/Math/Vector4.hpp"

namespace Ark
{
    Color32 Color32::fromColor(Color const& value)
    {
        return {
            static_cast<uint8>(Math::clamp(value.r * 255.0f, 0.0f, 255.0f)),
            static_cast<uint8>(Math::clamp(value.g * 255.0f, 0.0f, 255.0f)),
            static_cast<uint8>(Math::clamp(value.b * 255.0f, 0.0f, 255.0f)),
            static_cast<uint8>(Math::clamp(value.a * 255.0f, 0.0f, 255.0f))};
    }

    Color Color32::toFloat() const
    {
        return {
            static_cast<float32>(r / 255.0f),
            static_cast<float32>(g / 255.0f),
            static_cast<float32>(b / 255.0f),
            static_cast<float32>(a / 255.0f),
        };
    }

    Math::Vector3 Color32::toVector3() const
    {
        return toFloat().toVector3();
    }

    Math::Vector4 Color32::toVector4() const
    {
        return toFloat().toVector4();
    }

    Color32 Color32::operator+(Color32 const& color) const
    {
        auto const clampedAdd = [](uint8 left, uint8 right) -> uint8
        {
            int const result = int(left) + int(right);
            return static_cast<uint8>(result < 255 ? result : 255);
        };

        return {
            clampedAdd(r, color.r),
            clampedAdd(g, color.g),
            clampedAdd(b, color.b),
            clampedAdd(a, color.a),
        };
    }

    Color32& Color32::operator+=(Color32 const& color)
    {
        auto const clampedAdd = [](uint8 left, uint8 right) -> uint8
        {
            int const result = int(left) + int(right);
            return static_cast<uint8>(result < 255 ? result : 255);
        };

        r = clampedAdd(r, color.r);
        g = clampedAdd(g, color.g);
        b = clampedAdd(b, color.b);
        a = clampedAdd(a, color.a);
        return *this;
    }

    Color32 Color32::operator-(Color32 const& color) const
    {
        auto const clampedSubtract = [](uint8 left, uint8 right) -> uint8
        {
            int const result = int(left) - int(right);
            return static_cast<uint8>(result > 0 ? result : 0);
        };

        return {
            clampedSubtract(r, color.r),
            clampedSubtract(g, color.g),
            clampedSubtract(b, color.b),
            clampedSubtract(a, color.a),
        };
    }

    Color32& Color32::operator-=(Color32 const& color)
    {
        auto const clampedSubtract = [](uint8 left, uint8 right) -> uint8
        {
            int const result = int(left) - int(right);
            return static_cast<uint8>(result > 0 ? result : 0);
        };

        r = clampedSubtract(r, color.r);
        g = clampedSubtract(g, color.g);
        b = clampedSubtract(b, color.b);
        a = clampedSubtract(a, color.a);
        return *this;
    }

    Color32 Color32::operator*(Color32 const& color) const
    {
        return {
            static_cast<uint8>((static_cast<uint32>(r) * color.r) / 255u),
            static_cast<uint8>((static_cast<uint32>(g) * color.g) / 255u),
            static_cast<uint8>((static_cast<uint32>(b) * color.b) / 255u),
            static_cast<uint8>((static_cast<uint32>(a) * color.a) / 255u),
        };
    }

    Color32& Color32::operator*=(Color32 const& color)
    {
        r = static_cast<uint8>((static_cast<uint32>(r) * color.r) / 255u);
        g = static_cast<uint8>((static_cast<uint32>(g) * color.g) / 255u);
        b = static_cast<uint8>((static_cast<uint32>(b) * color.b) / 255u);
        a = static_cast<uint8>((static_cast<uint32>(a) * color.a) / 255u);
        return *this;
    }
}
