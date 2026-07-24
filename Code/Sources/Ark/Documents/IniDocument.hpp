#pragma once

#include "Ark/Collections/HashMap.hpp"
#include "Ark/Core/Option.hpp"
#include "Ark/Core/Result.hpp"
#include "Ark/Core/Types.hpp"
#include "Ark/Strings/String.hpp"
#include "Ark/Strings/StringSlice.hpp"

namespace Ark::Documents
{
    /// In-memory representation of an INI document
    /// @details Supports sections, keys, values, and comments.
    /// Empty section name represents the global section (properties before any section header).
    class IniDocument final
    {
    public:
        /// Error types for INI document operations
        enum class Error
        {
            SectionNotFound,
            KeyNotFound,
            InvalidKey,
            InvalidSection,
        };

    public:
        using PropertyMap = Collections::HashMap<String, String>;

    private:
        using SectionMap = Collections::HashMap<String, PropertyMap>;

        SectionMap sections;
        Collections::HashMap<String, String> comments;
        String globalSection;

    public:
        IniDocument();

        ~IniDocument() = default;

        /// Checks if a section exists
        /// @param sectionName The section name (empty string for global section)
        bool hasSection(StringSlice sectionName) const;

        /// Checks if a key exists in a section
        /// @param sectionName The section name (empty string for global section)
        /// @param key The key name
        bool hasKey(StringSlice sectionName, StringSlice key) const;

        /// Gets a value from the document
        /// @param sectionName The section name (empty string for global section)
        /// @param key The key name
        /// @param defaultValue The default value if key doesn't exist
        String getValue(StringSlice sectionName, StringSlice key, StringSlice defaultValue = {}) const;

        /// Gets a value as an option
        /// @param sectionName The section name (empty string for global section)
        /// @param key The key name
        Option<String> getValueOption(StringSlice sectionName, StringSlice key) const;

        /// Gets an integer value
        /// @param sectionName The section name (empty string for global section)
        /// @param key The key name
        /// @param defaultValue The default value if key doesn't exist or cannot be parsed
        int64 getInt(StringSlice sectionName, StringSlice key, int64 defaultValue = 0) const;

        /// Gets an unsigned integer value
        /// @param sectionName The section name (empty string for global section)
        /// @param key The key name
        /// @param defaultValue The default value if key doesn't exist or cannot be parsed
        uint64 getUInt(StringSlice sectionName, StringSlice key, uint64 defaultValue = 0) const;

        /// Gets a floating-point value
        /// @param sectionName The section name (empty string for global section)
        /// @param key The key name
        /// @param defaultValue The default value if key doesn't exist or cannot be parsed
        float64 getFloat(StringSlice sectionName, StringSlice key, float64 defaultValue = 0.0) const;

        /// Gets a boolean value
        /// @param sectionName The section name (empty string for global section)
        /// @param key The key name
        /// @param defaultValue The default value if key doesn't exist or cannot be parsed
        bool getBool(StringSlice sectionName, StringSlice key, bool defaultValue = false) const;

        /// Sets a value in the document
        /// @param sectionName The section name (empty string for global section)
        /// @param key The key name
        /// @param value The value to set
        void setValue(StringSlice sectionName, StringSlice key, StringSlice value);

        /// Sets an integer value
        /// @param sectionName The section name (empty string for global section)
        /// @param key The key name
        /// @param value The value to set
        void setInt(StringSlice sectionName, StringSlice key, int64 value);

        /// Sets an unsigned integer value
        /// @param sectionName The section name (empty string for global section)
        /// @param key The key name
        /// @param value The value to set
        void setUInt(StringSlice sectionName, StringSlice key, uint64 value);

        /// Sets a floating-point value
        /// @param sectionName The section name (empty string for global section)
        /// @param key The key name
        /// @param value The value to set
        void setFloat(StringSlice sectionName, StringSlice key, float64 value);

        /// Sets a boolean value
        /// @param sectionName The section name (empty string for global section)
        /// @param key The key name
        /// @param value The value to set
        void setBool(StringSlice sectionName, StringSlice key, bool value);

        /// Removes a key from a section
        /// @param sectionName The section name (empty string for global section)
        /// @param key The key name
        Result<Void, Error> removeKey(StringSlice sectionName, StringSlice key);

        /// Removes an entire section
        /// @param sectionName The section name
        Result<Void, Error> removeSection(StringSlice sectionName);

        /// Gets all section names (excluding global section)
        Collections::Array<String> getSectionNames() const;

        /// Gets all keys in a section
        /// @param sectionName The section name (empty string for global section)
        Collections::Array<String> getKeys(StringSlice sectionName) const;

        /// Gets all properties in a section
        /// @param sectionName The section name (empty string for global section)
        PropertyMap const* getSection(StringSlice sectionName) const;

        /// Clears all sections and properties
        void clear();

        /// Gets the number of sections (excluding global if empty)
        usize getSectionCount() const;

        /// Checks if the document is empty
        bool isEmpty() const;

        /// Sets a comment for a section
        /// @param sectionName The section name
        /// @param comment The comment text
        void setComment(StringSlice sectionName, StringSlice comment);

        /// Gets a comment for a section
        /// @param sectionName The section name
        Option<String> getComment(StringSlice sectionName) const;

    private:
        PropertyMap& getOrCreateSection(StringSlice sectionName);
    };
}
