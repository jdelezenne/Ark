#pragma once

#include "Ark/Core/Types.hpp"

namespace Ark::Compression
{
    struct DeflateParams final
    {
        uint8 const* input;
        usize inputSize;
        uint8* output;
        usize outputCapacity;
        usize* outputSize;
        bool rawDeflate;
    };

    bool deflateDecompress(DeflateParams const& params);
}
