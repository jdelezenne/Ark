#pragma once

#include "Ark/Core/Types.hpp"

namespace Ark::Imaging
{
    /// Defines the type of data stored in a color channel
    enum class ChannelType
    {
        UInt8,  // 8-bit unsigned integer (0-255)
        UInt16, // 16-bit unsigned integer (0-65535)
        UInt32, // 32-bit unsigned integer
        Int8,   // 8-bit signed integer (-128 to 127)
        Int16,  // 16-bit signed integer (-32768 to 32767)
        Int32,  // 32-bit signed integer
        Half,   // 16-bit floating point (half precision)
        Float,  // 32-bit floating point (single precision)
        Double, // 64-bit floating point (float64 precision)
    };

    /// Defines the layout of color channels in an image
    enum class ChannelLayout
    {
        // Standard layouts
        R,    // Single red channel
        RG,   // Red and green channels
        RGB,  // Red, green, and blue channels
        RGBA, // Red, green, blue, and alpha channels
        BGR,  // Blue, green, and red channels
        BGRA, // Blue, green, red, and alpha channels

        // Specialized layouts
        Gray,      // Single grayscale channel
        GrayAlpha, // Grayscale and alpha channels
        CMYK,      // Cyan, magenta, yellow, and key (black) channels
        YUV,       // Luminance and chrominance channels

        // Custom layouts
        Custom, // Custom channel layout defined by metadata
    };

    /// Defines how the alpha channel is interpreted
    enum class AlphaMode
    {
        None,          // No alpha channel
        Straight,      // Straight alpha (non-premultiplied)
        Premultiplied, // Premultiplied alpha
    };

    /// Defines the color space of an image
    enum class ColorSpace
    {
        Linear, // Linear RGB
        sRGB,   // Standard RGB
    };

    /// Defines the type of image (2D, 3D, Cubemap, etc.)
    enum struct ImageType
    {
        Image2D,      // Standard 2D image
        Image3D,      // Volumetric 3D image
        Cubemap,      // Cubemap image
        Array,        // Array of images
        CubemapArray, // Array of cubemaps
        Deep          // Deep image (multiple samples per pixel)
    };

    /// Error codes for image operations
    enum class Error
    {
        // General errors
        None,
        InvalidParameter,
        OutOfMemory,
        OutOfBounds,
        NotImplemented,

        // Format-specific errors
        InvalidFormat,
        UnsupportedFormat,
        UnsupportedChannelType,
        UnsupportedChannelLayout,
        UnsupportedColorSpace,

        // File I/O errors
        FileNotFound,
        FileReadError,
        FileWriteError,
        InvalidData,

        // Operation errors
        OperationFailed,
        OperationNotSupported,
        DataTypeConversionFailed
    };
}
