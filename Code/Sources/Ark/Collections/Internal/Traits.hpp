#pragma once

#include "Ark/Core/Concepts.hpp"

namespace Ark::Collections::Internal
{
    /// Marker tag for input iterators.
    struct InputIteratorTag
    {
    };

    /// Marker tag for output iterators.
    struct OutputIteratorTag
    {
    };

    /// Marker tag for forward iterators.
    struct ForwardIteratorTag : InputIteratorTag
    {
    };

    /// Marker tag for bidirectional iterators.
    struct BidirectionalIteratorTag : ForwardIteratorTag
    {
    };

    /// Marker tag for random-access iterators.
    struct RandomAccessIteratorTag : BidirectionalIteratorTag
    {
    };

    /// Marker tag for contiguous iterators.
    struct ContiguousIteratorTag : RandomAccessIteratorTag
    {
    };
}
