#include "Ark/Documents/IniDocument.hpp"

#include <charconv>
#include <cstdlib>

namespace Ark::Documents
{
    IniDocument::IniDocument()
        : sections{}
        , comments{}
        , globalSection{}
    {
    }

    bool IniDocument::hasSection(StringSlice sectionName) const
    {
        if (sectionName.isEmpty())
        {
            return sections.contains(globalSection);
        }

        return sections.contains(String{sectionName});
    }

    bool IniDocument::hasKey(StringSlice sectionName, StringSlice key) const
    {
        String const section = sectionName.isEmpty() ? globalSection : String{sectionName};

        auto it = sections.find(section);
        if (it == sections.end())
        {
            return false;
        }

        return it->second.contains(String{key});
    }

    String IniDocument::getValue(StringSlice sectionName, StringSlice key, StringSlice defaultValue) const
    {
        String const section = sectionName.isEmpty() ? globalSection : String{sectionName};

        auto sectionIt = sections.find(section);
        if (sectionIt == sections.end())
        {
            return String{defaultValue};
        }

        auto keyIt = sectionIt->second.find(String{key});
        if (keyIt == sectionIt->second.end())
        {
            return String{defaultValue};
        }

        return keyIt->second;
    }

    Option<String> IniDocument::getValueOption(StringSlice sectionName, StringSlice key) const
    {
        String const section = sectionName.isEmpty() ? globalSection : String{sectionName};

        auto sectionIt = sections.find(section);
        if (sectionIt == sections.end())
        {
            return none;
        }

        auto keyIt = sectionIt->second.find(String{key});
        if (keyIt == sectionIt->second.end())
        {
            return none;
        }

        return Option<String>{keyIt->second};
    }

    int64 IniDocument::getInt(StringSlice sectionName, StringSlice key, int64 defaultValue) const
    {
        String const value = getValue(sectionName, key);
        if (value.isEmpty())
        {
            return defaultValue;
        }

        int64 result = 0;
        auto const [ptr, ec] = std::from_chars(value.asPointer(), value.asPointer() + value.getLength(), result);

        if (ec != std::errc{} || ptr != value.asPointer() + value.getLength())
        {
            return defaultValue;
        }

        return result;
    }

    uint64 IniDocument::getUInt(StringSlice sectionName, StringSlice key, uint64 defaultValue) const
    {
        String const value = getValue(sectionName, key);
        if (value.isEmpty())
        {
            return defaultValue;
        }

        uint64 result = 0;
        auto const [ptr, ec] = std::from_chars(value.asPointer(), value.asPointer() + value.getLength(), result);

        if (ec != std::errc{})
        {
            return defaultValue;
        }

        return result;
    }

    float64 IniDocument::getFloat(StringSlice sectionName, StringSlice key, float64 defaultValue) const
    {
        String const value = getValue(sectionName, key);
        if (value.isEmpty())
        {
            return defaultValue;
        }

        char* end = nullptr;
        float64 result = std::strtod(value.asPointer(), &end);

        if (end == value.asPointer() || *end != '\0')
        {
            return defaultValue;
        }

        return result;
    }

    bool IniDocument::getBool(StringSlice sectionName, StringSlice key, bool defaultValue) const
    {
        String const value = getValue(sectionName, key);
        if (value.isEmpty())
        {
            return defaultValue;
        }

        String const lower = value.toLowercase();

        if (lower == "true" || lower == "1" || lower == "yes" || lower == "on")
        {
            return true;
        }

        if (lower == "false" || lower == "0" || lower == "no" || lower == "off")
        {
            return false;
        }

        return defaultValue;
    }

    void IniDocument::setValue(StringSlice sectionName, StringSlice key, StringSlice value)
    {
        PropertyMap& section = getOrCreateSection(sectionName);
        section[String{key}] = value;
    }

    void IniDocument::setInt(StringSlice sectionName, StringSlice key, int64 value)
    {
        char buffer[32];
        auto const [ptr, ec] = std::to_chars(buffer, buffer + sizeof(buffer), value);

        if (ec == std::errc{})
        {
            setValue(sectionName, key, StringSlice{buffer, static_cast<usize>(ptr - buffer)});
        }
    }

    void IniDocument::setUInt(StringSlice sectionName, StringSlice key, uint64 value)
    {
        char buffer[32];
        auto const [ptr, ec] = std::to_chars(buffer, buffer + sizeof(buffer), value);

        if (ec == std::errc{})
        {
            setValue(sectionName, key, StringSlice{buffer, static_cast<usize>(ptr - buffer)});
        }
    }

    void IniDocument::setFloat(StringSlice sectionName, StringSlice key, float64 value)
    {
        char buffer[64];
        auto const [ptr, ec] = std::to_chars(buffer, buffer + sizeof(buffer), value);

        if (ec == std::errc{})
        {
            setValue(sectionName, key, StringSlice{buffer, static_cast<usize>(ptr - buffer)});
        }
    }

    void IniDocument::setBool(StringSlice sectionName, StringSlice key, bool value)
    {
        setValue(sectionName, key, value ? "true" : "false");
    }

    Result<Void, IniDocument::Error> IniDocument::removeKey(StringSlice sectionName, StringSlice key)
    {
        String const section = sectionName.isEmpty() ? globalSection : String{sectionName};

        auto sectionIt = sections.find(section);
        if (sectionIt == sections.end())
        {
            return Ark::Error{Error::SectionNotFound};
        }

        auto keyIt = sectionIt->second.find(String{key});
        if (keyIt == sectionIt->second.end())
        {
            return Ark::Error{Error::KeyNotFound};
        }

        sectionIt->second.remove(keyIt);
        return Result<Void, Error>{};
    }

    Result<Void, IniDocument::Error> IniDocument::removeSection(StringSlice sectionName)
    {
        if (sectionName.isEmpty())
        {
            return Ark::Error{Error::InvalidSection};
        }

        auto it = sections.find(String{sectionName});
        if (it == sections.end())
        {
            return Ark::Error{Error::SectionNotFound};
        }

        sections.remove(it);
        return Result<Void, Error>{};
    }

    Collections::Array<String> IniDocument::getSectionNames() const
    {
        Collections::Array<String> names;
        names.reserve(sections.getCount());

        for (auto const& [name, _] : sections)
        {
            if (!name.isEmpty())
            {
                names.append(name);
            }
        }

        return names;
    }

    Collections::Array<String> IniDocument::getKeys(StringSlice sectionName) const
    {
        String const section = sectionName.isEmpty() ? globalSection : String{sectionName};

        auto it = sections.find(section);
        if (it == sections.end())
        {
            return Collections::Array<String>{};
        }

        Collections::Array<String> keys;
        keys.reserve(it->second.getCount());

        for (auto const& [key, _] : it->second)
        {
            keys.append(key);
        }

        return keys;
    }

    IniDocument::PropertyMap const* IniDocument::getSection(StringSlice sectionName) const
    {
        String const section = sectionName.isEmpty() ? globalSection : String{sectionName};

        auto it = sections.find(section);
        if (it == sections.end())
        {
            return nullptr;
        }

        return &it->second;
    }

    void IniDocument::clear()
    {
        sections.removeAll();
        comments.removeAll();
    }

    usize IniDocument::getSectionCount() const
    {
        usize count = 0;

        for (auto const& [name, properties] : sections)
        {
            if (!name.isEmpty() && !properties.isEmpty())
            {
                ++count;
            }
        }

        return count;
    }

    bool IniDocument::isEmpty() const
    {
        return sections.isEmpty();
    }

    void IniDocument::setComment(StringSlice sectionName, StringSlice comment)
    {
        comments[String{sectionName}] = String{comment};
    }

    Option<String> IniDocument::getComment(StringSlice sectionName) const
    {
        auto it = comments.find(String{sectionName});
        if (it == comments.end())
        {
            return none;
        }

        return Option<String>{it->second};
    }

    IniDocument::PropertyMap& IniDocument::getOrCreateSection(StringSlice sectionName)
    {
        String const section = sectionName.isEmpty() ? globalSection : String{sectionName};
        return sections[section];
    }
}
