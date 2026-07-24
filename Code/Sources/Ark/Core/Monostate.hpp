#pragma once

namespace Ark
{
    struct Monostate final
    {
        friend constexpr bool operator==(Monostate, Monostate)
        {
            return true;
        }
    };
}
