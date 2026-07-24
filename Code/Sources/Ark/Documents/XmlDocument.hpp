#pragma once

#include "Ark/Core/Option.hpp"
#include "Ark/Core/Types.hpp"
#include "Ark/Storage/Path.hpp"
#include "Ark/Storage/Stream.hpp"
#include "Ark/Strings/AnsiConversions.hpp"
#include "Ark/Strings/AnsiStrings.hpp"
#include "Ark/Strings/String.hpp"
#include "Ark/Strings/StringSlice.hpp"

#if defined(ARK_XML_BACKEND_ARK)
#include "Ark/Documents/XmlBackend.hpp"
#elif defined(ARK_XML_BACKEND_TINYXML2)
#include <tinyxml2.h>
#else
#error "A supported XML backend must be selected."
#endif

namespace Ark::Documents
{
    class XmlElementIterator;
    class XmlElementRange;

    class XmlElement final
    {
    private:
#if defined(ARK_XML_BACKEND_ARK)
        Details::XmlArkElement const* element = nullptr;
#else
        tinyxml2::XMLElement const* element = nullptr;
#endif

    public:
        XmlElement() = default;

#if defined(ARK_XML_BACKEND_ARK)
        explicit XmlElement(Details::XmlArkElement const* ptr)
#else
        explicit XmlElement(tinyxml2::XMLElement const* ptr)
#endif
            : element(ptr)
        {
            ARK_ASSERT(ptr != nullptr);
        }

        bool isValid() const
        {
            return element != nullptr;
        }

        explicit operator bool() const
        {
            return element != nullptr;
        }

        StringSlice getName() const
        {
            ARK_ASSERT(element != nullptr);
#if defined(ARK_XML_BACKEND_ARK)
            return StringSlice(element->name.asPointer(), element->name.getLength());
#else
            return element->Name() != nullptr ? StringSlice(element->Name()) : StringSlice();
#endif
        }

        Option<XmlElement> getFirstChildElement(char const* name = nullptr) const
        {
            if (element == nullptr)
            {
                return Option<XmlElement>();
            }
#if defined(ARK_XML_BACKEND_ARK)
            for (auto const& child : element->children)
            {
                if (name == nullptr || Details::xmlArkEquals(name, child->name))
                {
                    return Option<XmlElement>(XmlElement(child.get()));
                }
            }
            return Option<XmlElement>();
#else
            tinyxml2::XMLElement const* child = element->FirstChildElement(name);
            return child != nullptr ? Option<XmlElement>(XmlElement(child)) : Option<XmlElement>();
#endif
        }

        Option<XmlElement> getNextSiblingElement(char const* name = nullptr) const
        {
            if (element == nullptr)
            {
                return Option<XmlElement>();
            }
#if defined(ARK_XML_BACKEND_ARK)
            Details::XmlArkElement const* sibling = element->nextSibling;
            while (sibling != nullptr)
            {
                if (name == nullptr || Details::xmlArkEquals(name, sibling->name))
                {
                    return Option<XmlElement>(XmlElement(sibling));
                }
                sibling = sibling->nextSibling;
            }
            return Option<XmlElement>();
#else
            tinyxml2::XMLElement const* sibling = element->NextSiblingElement(name);
            return sibling != nullptr ? Option<XmlElement>(XmlElement(sibling)) : Option<XmlElement>();
#endif
        }

        Option<StringSlice> getText() const
        {
            if (element == nullptr)
            {
                return Ark::none;
            }
#if defined(ARK_XML_BACKEND_ARK)
            if (!element->hasText)
            {
                return Ark::none;
            }
            return Option<StringSlice>(StringSlice(element->text.asPointer(), element->text.getLength()));
#else
            char const* text = element->GetText();
            return text != nullptr ? Option<StringSlice>(text) : Ark::none;
#endif
        }

        Option<StringSlice> getAttribute(char const* name) const
        {
            if (element == nullptr)
            {
                return Ark::none;
            }
#if defined(ARK_XML_BACKEND_ARK)
            for (auto const& attribute : element->attributes)
            {
                if (Details::xmlArkEquals(name, attribute.first))
                {
                    return Option<StringSlice>(StringSlice(attribute.second.asPointer(), attribute.second.getLength()));
                }
            }
            return Ark::none;
#else
            char const* value = element->Attribute(name);
            return value != nullptr ? Option<StringSlice>(StringSlice(value)) : Ark::none;
#endif
        }

        Option<int32> getIntAttribute(char const* name) const
        {
            if (element == nullptr)
            {
                return Option<int32>();
            }
#if defined(ARK_XML_BACKEND_ARK)
            auto value = getAttribute(name);
            if (!value.hasValue())
            {
                return Option<int32>();
            }
            return Strings::toInt<int32>(value.getValue(), 10);
#else
            int32 value;
            if (element->QueryIntAttribute(name, &value) == tinyxml2::XML_SUCCESS)
            {
                return Option<int32>(value);
            }
            return Option<int32>();
#endif
        }

        Option<uint32> getUnsignedAttribute(char const* name) const
        {
            if (element == nullptr)
            {
                return Option<uint32>();
            }
#if defined(ARK_XML_BACKEND_ARK)
            auto value = getAttribute(name);
            if (!value.hasValue())
            {
                return Option<uint32>();
            }
            return Strings::toInt<uint32>(value.getValue(), 10);
#else
            uint32 value;
            if (element->QueryUnsignedAttribute(name, &value) == tinyxml2::XML_SUCCESS)
            {
                return Option<uint32>(value);
            }
            return Option<uint32>();
#endif
        }

        Option<float32> getFloatAttribute(char const* name) const
        {
            if (element == nullptr)
            {
                return Option<float32>();
            }
#if defined(ARK_XML_BACKEND_ARK)
            auto value = getAttribute(name);
            if (!value.hasValue())
            {
                return Option<float32>();
            }
            return Strings::toFloat<float32>(value.getValue());
#else
            float32 value;
            if (element->QueryFloatAttribute(name, &value) == tinyxml2::XML_SUCCESS)
            {
                return Option<float32>(value);
            }
            return Option<float32>();
#endif
        }

        Option<bool> getBoolAttribute(char const* name) const
        {
            if (element == nullptr)
            {
                return Option<bool>();
            }
#if defined(ARK_XML_BACKEND_ARK)
            auto value = getAttribute(name);
            if (!value.hasValue())
            {
                return Option<bool>();
            }

            StringSlice const attribute = value.getValue();
            if (attribute.isEqual(StringSlice("true")) || attribute.isEqual(StringSlice("1")))
            {
                return Option<bool>(true);
            }
            if (attribute.isEqual(StringSlice("false")) || attribute.isEqual(StringSlice("0")))
            {
                return Option<bool>(false);
            }
            return Option<bool>();
#else
            int32 value;
            if (element->QueryIntAttribute(name, &value) == tinyxml2::XML_SUCCESS)
            {
                return Option<bool>(value != 0);
            }
            return Option<bool>();
#endif
        }

        Option<String> getStringAttribute(char const* name) const
        {
            Option<StringSlice> value = getAttribute(name);
            return value.hasValue() ? Option<String>(String(value.getValue())) : Option<String>();
        }

        XmlElementRange children(char const* name = nullptr) const;
    };

    class XmlElementIterator final
    {
    private:
        Option<XmlElement> current;
        char const* nameFilter = nullptr;

    public:
        XmlElementIterator() = default;

        explicit XmlElementIterator(Option<XmlElement> const& element, char const* name = nullptr)
            : current(element)
            , nameFilter(name)
        {
        }

        XmlElement operator*() const
        {
            return current.getValue();
        }

        XmlElementIterator& operator++()
        {
            if (current.hasValue())
            {
                current = current.getValue().getNextSiblingElement(nameFilter);
            }
            return *this;
        }

        bool operator!=(XmlElementIterator const& other) const
        {
            bool const thisValid = current.hasValue();
            bool const otherValid = other.current.hasValue();
            return thisValid != otherValid;
        }
    };

    class XmlElementRange final
    {
    private:
        Option<XmlElement> first;
        char const* nameFilter = nullptr;

    public:
        XmlElementRange() = default;

        explicit XmlElementRange(Option<XmlElement> const& firstElement, char const* name = nullptr)
            : first(firstElement)
            , nameFilter(name)
        {
        }

        XmlElementIterator begin() const
        {
            return XmlElementIterator(first, nameFilter);
        }

        XmlElementIterator end() const
        {
            return XmlElementIterator(Option<XmlElement>(), nameFilter);
        }
    };

    inline XmlElementRange XmlElement::children(char const* name) const
    {
        return XmlElementRange(getFirstChildElement(name), name);
    }

    class XmlDocument final
    {
    private:
#if defined(ARK_XML_BACKEND_ARK)
        Details::XmlArkDocument document;
#else
        tinyxml2::XMLDocument document;
#endif

    public:
        XmlDocument() = default;

        bool parse(char const* text)
        {
#if defined(ARK_XML_BACKEND_ARK)
            return Details::parseXmlArkDocument(document, text, AnsiStrings::getLengthUnsafe(text));
#else
            return document.Parse(text) == tinyxml2::XML_SUCCESS;
#endif
        }

        bool parse(char const* text, size_t size)
        {
#if defined(ARK_XML_BACKEND_ARK)
            return Details::parseXmlArkDocument(document, text, size);
#else
            return document.Parse(text, size) == tinyxml2::XML_SUCCESS;
#endif
        }

        bool loadFromFile(Storage::Path const& path)
        {
#if defined(ARK_XML_BACKEND_ARK)
            return Details::loadXmlArkDocument(document, path);
#else
            return document.LoadFile(path.string().asPointer()) == tinyxml2::XML_SUCCESS;
#endif
        }

        Option<XmlElement> getRootElement() const
        {
#if defined(ARK_XML_BACKEND_ARK)
            return document.root != nullptr ? Option<XmlElement>(XmlElement(document.root.get())) : Option<XmlElement>();
#else
            tinyxml2::XMLElement const* root = document.RootElement();
            return root != nullptr ? Option<XmlElement>(XmlElement(root)) : Option<XmlElement>();
#endif
        }

        char const* getErrorString() const
        {
#if defined(ARK_XML_BACKEND_ARK)
            return document.error.asPointer();
#else
            return document.ErrorStr();
#endif
        }
    };
}
