#include "Ark/Math/StdLibRandom.hpp"

#if (ARK_USE_STANDARD_LIBRARY == ARK_STANDARD_LIBRARY_CPP) || (ARK_USE_STANDARD_LIBRARY == ARK_STANDARD_LIBRARY_C)

#include <chrono>

namespace Ark::Math
{
    StdLibRandom::StdLibRandom()
    {
        uint64 seed = static_cast<uint64>(std::chrono::high_resolution_clock::now().time_since_epoch().count());
        generator.seed(seed);
    }

    StdLibRandom::StdLibRandom(uint64 seed)
    {
        generator.seed(seed);
    }

    uint32 StdLibRandom::nextUInt32()
    {
        return static_cast<uint32>(generator());
    }

    uint32 StdLibRandom::nextUInt32(uint32 min, uint32 max)
    {
        std::uniform_int_distribution<uint32> distribution(min, max);
        return distribution(generator);
    }

    int32 StdLibRandom::nextInt32()
    {
        return static_cast<int32>(generator());
    }

    int32 StdLibRandom::nextInt32(int32 min, int32 max)
    {
        std::uniform_int_distribution<int32> distribution(min, max);
        return distribution(generator);
    }

    uint64 StdLibRandom::nextUInt64()
    {
        return generator();
    }

    uint64 StdLibRandom::nextUInt64(uint64 min, uint64 max)
    {
        std::uniform_int_distribution<uint64> distribution(min, max);
        return distribution(generator);
    }

    int64 StdLibRandom::nextInt64()
    {
        return static_cast<int64>(generator());
    }

    int64 StdLibRandom::nextInt64(int64 min, int64 max)
    {
        std::uniform_int_distribution<int64> distribution(min, max);
        return distribution(generator);
    }

    float32 StdLibRandom::nextFloat32()
    {
        std::uniform_real_distribution<float32> distribution(0.0f, 1.0f);
        return distribution(generator);
    }

    float32 StdLibRandom::nextFloat32(float32 min, float32 max)
    {
        std::uniform_real_distribution<float32> distribution(min, max);
        return distribution(generator);
    }

    float64 StdLibRandom::nextFloat64()
    {
        std::uniform_real_distribution<float64> distribution(0.0, 1.0);
        return distribution(generator);
    }

    float64 StdLibRandom::nextFloat64(float64 min, float64 max)
    {
        std::uniform_real_distribution<float64> distribution(min, max);
        return distribution(generator);
    }

    bool StdLibRandom::nextBool()
    {
        std::uniform_int_distribution<int> distribution(0, 1);
        return distribution(generator) == 1;
    }

    void StdLibRandom::setSeed(uint64 seed)
    {
        generator.seed(seed);
    }
}

#endif
