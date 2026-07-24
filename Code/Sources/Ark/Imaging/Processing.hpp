#pragma once

#include "Ark/Collections/Array.hpp"
#include "Ark/Core/Result.hpp"
#include "Ark/Imaging/Image.hpp"
#include "Ark/Imaging/Types.hpp"

namespace Ark::Imaging
{
    /// Collection of image processing and manipulation functions
    namespace Processing
    {
        /// Resizes an image
        Result<Image, Error> resize(
            Image const& image,
            uint32 newWidth,
            uint32 newHeight,
            uint32 newDepth = 0);

        /// Crops an image to a region of interest
        Result<Image, Error> crop(
            Image const& image,
            RectUInt const& roi);

        /// Converts an image to a different format
        Result<Image, Error> convert(
            Image const& image,
            ChannelLayout newChannelLayout,
            ChannelType newChannelType,
            ColorSpace newColorSpace = ColorSpace::sRGB,
            AlphaMode newAlphaMode = AlphaMode::Straight);

        /// Applies a filter to an image
        Result<Image, Error> applyFilter(
            Image const& image,
            const Ark::Collections::Array<float32>& kernel);

        /// Blends two images
        Result<Image, Error> blend(
            Image const& source,
            Image const& destination,
            float32 alpha);

        /// Rotates an image
        Result<Image, Error> rotate(
            Image const& image,
            float32 angleDegrees);

        /// Flips an image horizontally or vertically
        Result<Image, Error> flip(
            Image const& image,
            bool horizontal,
            bool vertical);

        /// Tone maps an HDR image to LDR
        Result<Image, Error> toneMap(
            Image const& image,
            float32 exposure = 1.0f,
            float32 gamma = 2.2f);
    }
}
