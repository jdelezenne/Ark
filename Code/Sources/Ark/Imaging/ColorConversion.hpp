#pragma once

#include "Ark/Collections/Array.hpp"
#include "Ark/Core/Result.hpp"
#include "Ark/Imaging/Types.hpp"

namespace Ark::Imaging
{
    /// Color conversion utilities
    namespace ColorConversion
    {
        /// Converts a color value between formats
        template <typename SourceType, typename DestinationType>
        Result<Ark::Collections::Array<DestinationType>, Error> convertColor(
            Ark::Collections::Array<SourceType> const& source,
            ChannelLayout sourceLayout,
            ChannelLayout destLayout);

        /// Converts between color spaces
        template <typename T>
        Result<Ark::Collections::Array<T>, Error> convertColorSpace(
            Ark::Collections::Array<T> const& source,
            ColorSpace sourceSpace,
            ColorSpace destSpace);
    }
}
