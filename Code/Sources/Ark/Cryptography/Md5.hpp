#pragma once

#include "Ark/Collections/InlineArray.hpp"
#include "Ark/Strings/String.hpp"

namespace Ark::Cryptography
{
    /// MD5 cryptographic hash algorithm.
    /// Computes MD5 message digest (128-bit hash).
    /// @note MD5 is cryptographically broken and should not be used for security purposes.
    class Md5 final
    {
    public:
        /// 128-bit MD5 digest output (16 bytes).
        using Digest = Collections::InlineArray<uint8, 16>;

    private:
        uint32 a;
        uint32 b;
        uint32 c;
        uint32 d;

        uint64 totalBits;
        uint8 buffer[64];
        usize bufferSize;
        bool finalized;
        Digest result;

        /// Constructs a new MD5 hasher.
        Md5();

        /// Resets the hasher to initial state.
        void reset();

        /// Updates the hash with data.
        /// @param data Input buffer.
        /// @param length Buffer size in bytes.
        void update(void const* data, usize length);

        /// Updates the hash with a string.
        /// @param data String to hash.
        void update(String const& data);

        /// Finalizes the hash computation.
        void finalize();

        /// Returns the computed digest.
        /// @return 16-byte MD5 hash.
        Digest digest() const;

        /// Returns the digest as a hex-encoded string.
        /// @return 32-character hex string.
        String hexDigest() const;

        /// Computes MD5 digest of a buffer in one call.
        /// @param data Input buffer.
        /// @param length Buffer size in bytes.
        /// @return 16-byte MD5 hash.
        static Digest compute(void const* data, usize length);

        /// Computes MD5 digest of a string in one call.
        /// @param data String to hash.
        /// @return 16-byte MD5 hash.
        static Digest compute(String const& data);

        /// Computes MD5 digest of a string and returns as hex string.
        /// @param data String to hash.
        /// @return 32-character hex string.
        static String hex(String const& data);

    private:
        void processBlock(uint8 const block[64]);
    };
}
