#pragma once

#include "Ark/Collections/Array.hpp"
#include "Ark/Imaging/ImageFormat.hpp"
#include "Ark/Imaging/ImageView.hpp"

namespace Ark::Imaging
{
    /// Single image level (base level or mipmap)
    struct ImageLevel final
    {
        friend struct Image;

    private:
        ImageFormat format;
        Collections::Array<uint8> data;

    public:
        /// Creates an empty image level
        ImageLevel() = default;

        /// Creates an image level with a format and data
        ImageLevel(ImageFormat const& format, Collections::Array<uint8>&& data)
            : format(format)
            , data(move(data))
        {
        }

        /// Creates an image level with a format and data
        ImageLevel(ImageFormat const& format, Collections::Array<uint8> const& data)
            : format(format)
            , data(data)
        {
        }

        /// Gets the format of the image level
        ImageFormat const& getFormat() const
        {
            return format;
        }

        /// Gets the raw data
        Collections::Array<uint8> const& getData() const
        {
            return data;
        }

        /// Gets a mutable reference to the raw data
        Collections::Array<uint8>& getData()
        {
            return data;
        }

        /// Gets an immutable view of the image data
        ImageView getView() const
        {
            return ImageView(format, data.isEmpty() ? nullptr : data.asPointer());
        }

        /// Gets a mutable view of the image data
        MutableImageView getMutableView()
        {
            return MutableImageView(format, data.isEmpty() ? nullptr : data.asPointer());
        }

        /// Gets a pixel value as a typed array
        template <typename T>
        Result<Collections::Array<T>, Error> getPixel(uint32 x, uint32 y, uint32 z = 0) const
        {
            return getView().getPixel<T>(x, y, z);
        }

        /// Sets a pixel value from a typed array
        template <typename T>
        Result<Void, Error> setPixel(uint32 x, uint32 y, Collections::Array<T> const& values, uint32 z = 0)
        {
            return getMutableView().setPixel<T>(x, y, values, z);
        }

        /// Checks if the level has valid data
        bool isValid() const
        {
            return format.isValid() && !data.isEmpty() && data.getByteSize() >= format.getTotalBytes();
        }

        /// Resizes the data buffer to match the format
        void resizeToFormat()
        {
            if (format.isValid())
            {
                data.resize(format.getTotalBytes());
            }
        }
    };

    /// Core image class with data ownership
    struct Image final
    {
    private:
        ImageFormat baseFormat;
        Collections::Array<ImageLevel> levels;

    public:
        /// Creates an empty image
        Image() = default;

        /// Creates an image with a specified format
        explicit Image(ImageFormat const& format, bool allocateData = true)
            : baseFormat(format)
        {
            if (allocateData && format.isValid())
            {
                createEmpty(format);
            }
        }

        /// Creates an image with a format and data
        Image(ImageFormat const& format, Collections::Array<uint8> const& data)
            : baseFormat(format)
        {
            createFromData(format, data);
        }

        /// Creates an image with a format and data
        Image(ImageFormat const& format, Collections::Array<uint8>&& data)
            : baseFormat(format)
        {
            createFromData(format, move(data));
        }

        /// Gets the base image format
        ImageFormat const& getFormat() const
        {
            return baseFormat;
        }

        /// Checks if the image has mipmaps
        bool hasMipmaps() const
        {
            return levels.getCount() > 1;
        }

        /// Gets the number of mipmap levels
        uint32 getLevelCount() const
        {
            return levels.getCount();
        }

        /// Gets an image level by index (0 = base level)
        const ImageLevel& getLevel(uint32 level = 0) const
        {
            return levels[level < levels.getCount() ? level : 0];
        }

        /// Gets a mutable image level by index (0 = base level)
        ImageLevel& getLevel(uint32 level = 0)
        {
            return levels[level < levels.getCount() ? level : 0];
        }

        /// Gets a view of the base level
        ImageView getView() const
        {
            return levels.isEmpty() ? ImageView() : levels[0].getView();
        }

        /// Gets a mutable view of the base level
        MutableImageView getMutableView()
        {
            return levels.isEmpty() ? MutableImageView() : levels[0].getMutableView();
        }

        /// Creates an empty image with the specified format
        Result<Void, Error> createEmpty(ImageFormat const& format)
        {
            if (!format.isValid())
            {
                return Error::InvalidFormat;
            }

            baseFormat = format;
            levels.clear();

            // Create base level
            ImageLevel baseLevel;
            baseLevel.format = format;
            baseLevel.resizeToFormat();

            levels.append(move(baseLevel));
            return makeResult<Error>();
        }

        /// Creates an image from existing data
        Result<Void, Error> createFromData(ImageFormat const& format, Collections::Array<uint8> const& data)
        {
            if (!format.isValid())
            {
                return Error::InvalidFormat;
            }

            if (data.getCount() < format.getTotalBytes())
            {
                return Error::InvalidData;
            }

            baseFormat = format;
            levels.clear();

            // Create base level
            ImageLevel baseLevel(format, data);
            levels.append(move(baseLevel));

            return makeResult<Error>();
        }

        /// Creates an image from existing data
        Result<Void, Error> createFromData(ImageFormat const& format, Collections::Array<uint8>&& data)
        {
            if (!format.isValid())
            {
                return Error::InvalidFormat;
            }

            if (data.getCount() < format.getTotalBytes())
            {
                return Error::InvalidData;
            }

            baseFormat = format;
            levels.clear();

            // Create base level
            ImageLevel baseLevel(format, move(data));
            levels.append(move(baseLevel));

            return makeResult<Error>();
        }

        /// Generates all mipmap levels
        Result<Void, Error> generateMipmaps();

        // Pixel access (proxies to the base level)
        template <typename T>
        Result<Collections::Array<T>, Error> getPixel(uint32 x, uint32 y, uint32 z = 0, uint level = 0) const
        {
            if (levels.isEmpty())
            {
                return Result<Collections::Array<T>, Error>(Ark::unexpectedResult, Error::InvalidData);
            }

            if (level >= levels.getCount())
            {
                return Result<Collections::Array<T>, Error>(Ark::unexpectedResult, Error::OutOfBounds);
            }

            return levels[level].getPixel<T>(x, y, z);
        }

        template <typename T>
        Result<Void, Error> setPixel(uint32 x, uint32 y, Collections::Array<T> const& values, uint32 z = 0, uint level = 0)
        {
            if (levels.isEmpty())
            {
                return Result<Void, Error>(Ark::unexpectedResult, Error::InvalidData);
            }

            if (level >= levels.getCount())
            {
                return Result<Void, Error>(Ark::unexpectedResult, Error::OutOfBounds);
            }

            return levels[level].setPixel<T>(x, y, values, z);
        }

        /// Checks if the image is valid
        bool isValid() const
        {
            return !levels.isEmpty() && levels[0].isValid();
        }
    };
}
