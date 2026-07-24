#include "Ark/Storage/Path.hpp"
#include "Ark/Strings/StringSlice.hpp"

#include <cctype>

namespace Ark::Storage
{
    static bool isSeparator(char c)
    {
        return c == '/' || c == '\\';
    }

    Path::Path(StringSlice path)
        : value(path)
    {
        toGenericInPlace(value);
    }

    String const& Path::string() const
    {
        return value;
    }

    String Path::nativeString() const
    {
#if defined(ARK_PLATFORM_WINDOWS)
        String s = value;
        for (char& c : s)
        {
            if (c == '/')
            {
                c = '\\';
            }
        }
        return s;
#else
        return value;
#endif
    }

    bool Path::isEmpty() const
    {
        return value.isEmpty();
    }

    Path Path::join(StringSlice segment) const
    {
        if (segment.isEmpty())
        {
            return *this;
        }

        String s = value;
        if (!s.isEmpty() && !isSeparator(s.getLast()))
        {
            s.append('/');
        }

        String seg(segment);
        toGenericInPlace(seg);
        if (!seg.isEmpty() && isSeparator(seg[0]))
        {
            seg.remove(0, 1);
        }

        s += seg;
        return Path(StringSlice(s));
    }

    Path Path::parent() const
    {
        if (value.isEmpty())
        {
            return Path();
        }

        String s = value;
        // remove trailing separators except root-only
        while (s.getLength() > 1 && isSeparator(s.getLast()))
        {
            s.removeLast();
        }

        auto position = s.findLast('/');
        if (position == String::InvalidIndex)
        {
            return Path();
        }
        if (position == 0)
        {
            return Path(StringSlice("/"));
        }
        StringSlice sub = s.substring(0, position);
        return Path(sub);
    }

    String Path::filename() const
    {
        if (value.isEmpty())
        {
            return String();
        }
        String s = value;
        while (s.getLength() > 1 && isSeparator(s.getLast()))
        {
            s.removeLast();
        }
        auto position = s.findLast('/');
        if (position == String::InvalidIndex)
        {
            return s;
        }
        StringSlice sub = s.substring(position + 1);
        return sub.toString();
    }

    String Path::stem() const
    {
        String name = filename();
        auto dot = name.findLast('.');
        if (dot == String::InvalidIndex || dot == 0)
        {
            return name;
        }

        return name.substring(0, dot).toString();
    }

    String Path::extension() const
    {
        String name = filename();
        auto dot = name.findLast('.');
        if (dot == String::InvalidIndex || dot == 0)
        {
            return String();
        }

        return name.substring(dot + 1).toString();
    }

    Path Path::normalized() const
    {
        String s = value;
        collapseSeparatorsInPlace(s);
        collapseDotsInPlace(s);

        if (s.isEmpty())
        {
            return Path();
        }

        return Path(StringSlice(s));
    }

    Path Path::withTrailingSeparator() const
    {
        if (value.isEmpty())
        {
            return *this;
        }

        if (isSeparator(value.getLast()))
        {
            return *this;
        }

        String s = value;
        s.append('/');
        return Path(StringSlice(s));
    }

    Path Path::withoutTrailingSeparator() const
    {
        if (value.isEmpty())
        {
            return *this;
        }

        String s = value;
        while (s.getLength() > 1 && isSeparator(s.getLast()))
        {
            s.removeLast();
        }

        return Path(StringSlice(s));
    }

    bool Path::operator==(Path const& other) const
    {
        return normalized().string() == other.normalized().string();
    }

    bool Path::operator!=(Path const& other) const
    {
        return !(*this == other);
    }

    bool Path::isAbsolute() const
    {
        if (value.isEmpty())
        {
            return false;
        }

#if defined(ARK_PLATFORM_WINDOWS)
        if (value.getLength() >= 3 && std::isalpha(static_cast<unsigned char>(value[0])) && value[1] == ':' && isSeparator(value[2]))
        {
            return true;
        }

        if (value.getLength() >= 2 && isSeparator(value[0]) && isSeparator(value[1]))
        {
            return true; // UNC path
        }

        return false;
#else
        return (!value.isEmpty() && value[0] == '/');
#endif
    }

    void Path::toGenericInPlace(String& s)
    {
        for (usize i = 0; i < s.getLength(); ++i)
        {
            char ch = s[i];
            if (ch == '\\')
            {
                s.set(i, '/');
            }
        }
    }

    void Path::collapseSeparatorsInPlace(String& s)
    {
        if (s.isEmpty())
        {
            return;
        }

        String result;
        result.reserve(s.getLength());
        bool previousSeparator = false;
        for (usize i = 0; i < s.getLength(); ++i)
        {
            char c = s[i];
            bool separator = (c == '/');
            if (separator)
            {
                if (!previousSeparator)
                {
                    result.append('/');
                }
            }
            else
            {
                result.append(c);
            }

            previousSeparator = separator;
        }

        s = result;
    }

    void Path::collapseDotsInPlace(String& s)
    {
        if (s.isEmpty())
        {
            return;
        }
        bool absolute = (!s.isEmpty() && s[0] == '/');
        bool hadTrailing = (!s.isEmpty() && s.getLast() == '/');

        String result;
        if (absolute)
        {
            result.append('/');
        }

        String segment;
        usize i = 0;
        while (i <= s.getLength())
        {
            if (i == s.getLength() || s[i] == '/')
            {
                if (segment.isEmpty() || segment == ".")
                {
                }
                else if (segment == "..")
                {
                    if (absolute)
                    {
                        // Backtrack one segment if possible
                        if (result.getLength() > 1)
                        {
                            // Remove trailing '/'
                            if (result.getLast() == '/')
                            {
                                result.removeLast();
                            }
                            auto slash = result.findLast('/');
                            if (slash == String::InvalidIndex)
                            {
                                result.clear();
                            }
                            else if (slash == 0)
                            {
                                result.resize(1);
                                result.set(0, '/');
                            }
                            else
                            {
                                result.truncate(slash);
                            }
                        }
                        // else at root, ignore
                    }
                    else
                    {
                        if (result.isEmpty())
                        {
                            result += segment;
                        }
                        else
                        {
                            // remove previous segment if exists, else append
                            if (result.getLast() == '/')
                            {
                                result.removeLast();
                            }
                            auto slash = result.findLast('/');
                            if (slash == String::InvalidIndex)
                            {
                                result.clear();
                            }
                            else
                            {
                                result.truncate(slash);
                            }
                        }
                    }
                }
                else
                {
                    if (!result.isEmpty() && result.getLast() != '/')
                    {
                        result.append('/');
                    }
                    result += segment;
                }

                segment.clear();
                i += 1;
                continue;
            }

            segment.append(s[i]);
            i += 1;
        }

        if (hadTrailing && !result.isEmpty() && result.getLast() != '/')
        {
            result.append('/');
        }

        s = result;
    }
}
