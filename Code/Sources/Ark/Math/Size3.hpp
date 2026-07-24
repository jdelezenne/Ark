#pragma once

#include "Ark/Core/Concepts.hpp"
#include "Ark/Math/Types.hpp"

namespace Ark
{
    /// A structure that contains width, height and depth values.
    template <Concepts::Arithmetic T>
    struct Size3 final
    {
        union
        {
            struct
            {
                T width;  ///< The width of the size.
                T height; ///< The height of the size.
                T depth;  ///< The depth of the size.
            };

            T data[3];
        };

        /// Gets a special value that represents an empty size.
        static const Size3 empty;

        Size3() = default;

        /// Constructs a size with the given width, height, and depth.
        /// @param width The width value.
        /// @param height The height value.
        /// @param depth The depth value.
        constexpr Size3(T width, T height, T depth)
            : width{width}
            , height{height}
            , depth{depth}
        {
        }

        /// Gets a value that indicates whether the size is empty.
        /// @return True if all dimensions equal the empty size, false otherwise.
        constexpr bool isEmpty() const
        {
            return *this == empty;
        }

        /// Gets a value that indicates whether the size has non-negative dimensions.
        /// @return True if width, height, and depth are all >= 0, false otherwise.
        constexpr bool isValid() const
        {
            return (width >= 0.0f) && (height >= 0.0f) && (depth >= 0.0f);
        }

        constexpr T& operator[](int index)
        {
            return data[index];
        }

        constexpr T const& operator[](int index) const
        {
            return data[index];
        }

        constexpr bool operator==(Size3 const& size) const
        {
            return (width == size.width && height == size.height && depth == size.depth);
        }

        constexpr bool operator!=(Size3 const& size) const
        {
            return (width != size.width || height != size.height || depth != size.depth);
        }

        Size3<T> operator+(Size3<T> const& size) const
        {
            return {
                width + size.width,
                height + size.height,
                depth + size.depth,
            };
        }

        void operator+=(Size3<T> const& size)
        {
            width += size.width;
            height += size.height;
            depth += size.depth;
        }

        Size3<T> operator-(Size3<T> const& size) const
        {
            return {
                width - size.width,
                height - size.height,
                depth - size.depth,
            };
        }

        void operator-=(Size3<T> const& size)
        {
            width -= size.width;
            height -= size.height;
            depth -= size.depth;
        }
    };

    template <Concepts::Arithmetic T>
    const Size3<T> Size3<T>::empty = {T(0), T(0), T(0)};

    using Size3Float = Size3<float32>;
    using Size3Int = Size3<int32>;
    using Size3UInt = Size3<uint32>;
}
