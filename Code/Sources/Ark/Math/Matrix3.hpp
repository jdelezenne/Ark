#pragma once

#include "Ark/Core/Result.hpp"
#include "Ark/Math/Types.hpp"
#include "Ark/Math/Vector2.hpp"
#include "Ark/Math/Vector3.hpp"

namespace Ark::Math
{
    /// A 3x3 matrix for 2D transformations (translation, rotation, scaling).
    /// Supports column-major storage with multiple access patterns.
    struct Matrix3
    {
        union
        {
            struct
            {
                float32 m11;
                float32 m12;
                float32 m13;
                float32 m21;
                float32 m22;
                float32 m23;
                float32 m31;
                float32 m32;
                float32 m33;
            };

            float32 m[3][3];
            float32 values[9];
        };

        static const Matrix3 Zero;

        static const Matrix3 Identity;

        constexpr Matrix3() = default;

        /// Constructs a matrix from individual element values.
        /// @param m11 Element at row 1, column 1.
        /// @param m12 Element at row 1, column 2.
        /// @param m13 Element at row 1, column 3.
        /// @param m21 Element at row 2, column 1.
        /// @param m22 Element at row 2, column 2.
        /// @param m23 Element at row 2, column 3.
        /// @param m31 Element at row 3, column 1.
        /// @param m32 Element at row 3, column 2.
        /// @param m33 Element at row 3, column 3.
        constexpr Matrix3(float32 m11, float32 m12, float32 m13, float32 m21, float32 m22, float32 m23, float32 m31, float32 m32, float32 m33)
            : m11{m11}
            , m12{m12}
            , m13{m13}
            , m21{m21}
            , m22{m22}
            , m23{m23}
            , m31{m31}
            , m32{m32}
            , m33{m33}
        {
        }

        /// Constructs a matrix from three column vectors.
        /// @param column0 The first column.
        /// @param column1 The second column.
        /// @param column2 The third column.
        constexpr Matrix3(Vector3 const& column0, Vector3 const& column1, Vector3 const& column2)
            : m11{column0.x}
            , m12{column0.y}
            , m13{column0.z}
            , m21{column1.x}
            , m22{column1.y}
            , m23{column1.z}
            , m31{column2.x}
            , m32{column2.y}
            , m33{column2.z}
        {
        }

        /// Constructs a matrix from a 2D array of values.
        /// @param values A 3x3 array of values.
        inline Matrix3(float32 const values[3][3])
        {
            Memory::copy(values, this->values, sizeof(this->values));
        }

        /// Constructs a matrix from a flat array of 9 values in row-major order.
        /// @param values An array of 9 float32 values.
        inline Matrix3(float32 const values[9])
        {
            Memory::copy(values, this->values, sizeof(this->values));
        }

        /// Constructs a 3x3 matrix from a 4x4 matrix by taking the upper-left 3x3 block.
        /// @param matrix The 4x4 matrix to extract from.
        explicit Matrix3(Matrix4 const& matrix);

        /// Checks if this matrix is the identity matrix.
        /// @return True if this matrix is identity, false otherwise.
        inline bool isIdentity() const
        {
            return *this == Identity;
        }

        /// Gets the column at the specified index as a Vector3.
        /// @param index The column index (0-2).
        /// @return The column vector.
        inline Vector3 getColumn(int index) const
        {
            return {m[0][index], m[1][index], m[2][index]};
        }

        /// Sets the column at the specified index.
        /// @param index The column index (0-2).
        /// @param column The new column vector.
        inline void setColumn(int index, Vector3 const& column)
        {
            m[0][index] = column.x;
            m[1][index] = column.y;
            m[2][index] = column.z;
        }

        /// Gets the row at the specified index as a Vector3.
        /// @param index The row index (0-2).
        /// @return The row vector.
        inline Vector3 getRow(int index) const
        {
            return {m[index][0], m[index][1], m[index][2]};
        }

        /// Sets the row at the specified index.
        /// @param index The row index (0-2).
        /// @param row The new row vector.
        inline void setRow(int index, Vector3 const& row)
        {
            m[index][0] = row.x;
            m[index][1] = row.y;
            m[index][2] = row.z;
        }

#pragma region Axes

        Vector3 getUp() const
        {
            return {m21, m22, m23};
        }

        void setUp(Vector3 const& value)
        {
            m21 = value.x;
            m22 = value.y;
            m23 = value.z;
        }

        Vector3 getDown() const
        {
            return {-m21, -m22, -m23};
        }

        void setDown(Vector3 const& value)
        {
            m21 = -value.x;
            m22 = -value.y;
            m23 = -value.z;
        }

        Vector3 getLeft() const
        {
            return {-m11, -m12, -m13};
        }

        void setLeft(Vector3 const& value)
        {
            m11 = -value.x;
            m12 = -value.y;
            m13 = -value.z;
        }

        Vector3 getRight() const
        {
            return {m11, m12, m13};
        }

        void setRight(Vector3 const& value)
        {
            m11 = value.x;
            m12 = value.y;
            m13 = value.z;
        }

        Vector3 getForward() const
        {
            return {m31, m32, m33};
        }

        void setForward(Vector3 const& value)
        {
            m31 = value.x;
            m32 = value.y;
            m33 = value.z;
        }

        Vector3 getBackward() const
        {
            return {-m31, -m32, -m33};
        }

        void setBackward(Vector3 const& value)
        {
            m31 = -value.x;
            m32 = -value.y;
            m33 = -value.z;
        }

#pragma endregion

        float32 getDeterminant() const;

        void transpose();

        Matrix3 getTransposed() const;

        Outcome inverse();

        Matrix3 getInversed() const;

        static Matrix3 translation(Vector2 const& translation);

        Vector2 getTranslation() const;

        void setTranslation(Vector2 const& translation);

        void translate(Vector2 const& position);

        static Matrix3 rotation(float32 angle);

        void setRotation(float32 angle);

        void rotate(float32 angle);

        static Matrix3 scaling(Vector2 const& scale);

        Vector2 getScale() const;

        void setScale(Vector2 const& scale);

        void scale(Vector2 const& scale);

        float32 getBaseRotation() const;
        void setBaseRotation(float32 angle);
        Vector2 getBaseScale() const;
        void setBaseScale(Vector2 const& scale);

        static Matrix3 orthographic(float32 width, float32 height);

        static Matrix3 orthographicOffCenter(float32 left, float32 right, float32 bottom, float32 top);

        static Matrix3 multiply(Matrix3 const& left, float32 right);

        void multiply(float32 scalar);

        static Matrix3 multiply(Matrix3 const& left, Matrix3 const& right);

        void multiply(Matrix3 const& other);

        static Vector2 transformPoint(Vector2 const& point, Matrix3 const& transform);

        Vector2 transformPoint(Vector2 const& point) const;

        static Vector2 transformVector(Vector2 const& vector, Matrix3 const& transform);

        Vector2 transformVector(Vector2 const& vector) const;

        bool isNearEqual(Matrix3 const& other) const;

        bool operator==(Matrix3 const& other) const;

        inline bool operator!=(Matrix3 const& other) const
        {
            return !operator==(other);
        }

        Matrix3 operator*(Matrix3 const& other) const;

        inline Matrix3& operator*=(Matrix3 const& other)
        {
            *this = *this * other;
            return *this;
        }
    };
}
