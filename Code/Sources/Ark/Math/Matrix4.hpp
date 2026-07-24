#pragma once

#include "Ark/Core/Result.hpp"
#include "Ark/Math/Types.hpp"
#include "Ark/Math/Vector3.hpp"
#include "Ark/Math/Vector4.hpp"

namespace Ark::Math
{
    /// A 4x4 matrix for 3D transformations (translation, rotation, scaling, and projection).
    /// Supports column-major storage with multiple access patterns.
    struct Matrix4
    {
        union
        {
            struct
            {
                float32 m11;
                float32 m12;
                float32 m13;
                float32 m14;
                float32 m21;
                float32 m22;
                float32 m23;
                float32 m24;
                float32 m31;
                float32 m32;
                float32 m33;
                float32 m34;
                float32 m41;
                float32 m42;
                float32 m43;
                float32 m44;
            };

            float32 m[4][4];
            float32 values[16];
        };

        static const Matrix4 Zero;

        static const Matrix4 Identity;

        constexpr Matrix4()
        {
        }

        /// Constructs a matrix from individual element values.
        /// @param m11 Element at row 1, column 1.
        /// @param m12 Element at row 1, column 2.
        /// @param m13 Element at row 1, column 3.
        /// @param m14 Element at row 1, column 4.
        /// @param m21 Element at row 2, column 1.
        /// @param m22 Element at row 2, column 2.
        /// @param m23 Element at row 2, column 3.
        /// @param m24 Element at row 2, column 4.
        /// @param m31 Element at row 3, column 1.
        /// @param m32 Element at row 3, column 2.
        /// @param m33 Element at row 3, column 3.
        /// @param m34 Element at row 3, column 4.
        /// @param m41 Element at row 4, column 1.
        /// @param m42 Element at row 4, column 2.
        /// @param m43 Element at row 4, column 3.
        /// @param m44 Element at row 4, column 4.
        constexpr Matrix4(float32 m11, float32 m12, float32 m13, float32 m14, float32 m21, float32 m22, float32 m23, float32 m24, float32 m31, float32 m32, float32 m33, float32 m34, float32 m41, float32 m42, float32 m43, float32 m44)
            : m11{m11}
            , m12{m12}
            , m13{m13}
            , m14{m14}
            , m21{m21}
            , m22{m22}
            , m23{m23}
            , m24{m24}
            , m31{m31}
            , m32{m32}
            , m33{m33}
            , m34{m34}
            , m41{m41}
            , m42{m42}
            , m43{m43}
            , m44{m44}
        {
        }

        /// Constructs a matrix from four column vectors.
        /// @param column0 The first column.
        /// @param column1 The second column.
        /// @param column2 The third column.
        /// @param column3 The fourth column.
        constexpr Matrix4(Vector4 const& column0, Vector4 const& column1, Vector4 const& column2, Vector4 const& column3)
            : m11{column0.x}
            , m12{column0.y}
            , m13{column0.z}
            , m14{column0.w}
            , m21{column1.x}
            , m22{column1.y}
            , m23{column1.z}
            , m24{column1.w}
            , m31{column2.x}
            , m32{column2.y}
            , m33{column2.z}
            , m34{column2.w}
            , m41{column3.x}
            , m42{column3.y}
            , m43{column3.z}
            , m44{column3.w}
        {
        }

        /// Constructs a matrix from a 2D array of values.
        /// @param values A 4x4 array of values.
        inline Matrix4(float32 const values[4][4])
        {
            Memory::copy(values, this->values, sizeof(this->values));
        }

        /// Constructs a matrix from a flat array of 16 values in row-major order.
        /// @param values An array of 16 float32 values.
        inline Matrix4(float32 const values[16])
        {
            Memory::copy(values, this->values, sizeof(this->values));
        }

        /// Constructs a 4x4 matrix from a 3x3 matrix, embedding it in the upper-left with identity for other elements.
        /// @param matrix The 3x3 matrix to embed.
        explicit Matrix4(Matrix3 const& matrix);

        /// Checks if this matrix is the identity matrix.
        /// @return True if this matrix is identity, false otherwise.
        inline bool isIdentity() const
        {
            return *this == Identity;
        }

        /// Gets the column at the specified index as a Vector4.
        /// @param index The column index (0-3).
        /// @return The column vector.
        inline Vector4 getColumn(int index) const
        {
            return {m[0][index], m[1][index], m[2][index], m[3][index]};
        }

        /// Sets the column at the specified index.
        /// @param index The column index (0-3).
        /// @param column The new column vector.
        inline void setColumn(int index, Vector4 const& column)
        {
            m[0][index] = column.x;
            m[1][index] = column.y;
            m[2][index] = column.z;
            m[3][index] = column.w;
        }

        /// Gets the row at the specified index as a Vector4.
        /// @param index The row index (0-3).
        /// @return The row vector.
        inline Vector4 getRow(int index) const
        {
            return {m[index][0], m[index][1], m[index][2], m[index][3]};
        }

        /// Sets the row at the specified index.
        /// @param index The row index (0-3).
        /// @param row The new row vector.
        inline void setRow(int index, Vector4 const& row)
        {
            m[index][0] = row.x;
            m[index][1] = row.y;
            m[index][2] = row.z;
            m[index][3] = row.w;
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

        static Matrix4 transposed(Matrix4 const& matrix);

        void transpose();

        Matrix4 getTransposed() const;

        static Matrix4 inversed(Matrix4 const& matrix);

        Outcome inverse();

        Matrix4 getInversed() const;

        static Matrix4 translation(Vector3 const& translation);

        Vector3 getTranslation() const;

        void setTranslation(Vector3 const& translation);

        void translate(Vector3 const& position);

        static Matrix4 rotationXLH(float32 angle);
        static Matrix4 rotationXRH(float32 angle);
        static Matrix4 rotationX(float32 angle);

        static Matrix4 rotationYLH(float32 angle);
        static Matrix4 rotationYRH(float32 angle);
        static Matrix4 rotationY(float32 angle);

        static Matrix4 rotationZLH(float32 angle);
        static Matrix4 rotationZRH(float32 angle);
        static Matrix4 rotationZ(float32 angle);

        static Matrix4 rotationAxisLH(Vector3 const& axis, float32 angle);
        static Matrix4 rotationAxisRH(Vector3 const& axis, float32 angle);
        static Matrix4 rotationAxis(Vector3 const& axis, float32 angle);

        static Matrix4 scaling(Vector3 const& scale);

        static Matrix4 scaling(float32 scale);

        Vector3 getScale() const;

        void setScale(Vector3 const& scale);

        void scale(Vector3 const& scale);

        static Matrix4 orthographic(float32 width, float32 height, float32 zNear, float32 zFar);

        static Matrix4 orthographicOffCenter(float32 left, float32 right, float32 bottom, float32 top, float32 zNear, float32 zFar);

        static Matrix4 frustum(float32 left, float32 right, float32 bottom, float32 top, float32 zNear, float32 zFar);

        static Matrix4 perspectiveLH(float32 width, float32 height, float32 zNear, float32 zFar);
        static Matrix4 perspectiveRH(float32 width, float32 height, float32 zNear, float32 zFar);

        static Matrix4 perspectiveOffCenterLH(float32 left, float32 right, float32 bottom, float32 top, float32 zNear, float32 zFar);
        static Matrix4 perspectiveOffCenterRH(float32 left, float32 right, float32 bottom, float32 top, float32 zNear, float32 zFar);

        static Matrix4 perspectiveFieldOfViewLH(float32 fieldOfView, float32 aspectRatio, float32 zNear, float32 zFar);
        static Matrix4 perspectiveFieldOfViewRH(float32 fieldOfView, float32 aspectRatio, float32 zNear, float32 zFar);

        static Matrix4 perspective(float32 width, float32 height, float32 zNear, float32 zFar)
        {
            if constexpr (CurrentCoordinateSystem == CoordinateSystem::LeftHanded)
            {
                return perspectiveLH(width, height, zNear, zFar);
            }
            else
            {
                return perspectiveRH(width, height, zNear, zFar);
            }
        }

        static Matrix4 perspectiveOffCenter(float32 left, float32 right, float32 bottom, float32 top, float32 zNear, float32 zFar)
        {
            if constexpr (CurrentCoordinateSystem == CoordinateSystem::LeftHanded)
            {
                return perspectiveOffCenterLH(left, right, bottom, top, zNear, zFar);
            }
            else
            {
                return perspectiveOffCenterRH(left, right, bottom, top, zNear, zFar);
            }
        }

        static Matrix4 perspectiveFieldOfView(float32 fieldOfView, float32 aspectRatio, float32 zNear, float32 zFar)
        {
            if constexpr (CurrentCoordinateSystem == CoordinateSystem::LeftHanded)
            {
                return perspectiveFieldOfViewLH(fieldOfView, aspectRatio, zNear, zFar);
            }
            else
            {
                return perspectiveFieldOfViewRH(fieldOfView, aspectRatio, zNear, zFar);
            }
        }

        static Matrix4 lookAtLH(Vector3 const& eye, Vector3 const& target, Vector3 const& up);
        static Matrix4 lookAtRH(Vector3 const& eye, Vector3 const& target, Vector3 const& up);

        static Matrix4 lookAt(Vector3 const& eye, Vector3 const& target, Vector3 const& up)
        {
            if constexpr (CurrentCoordinateSystem == CoordinateSystem::LeftHanded)
            {
                return lookAtLH(eye, target, up);
            }
            else
            {
                return lookAtRH(eye, target, up);
            }
        }

        static Matrix4 rotation(Vector3 const& right, Vector3 const& up, Vector3 const& forward);

        // Helper function to extract Euler angles (XYZ order) from a rotation matrix
        static Vector3 toEulerAngles(Matrix4 const& rotationMatrix);

        static Matrix4 fromQuaternion(Quaternion const& q);

        static Matrix4 multiply(Matrix4 const& left, float32 right);

        void multiply(float32 scalar);

        static Matrix4 multiply(Matrix4 const& left, Matrix4 const& right);

        void multiply(Matrix4 const& other);

        /// Transforms a point by this matrix as a homogeneous position (w=1).
        /// Translation and projection terms are applied.
        static Vector4 transformPoint(Vector3 const& point, Matrix4 const& transform);

        Vector4 transformPoint(Vector3 const& point) const;

        /// Transforms a direction vector by this matrix as a non-homogeneous direction (w=0).
        /// Translation is ignored and no perspective divide is performed.
        static Vector3 transformVector(Vector3 const& vector, Matrix4 const& transform);

        Vector3 transformVector(Vector3 const& vector) const;

        /// Transforms a homogeneous vector by this matrix.
        static Vector4 transformVector(Vector4 const& vector, Matrix4 const& transform);

        Vector4 transformVector(Vector4 const& vector) const;

        bool isNearEqual(Matrix4 const& other) const;

        bool operator==(Matrix4 const& other) const;

        inline bool operator!=(Matrix4 const& other) const
        {
            return !operator==(other);
        }

        Matrix4 operator*(float32 scalar) const;

        Matrix4 operator*(Matrix4 const& other) const;

        Vector4 operator*(Vector4 const& v) const
        {
            return transformVector(v);
        }

        inline Matrix4& operator*=(float32 scalar)
        {
            *this = multiply(*this, scalar);
            return *this;
        }

        inline Matrix4& operator*=(Matrix4 const& other)
        {
            *this = multiply(*this, other);
            return *this;
        }
    };
}
