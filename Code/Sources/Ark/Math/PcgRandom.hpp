#pragma once

#include "Ark/Math/Random.hpp"

namespace Ark::Math
{
    /// PCG (Permuted Congruential Generator) implementation of the Random interface.
    /// PCG is a family of simple fast space-efficient statistically good algorithms for random number generation.
    /// This implementation uses PCG-XSH-RR variant with 64-bit state.
    class PcgRandom final : public Random
    {
    public:
        /// Constructs a PcgRandom generator seeded from a non-deterministic source.
        PcgRandom();

        /// Constructs a PcgRandom generator with the given seed.
        /// @param seed The seed value.
        explicit PcgRandom(uint64 seed);

        /// Constructs a PcgRandom generator with explicit state and sequence.
        /// @param state The initial state.
        /// @param sequence The sequence selector (stream id).
        PcgRandom(uint64 state, uint64 sequence);

        ~PcgRandom() override = default;

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
        uint64 state;
        uint64 increment;

        void initialize(uint64 seedState, uint64 seedSequence);

        uint32 generate();
    };
}
