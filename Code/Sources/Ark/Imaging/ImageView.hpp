#pragma once

#include "Ark/Core/Result.hpp"
#include "Ark/Imaging/ImageFormat.hpp"
#include "Ark/Math/Rect.hpp"

namespace Ark::Imaging
{
    /// An immutable view of image data
    struct ImageView final
    {
    private:
        ImageFormat format;
        void const* data;

    public:
        /// Creates an empty view
        ImageView()
            : data(nullptr)
        {
        }

        /// Creates a view of image data with a format
        ImageView(ImageFormat const& format, void const* data)
            : format(format)
            , data(data)
        {
        }

        /// Gets the format of the image data
        ImageFormat const& getFormat() const
        {
            return format;
        }

        /// Gets the raw data pointer
        void const* getData() const
        {
            return data;
        }

        /// Gets a subview
        ImageView subview(RectUInt const& bounds) const;

        /// Gets a pixel value as a typed array
        template <typename T>
        Result<T, Error> getPixel(uint32 x, uint32 y, uint32 z = 0) const;

        /// Gets a tile for tiled images
        Result<ImageView, Error> getTile(uint32 tileX, uint32 tileY, uint32 tileZ = 0) const;

        /// Checks if the view is valid
        bool isValid() const
        {
            return data != nullptr && format.isValid();
        }
    };

    /// Mutable view of image data
    class MutableImageView
    {
    private:
        ImageFormat format;
        void* data;

    public:
        /// Creates an empty mutable view
        MutableImageView()
            : data(nullptr)
        {
        }

        /// Creates a mutable view of image data with a format
        MutableImageView(ImageFormat const& format, void* data)
            : format(format)
            , data(data)
        {
        }

        /// Gets the format of the image data
        ImageFormat const& getFormat() const
        {
            return format;
        }

        /// Gets the raw data pointer
        void* getData() const
        {
            return data;
        }

        /// Gets an immutable view of the same data
        ImageView asView() const
        {
            return ImageView(format, data);
        }

        /// Gets a mutable subview (region of interest)
        MutableImageView subview(RectUInt const& roi) const;

        /// Gets a pixel value as a typed array
        template <typename T>
        Result<T, Error> getPixel(uint32 x, uint32 y, uint32 z = 0) const;

        /// Sets a pixel value from a typed array
        template <typename T>
        Result<Void, Error> setPixel(uint32 x, uint32 y, T const& values, uint32 z = 0);

        /// Checks if the view is valid
        bool isValid() const
        {
            return data != nullptr && format.isValid();
        }
    };
}
