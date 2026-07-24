#pragma once

#include "Ark/Collections/Array.hpp"
#include "Ark/Core/Types.hpp"
#include "Ark/Display/PixelFormat.hpp"

namespace Ark::Display
{
    /// Pixel surface buffer for rendering or display.
    struct Surface final
    {
        /// Surface width in pixels.
        uint32 width = 0;
        /// Surface height in pixels.
        uint32 height = 0;

        /// Pixel format (typically RGBA8).
        PixelFormat format = PixelFormat::R8G8B8A8;
        /// Bytes per row; 0 means tightly packed (width * bytes_per_pixel).
        uint32 strideBytes = 0;

        /// Pixel data buffer.
        Collections::Array<uint8> pixels;
    };
}
