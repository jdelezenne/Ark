#pragma once

#include "Ark/Collections/InlineArray.hpp"
#include "Ark/Strings/String.hpp"

namespace Ark::Cryptography
{
    /// SHA-1 cryptographic hash algorithm.
    /// Computes SHA-1 message digest (160-bit hash).
    /// @note SHA-1 has known weaknesses and should be avoided for new applications.
    class Sha1 final
    {
    public:
        /// 160-bit SHA-1 digest output (20 bytes).
        using Digest = Collections::InlineArray<uint8, 20>;

    private:
        uint32 h0;
        uint32 h1;
        uint32 h2;
        uint32 h3;
        uint32 h4;

        uint64 totalBits;
        uint8 buffer[64];
        usize bufferSize;
        bool finalized;
        Digest result;

    public:
        /// Constructs a new SHA-1 hasher.
        Sha1();

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
        /// @return 20-byte SHA-1 hash.
        Digest digest() const;

        /// Returns the digest as a hex-encoded string.
        /// @return 40-character hex string.
        String hexDigest() const;

        /// Computes SHA-1 digest of a buffer in one call.
        /// @param data Input buffer.
        /// @param length Buffer size in bytes.
        /// @return 20-byte SHA-1 hash.
        static Digest compute(void const* data, usize length);

        /// Computes SHA-1 digest of a string in one call.
        /// @param data String to hash.
        /// @return 20-byte SHA-1 hash.
        static Digest compute(String const& data);

        /// Computes SHA-1 digest of a string and returns as hex string.
        /// @param data String to hash.
        /// @return 40-character hex string.
        static String hex(String const& data);

    private:
        void processBlock(uint8 const block[64]);
    };
}
