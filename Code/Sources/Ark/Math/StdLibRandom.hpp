#pragma once

#include "Ark/Core/Configuration.hpp"

#if (ARK_USE_STANDARD_LIBRARY == ARK_STANDARD_LIBRARY_CPP) || (ARK_USE_STANDARD_LIBRARY == ARK_STANDARD_LIBRARY_C)

#include "Ark/Math/Random.hpp"

#include <random>

namespace Ark::Math
{
    /// Standard library implementation of the Random interface using std::mt19937_64.
    class StdLibRandom final : public Random
    {
    public:
        /// Constructs a StdLibRandom generator seeded from a non-deterministic source.
        StdLibRandom();

        /// Constructs a StdLibRandom generator with the given seed.
        /// @param seed The seed value.
        explicit StdLibRandom(uint64 seed);

        ~StdLibRandom() override = default;

        /// @copydoc Random::nextUInt32()
        uint32 nextUInt32() override;

        /// @copydoc Random::nextUInt32(uint32,uint32)
        uint32 nextUInt32(uint32 min, uint32 max) override;

        /// @copydoc Random::nextInt32()
        int32 nextInt32() override;

        /// @copydoc Random::nextInt32(int32,int32)
        int32 nextInt32(int32 min, int32 max) override;

        /// @copydoc Random::nextUInt64()
        uint64 nextUInt64() override;

        /// @copydoc Random::nextUInt64(uint64,uint64)
        uint64 nextUInt64(uint64 min, uint64 max) override;

        /// @copydoc Random::nextInt64()
        int64 nextInt64() override;

        /// @copydoc Random::nextInt64(int64,int64)
        int64 nextInt64(int64 min, int64 max) override;

        /// @copydoc Random::nextFloat32()
        float32 nextFloat32() override;

        /// @copydoc Random::nextFloat32(float32,float32)
        float32 nextFloat32(float32 min, float32 max) override;

        /// @copydoc Random::nextFloat64()
        float64 nextFloat64() override;

        /// @copydoc Random::nextFloat64(float64,float64)
        float64 nextFloat64(float64 min, float64 max) override;

        /// @copydoc Random::nextBool()
        bool nextBool() override;

        /// @copydoc Random::setSeed(uint64)
        void setSeed(uint64 seed) override;

    private:
        std::mt19937_64 generator;
    };
}

#endif
