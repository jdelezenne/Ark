#pragma once

#include "Ark/Core/Macros.hpp"
#include "Ark/Strings/Internal/Format.hpp"
#include "Ark/Strings/String.hpp"
#include "Ark/Strings/StringSlice.hpp"

namespace Ark::Storage
{
    /// Value type representing a filesystem path using forward slashes ('/') as separator.
    /// Paths are stored in generic (Unix-style) format internally. nativeString() converts to OS format on demand.
    /// Path operations are purely syntactic; they don't check filesystem. Use FileSystem for I/O operations.
    struct Path final
    {
    private:
        String value;

    public:
        ARK_STRUCT(Path)

        Path() = default;
        Path(StringSlice path);

        /// Returns the normalized, generic form (uses '/' internally).
        String const& string() const;

        /// Returns the platform-native form (uses '\\' on Windows, '/' on Unix-like systems).
        String nativeString() const;

        /// True if path is empty or only whitespace.
        bool isEmpty() const;

        /// Returns a new path with the segment appended, adding '/' separator if needed.
        Path join(StringSlice segment) const;

        /// Returns a new path with another path appended.
        Path join(Path const& segment) const
        {
            return join(segment.string());
        }

        /// Returns the directory portion of this path, or empty if no parent exists (e.g., "/" or "C:\\").
        Path parent() const;

        /// Returns the filename component (text after last '/'). Empty for root or paths ending in '/'.
        String filename() const;

        /// Returns the filename without its extension (everything before last '.' in filename).
        /// If no extension exists, returns the entire filename. "." and ".." are returned as-is.
        String stem() const;

        /// Returns the file extension including the dot (e.g., ".txt"), or empty if no extension exists.
        String extension() const;

        /// Returns a lexically normalized path (collapses '.', '..' and redundant separators).
        /// This is syntactic; it doesn't verify the path exists on the filesystem.
        Path normalized() const;

        /// Appends a trailing separator if missing (for directory display convenience).
        Path withTrailingSeparator() const;

        /// Removes any trailing separators (except for root paths like "/").
        Path withoutTrailingSeparator() const;

        /// Equality comparison on normalized generic strings. Case-sensitive on Unix, case-insensitive on Windows.
        bool operator==(Path const& other) const;
        bool operator!=(Path const& other) const;

        /// Returns true if the path appears to be absolute. Heuristic: starts with "/" or has Windows drive letter.
        bool isAbsolute() const;

        // Join operators
        Path operator/(StringSlice segment) const
        {
            return join(segment);
        }

        Path operator/(Path const& segment) const
        {
            return join(segment.string());
        }

    private:
        static void toGenericInPlace(String& s);
        static void collapseDotsInPlace(String& s);
        static void collapseSeparatorsInPlace(String& s);
    };

    // Convenience non-member join operators for String and StringSlice on the left-hand side
    inline Path operator/(String const& left, Path const& right)
    {
        return Path(StringSlice(left)) / right;
    }

    inline Path operator/(StringSlice left, Path const& right)
    {
        return Path(left) / right;
    }
}

template <>
struct ARK_FORMAT_NAMESPACE::formatter<Ark::Storage::Path, char>
{
    template <typename FormatContext>
    auto format(Ark::Storage::Path const& v, FormatContext& ctx) const
    {
        return ARK_FORMAT_NAMESPACE::format_to(ctx.out(), "{}", v.string());
    }

    constexpr auto parse(ARK_FORMAT_NAMESPACE::format_parse_context& ctx) -> decltype(ctx.begin())
    {
        return ctx.begin();
    }
};

namespace std
{
    template <>
    struct hash<Ark::Storage::Path>
    {
        size_t operator()(Ark::Storage::Path const& p) const
        {
            Ark::String const& s = p.normalized().string();
            return static_cast<size_t>(Ark::computeHashArray(s.asPointer(), s.getCount()));
        }
    };
}
