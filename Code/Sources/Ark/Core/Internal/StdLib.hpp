#pragma once

namespace std
{
    template <typename Ep>
    class initializer_list;
}

namespace Ark
{
    /// Alias for `std::initializer_list` used by Ark APIs.
    /// @tparam Ep Element type.
    template <typename Ep>
    using InitializerList = std::initializer_list<Ep>;
}
