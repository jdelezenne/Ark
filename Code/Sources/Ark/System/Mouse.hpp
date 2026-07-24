#pragma once

#include "Ark/Core/Flags.hpp"
#include "Ark/Core/Types.hpp"

namespace Ark
{
    /// Mouse button identifiers.
    enum class MouseButton : uint32
    {
        Left = 1,
        Right = 2,
        Middle = 3,
        Button4 = 4,
        Button5 = 5,
    };

    /// Bitmask set of pressed mouse buttons.
    enum class MouseButtons : uint32
    {
        None = 0,
        Left = ARK_BIT(0),
        Right = ARK_BIT(1),
        Middle = ARK_BIT(2),
        Button4 = ARK_BIT(3),
        Button5 = ARK_BIT(4),
    };

    ARK_FLAG_ENUM(MouseButtons)

    /// Mouse wheel direction convention.
    enum class MouseWheelDirection : uint32
    {
        Normal = 0,
        Flipped = 1,
    };
}
