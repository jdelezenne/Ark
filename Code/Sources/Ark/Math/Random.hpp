#pragma once

#include "Ark/Math/Types.hpp"

namespace Ark::Math
{
    /// Abstract interface for random number generation.
    class Random
    {
    public:
        virtual ~Random() = default;

        /// Generates a random 32-bit unsigned integer.
        /// @return A random uint32 value.
        virtual uint32 nextUInt32() = 0;

        /// Generates a random 32-bit unsigned integer within the specified range [min, max].
        /// @param min The minimum value (inclusive).
        /// @param max The maximum value (inclusive).
        /// @return A random uint32 value within the specified range.
        virtual uint32 nextUInt32(uint32 min, uint32 max) = 0;

        /// Generates a random 32-bit signed integer.
        /// @return A random int32 value.
        virtual int32 nextInt32() = 0;

        /// Generates a random 32-bit signed integer within the specified range [min, max].
        /// @param min The minimum value (inclusive).
        /// @param max The maximum value (inclusive).
        /// @return A random int32 value within the specified range.
        virtual int32 nextInt32(int32 min, int32 max) = 0;

        /// Generates a random 64-bit unsigned integer.
        /// @return A random uint64 value.
        virtual uint64 nextUInt64() = 0;

        /// Generates a random 64-bit unsigned integer within the specified range [min, max].
        /// @param min The minimum value (inclusive).
        /// @param max The maximum value (inclusive).
        /// @return A random uint64 value within the specified range.
        virtual uint64 nextUInt64(uint64 min, uint64 max) = 0;

        /// Generates a random 64-bit signed integer.
        /// @return A random int64 value.
        virtual int64 nextInt64() = 0;

        /// Generates a random 64-bit signed integer within the specified range [min, max].
        /// @param min The minimum value (inclusive).
        /// @param max The maximum value (inclusive).
        /// @return A random int64 value within the specified range.
        virtual int64 nextInt64(int64 min, int64 max) = 0;

        /// Generates a random 32-bit floating-point value in the range [0.0, 1.0).
        /// @return A random float32 value.
        virtual float32 nextFloat32() = 0;

        /// Generates a random 32-bit floating-point value within the specified range [min, max).
        /// @param min The minimum value (inclusive).
        /// @param max The maximum value (exclusive).
        /// @return A random float32 value within the specified range.
        virtual float32 nextFloat32(float32 min, float32 max) = 0;

        /// Generates a random 64-bit floating-point value in the range [0.0, 1.0).
        /// @return A random float64 value.
        virtual float64 nextFloat64() = 0;

        /// Generates a random 64-bit floating-point value within the specified range [min, max).
        /// @param min The minimum value (inclusive).
        /// @param max The maximum value (exclusive).
        /// @return A random float64 value within the specified range.
        virtual float64 nextFloat64(float64 min, float64 max) = 0;

        /// Generates a random boolean value.
        /// @return A random boolean value (true or false).
        virtual bool nextBool() = 0;

        /// Sets the seed for the random number generator.
        /// @param seed The seed value to initialize the generator.
        virtual void setSeed(uint64 seed) = 0;
    };
}
