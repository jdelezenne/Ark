#pragma once

#include "Ark/Core/Types.hpp"

namespace Ark::Compression
{
    struct GzipParams final
    {
        uint8 const* input;
        usize inputSize;
        uint8* output;
        usize outputCapacity;
        usize* outputSize;
    };

    bool gzipDecompress(GzipParams const& params);
}
