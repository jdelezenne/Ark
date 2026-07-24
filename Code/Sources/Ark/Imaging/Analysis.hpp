#pragma once

#include "Ark/Collections/Array.hpp"
#include "Ark/Core/Result.hpp"
#include "Ark/Imaging/Image.hpp"
#include "Ark/Imaging/Types.hpp"

namespace Ark::Imaging
{
    /// Image analysis utilities
    namespace Analysis
    {
        /// Calculates the histogram of an image
        Result<Ark::Collections::Array<uint32>, Error> calculateHistogram(
            Image const& image,
            uint32 channel = 0,
            uint32 bins = 256);

        /// Calculates the average color of an image
        template <typename T>
        Result<Ark::Collections::Array<T>, Error> calculateAverageColor(Image const& image);

        /// Image statistics structure
        struct ImageStatistics final
        {
            float32 minValue;
            float32 maxValue;
            float32 meanValue;
            float32 medianValue;
            float32 standardDeviation;
        };

        /// Calculates statistics for an image channel
        Result<ImageStatistics, Error> calculateStatistics(
            Image const& image,
            uint32 channel = 0);
    }
}
