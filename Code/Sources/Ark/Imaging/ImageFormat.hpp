#pragma once

#include "Ark/Imaging/Types.hpp"

namespace Ark::Imaging
{
    /// Image format description defining pixel layout and storage
    struct ImageFormat final
    {
    private:
        uint32 width;
        uint32 height;
        uint32 depth;

        ChannelLayout channelLayout;
        ChannelType channelType;

        ColorSpace colorSpace;
        AlphaMode alphaMode;

        ImageType type;

    public:
        /// Creates a default format (RGBA8 2D image)
        ImageFormat()
            : width(0)
            , height(0)
            , depth(1)
            , channelLayout(ChannelLayout::RGBA)
            , channelType(ChannelType::UInt8)
            , colorSpace(ColorSpace::sRGB)
            , alphaMode(AlphaMode::Straight)
            , type(ImageType::Image2D)
        {
        }

        /// Creates a format with specified dimensions and pixel layout
        ImageFormat(
            uint32 width,
            uint32 height,
            ChannelLayout channelLayout = ChannelLayout::RGBA,
            ChannelType channelType = ChannelType::UInt8,
            ColorSpace colorSpace = ColorSpace::sRGB,
            AlphaMode alphaMode = AlphaMode::Straight,
            ImageType type = ImageType::Image2D)
            : width(width)
            , height(height)
            , depth(1)
            , channelLayout(channelLayout)
            , channelType(channelType)
            , colorSpace(colorSpace)
            , alphaMode(alphaMode)
            , type(type)
        {
        }

        /// Creates a 3D image format
        ImageFormat(
            uint32 width,
            uint32 height,
            uint32 depth,
            ChannelLayout channelLayout = ChannelLayout::RGBA,
            ChannelType channelType = ChannelType::UInt8,
            ColorSpace colorSpace = ColorSpace::sRGB,
            AlphaMode alphaMode = AlphaMode::Straight)
            : width(width)
            , height(height)
            , depth(depth)
            , channelLayout(channelLayout)
            , channelType(channelType)
            , colorSpace(colorSpace)
            , alphaMode(alphaMode)
            , type(ImageType::Image3D)
        {
        }

        /// Creates a tiled image format
        static ImageFormat createTiled(
            uint32 width,
            uint32 height,
            uint32 tileWidth,
            uint32 tileHeight,
            ChannelLayout channelLayout = ChannelLayout::RGBA,
            ChannelType channelType = ChannelType::UInt8,
            ColorSpace colorSpace = ColorSpace::sRGB,
            AlphaMode alphaMode = AlphaMode::Straight)
        {
            ImageFormat format(width, height, channelLayout, channelType, colorSpace, alphaMode);
            return format;
        }

        /// Get the width of the image
        uint32 getWidth() const
        {
            return width;
        }

        /// Get the height of the image
        uint32 getHeight() const
        {
            return height;
        }

        /// Get the depth of the image (for 3D images)
        uint32 getDepth() const
        {
            return depth;
        }

        /// Get the channel layout
        ChannelLayout getChannelLayout() const
        {
            return channelLayout;
        }

        /// Get the channel type
        ChannelType getChannelType() const
        {
            return channelType;
        }

        /// Get the color space
        ColorSpace getColorSpace() const
        {
            return colorSpace;
        }

        /// Get the alpha mode
        AlphaMode getAlphaMode() const
        {
            return alphaMode;
        }

        /// Get the image type
        ImageType getType() const
        {
            return type;
        }

        /// Get the number of channels
        uint32 getChannelCount() const
        {
            switch (channelLayout)
            {
                case ChannelLayout::R:
                case ChannelLayout::Gray:
                    return 1;

                case ChannelLayout::RG:
                case ChannelLayout::GrayAlpha:
                    return 2;

                case ChannelLayout::RGB:
                case ChannelLayout::BGR:
                case ChannelLayout::YUV:
                    return 3;

                case ChannelLayout::RGBA:
                case ChannelLayout::BGRA:
                case ChannelLayout::CMYK:
                    return 4;

                default:
                    return 0;
            }
        }

        /// Get the bytes per channel
        uint32 getBytesPerChannel() const
        {
            switch (channelType)
            {
                case ChannelType::UInt8:
                case ChannelType::Int8:
                    return 1;

                case ChannelType::UInt16:
                case ChannelType::Int16:
                case ChannelType::Half:
                    return 2;

                case ChannelType::UInt32:
                case ChannelType::Int32:
                case ChannelType::Float:
                    return 4;

                case ChannelType::Double:
                    return 8;

                default:
                    return 0;
            }
        }

        /// Get the bytes per pixel
        uint32 getBytesPerPixel() const
        {
            return getChannelCount() * getBytesPerChannel();
        }

        /// Get the row pitch (bytes per row)
        uint32 getRowPitch() const
        {
            return width * getBytesPerPixel();
        }

        /// Get the slice pitch (bytes per depth slice)
        uint32 getSlicePitch() const
        {
            return height * getRowPitch();
        }

        /// Get the total bytes needed for the image data
        uint32 getTotalBytes() const
        {
            return depth * getSlicePitch();
        }

        /// Checks if the format is valid for image data
        bool isValid() const
        {
            if (width == 0 || height == 0 || depth == 0)
            {
                return false;
            }

            if (getChannelCount() == 0 || getBytesPerChannel() == 0)
            {
                return false;
            }

            return true;
        }
    };
}
