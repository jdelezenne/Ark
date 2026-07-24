#include "Ark/Compression/Bzip2.hpp"

#include <bzlib.h>

namespace Ark::Compression
{
    bool bzip2Decompress(Bzip2Params const& params)
    {
        unsigned int destLen = static_cast<unsigned int>(params.outputCapacity);
        int ret = BZ2_bzBuffToBuffDecompress(reinterpret_cast<char*>(params.output), &destLen, const_cast<char*>(reinterpret_cast<char const*>(params.input)), static_cast<unsigned int>(params.inputSize), 0, 0);
        if (ret != BZ_OK)
        {
            return false;
        }
        *params.outputSize = static_cast<usize>(destLen);
        return true;
    }
}
