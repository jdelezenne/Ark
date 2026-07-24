#include "Ark/Compression/Gzip.hpp"

#include <zlib.h>

namespace Ark::Compression
{
    bool gzipDecompress(GzipParams const& params)
    {
        if (params.input == nullptr || params.output == nullptr || params.outputSize == nullptr)
        {
            return false;
        }

        z_stream strm{};
        strm.next_in = const_cast<Bytef*>(reinterpret_cast<Bytef const*>(params.input));
        strm.avail_in = static_cast<uInt>(params.inputSize);
        strm.next_out = reinterpret_cast<Bytef*>(params.output);
        strm.avail_out = static_cast<uInt>(params.outputCapacity);

        int windowBits = MAX_WBITS + 16;
        int ret = inflateInit2(&strm, windowBits);
        if (ret != Z_OK)
        {
            return false;
        }

        ret = inflate(&strm, Z_FINISH);
        inflateEnd(&strm);
        if (ret != Z_STREAM_END)
        {
            return false;
        }
        *params.outputSize = static_cast<usize>(strm.total_out);
        return true;
    }
}
