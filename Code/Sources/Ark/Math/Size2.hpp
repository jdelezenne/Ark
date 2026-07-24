#pragma once

#include "Ark/Core/Concepts.hpp"
#include "Ark/Math/Types.hpp"
#include "Ark/Math/Vector2.hpp"

namespace Ark
{
    /// A structure that contains width and height values.
    template <Concepts::Arithmetic T>
    struct Size2 final
    {
        /// Gets a special value that represents an empty size.
        static const Size2 empty;

        union
        {
            struct
            {
                T width;  ///< The width of the size.
                T height; ///< The height of the size.
            };

            T data[2];
        };

        Size2() = default;

        /// Constructs a size with the given width and height.
        /// @param width The width value.
        /// @param height The height value.
        constexpr Size2(T width, T height)
            : width{width}
            , height{height}
        {
        }

        /// Gets a value that indicates whether the size is empty.
        /// @return True if both dimensions equal the empty size, false otherwise.
        constexpr bool isEmpty() const
        {
            return *this == empty;
        }

        /// Gets a value that indicates whether the size has non-negative dimensions.
        /// @return True if both width and height are >= 0, false otherwise.
        constexpr bool isValid() const
        {
            return (width >= 0.0f) && (height >= 0.0f);
        }

        /// Converts this size to a Size2 with a different element type.
        /// @tparam U The target element type.
        /// @return A Size2<U> with statically cast values.
        template <Concepts::Arithmetic U>
        Size2<U> toSize()
        {
            return {
                static_cast<U>(width),
                static_cast<U>(height),
            };
        }

        /// Converts this size to a floating-point Vector2.
        /// @return A Vector2 with the width and height as components.
        Math::Vector2 toVector2() const
        {
            return Math::Vector2{
                static_cast<float32>(width),
                static_cast<float32>(height),
            };
        }

        constexpr T& operator[](int index)
        {
            return data[index];
        }

        constexpr T const& operator[](int index) const
        {
            return data[index];
        }

        constexpr bool operator==(Size2 const& size) const
        {
            return (width == size.width && height == size.height);
        }

        constexpr bool operator!=(Size2 const& size) const
        {
            return (width != size.width || height != size.height);
        }

        Size2<T> operator+(Size2<T> const& size) const
        {
            return {
                width + size.width,
                height + size.height,
            };
        }

        void operator+=(Size2<T> const& size)
        {
            width += size.width;
            height += size.height;
        }

        Size2<T> operator-(Size2<T> const& size) const
        {
            return {
                width - size.width,
                height - size.height,
            };
        }

        void operator-=(Size2<T> const& size)
        {
            width -= size.width;
            height -= size.height;
        }

        Size2<T> operator*(T scalar) const
        {
            return {
                width * scalar,
                height * scalar,
            };
        }

        void operator*=(T scalar)
        {
            width *= scalar;
            height *= scalar;
        }

        Size2<T> operator/(T scalar) const
        {
            ARK_ASSERT(scalar != T(0));

            return {
                width / scalar,
                height / scalar,
            };
        }

        void operator/=(T scalar)
        {
            ARK_ASSERT(scalar != T(0));

            width /= scalar;
            height /= scalar;
        }
    };

    template <Concepts::Arithmetic T>
    const Size2<T> Size2<T>::empty = {T(0), T(0)};

    using Size2Float = Size2<float32>;
    using Size2Int = Size2<int32>;
    using Size2UInt = Size2<uint32>;

    using Size2f = Size2<float32>;
    using Size2i = Size2<int32>;
    using Size2ui = Size2<uint32>;

    template <Concepts::Arithmetic T>
    using Size = Size2<T>;

    using SizeFloat = Size<float32>;
    using SizeInt = Size<int32>;
    using SizeUInt = Size<uint32>;

    using Sizef = Size<float32>;
    using Sizei = Size<int32>;
    using Sizeui = Size<uint32>;
}
