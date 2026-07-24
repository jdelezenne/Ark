#include "Ark/Math/PcgRandom.hpp"

#include "Ark/Core/Debug/Assert.hpp"

namespace Ark::Math
{
    namespace
    {
        uint64 makeNondeterministicSeed()
        {
            static uint64 counter = 0x853c49e6748fea9bULL;
            counter = counter * 6364136223846793005ULL + 1442695040888963407ULL;
            uintptr const addr = reinterpret_cast<uintptr>(&counter);
            return counter ^ static_cast<uint64>(addr) ^ (static_cast<uint64>(addr) << 32);
        }
    }

    PcgRandom::PcgRandom()
        : state(0)
        , increment(0)
    {
        uint64 seed = makeNondeterministicSeed();
        initialize(seed, seed >> 1);
    }

    PcgRandom::PcgRandom(uint64 seed)
        : state(0)
        , increment(0)
    {
        initialize(seed, seed >> 1);
    }

    PcgRandom::PcgRandom(uint64 seedState, uint64 seedSequence)
        : state(0)
        , increment(0)
    {
        initialize(seedState, seedSequence);
    }

    void PcgRandom::initialize(uint64 seedState, uint64 seedSequence)
    {
        state = 0;
        increment = (seedSequence << 1) | 1;
        generate();
        state += seedState;
        generate();
    }

    uint32 PcgRandom::generate()
    {
        uint64 oldState = state;
        state = oldState * 6364136223846793005ULL + increment;

        uint32 xorshifted = static_cast<uint32>(((oldState >> 18) ^ oldState) >> 27);
        uint32 rot = static_cast<uint32>(oldState >> 59);

        return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
    }

    uint32 PcgRandom::nextUInt32()
    {
        return generate();
    }

    uint32 PcgRandom::nextUInt32(uint32 min, uint32 max)
    {
        ARK_ASSERT_MSG(min <= max, "Invalid range");

        uint64 const span = static_cast<uint64>(max) - static_cast<uint64>(min) + 1ULL;
        if (span == (1ULL << 32U))
        {
            return generate();
        }

        uint32 const range = static_cast<uint32>(span);
        uint32 const threshold = static_cast<uint32>(-range % range);
        uint32 value;

        do
        {
            value = generate();
        }
        while (value < threshold);

        return min + (value % range);
    }

    int32 PcgRandom::nextInt32()
    {
        return static_cast<int32>(generate());
    }

    int32 PcgRandom::nextInt32(int32 min, int32 max)
    {
        ARK_ASSERT_MSG(min <= max, "Invalid range");

        uint64 const span = static_cast<uint64>(max) - static_cast<uint64>(min) + 1ULL;
        if (span == (1ULL << 32U))
        {
            return static_cast<int32>(generate());
        }

        uint32 const range = static_cast<uint32>(span);
        uint32 const threshold = static_cast<uint32>(-range % range);
        uint32 value;

        do
        {
            value = generate();
        }
        while (value < threshold);

        return min + static_cast<int32>(value % range);
    }

    uint64 PcgRandom::nextUInt64()
    {
        uint64 high = static_cast<uint64>(generate());
        uint64 low = static_cast<uint64>(generate());
        return (high << 32) | low;
    }

    uint64 PcgRandom::nextUInt64(uint64 min, uint64 max)
    {
        ARK_ASSERT_MSG(min <= max, "Invalid range");

        uint64 const range = max - min + 1;
        if (range == 0)
        {
            return nextUInt64();
        }

        uint64 const threshold = static_cast<uint64>(-range % range);
        uint64 value;

        do
        {
            value = nextUInt64();
        }
        while (value < threshold);

        return min + (value % range);
    }

    int64 PcgRandom::nextInt64()
    {
        return static_cast<int64>(nextUInt64());
    }

    int64 PcgRandom::nextInt64(int64 min, int64 max)
    {
        ARK_ASSERT_MSG(min <= max, "Invalid range");

        uint64 const umin = static_cast<uint64>(min);
        uint64 const umax = static_cast<uint64>(max);
        uint64 const range = umax - umin + 1;
        if (range == 0)
        {
            return static_cast<int64>(nextUInt64());
        }

        uint64 const threshold = static_cast<uint64>(-range % range);
        uint64 value;

        do
        {
            value = nextUInt64();
        }
        while (value < threshold);

        return static_cast<int64>(umin + (value % range));
    }

    float32 PcgRandom::nextFloat32()
    {
        return static_cast<float32>(generate()) / static_cast<float32>(0x100000000ULL);
    }

    float32 PcgRandom::nextFloat32(float32 min, float32 max)
    {
        return min + nextFloat32() * (max - min);
    }

    float64 PcgRandom::nextFloat64()
    {
        uint64 value = nextUInt64();
        return static_cast<float64>(value) / static_cast<float64>(0xFFFFFFFFFFFFFFFFULL);
    }

    float64 PcgRandom::nextFloat64(float64 min, float64 max)
    {
        return min + nextFloat64() * (max - min);
    }

    bool PcgRandom::nextBool()
    {
        return (generate() & 1) == 1;
    }

    void PcgRandom::setSeed(uint64 seed)
    {
        initialize(seed, seed >> 1);
    }
}
