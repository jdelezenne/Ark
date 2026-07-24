#include "Ark/Math/Color64.hpp"
#include "Ark/Math/Color.hpp"

namespace Ark
{
    Color Color64::toFloat() const
    {
        return {
            static_cast<float32>(r / 65535.0f),
            static_cast<float32>(g / 65535.0f),
            static_cast<float32>(b / 65535.0f),
            static_cast<float32>(a / 65535.0f),
        };
    }

    Color64 Color64::operator+(Color64 const& color) const
    {
        auto const clampedAdd = [](uint16 left, uint16 right) -> uint16
        {
            int const result = int(left) + int(right);
            return static_cast<uint16>(result < 65535 ? result : 65535);
        };

        return {
            clampedAdd(r, color.r),
            clampedAdd(g, color.g),
            clampedAdd(b, color.b),
            clampedAdd(a, color.a),
        };
    }

    Color64& Color64::operator+=(Color64 const& color)
    {
        auto const clampedAdd = [](uint16 left, uint16 right) -> uint16
        {
            int const result = int(left) + int(right);
            return static_cast<uint16>(result < 65535 ? result : 65535);
        };

        r = clampedAdd(r, color.r);
        g = clampedAdd(g, color.g);
        b = clampedAdd(b, color.b);
        a = clampedAdd(a, color.a);
        return *this;
    }

    Color64 Color64::operator-(Color64 const& color) const
    {
        auto const clampedSubtract = [](uint16 left, uint16 right) -> uint16
        {
            int const result = int(left) - int(right);
            return static_cast<uint16>(result > 0 ? result : 0);
        };

        return {
            clampedSubtract(r, color.r),
            clampedSubtract(g, color.g),
            clampedSubtract(b, color.b),
            clampedSubtract(a, color.a),
        };
    }

    Color64& Color64::operator-=(Color64 const& color)
    {
        auto const clampedSubtract = [](uint16 left, uint16 right) -> uint16
        {
            int const result = int(left) - int(right);
            return static_cast<uint16>(result > 0 ? result : 0);
        };

        r = clampedSubtract(r, color.r);
        g = clampedSubtract(g, color.g);
        b = clampedSubtract(b, color.b);
        a = clampedSubtract(a, color.a);
        return *this;
    }
}
