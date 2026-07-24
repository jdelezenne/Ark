#pragma once

#include "Ark/Core/Types.hpp"

namespace Ark::Compression
{
    struct Bzip2Params final
    {
        uint8 const* input;
        usize inputSize;
        uint8* output;
        usize outputCapacity;
        usize* outputSize;
    };

    bool bzip2Decompress(Bzip2Params const& params);
}
