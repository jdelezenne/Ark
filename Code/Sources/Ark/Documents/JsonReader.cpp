#include "Ark/Documents/JsonReader.hpp"

#include <string>

namespace Ark::Documents
{
    JsonReader::JsonReader(Storage::Stream& stream)
        : stream{stream}
    {
        auto sizeResult = stream.getSize();
        if (!sizeResult)
        {
            return;
        }

        usize const size = *sizeResult;
        String str;
        str.resize(size);
        stream.read(str.asPointer(), str.getLength());

#if defined(ARK_JSON_BACKEND_ARK)
        Details::JsonArkParser parser(str.asPointer(), str.getLength());
        parser.parseDocument(document);
#elif defined(ARK_JSON_BACKEND_RAPIDJSON)
        document.Parse(str.asPointer());
#elif defined(ARK_JSON_BACKEND_NLOHMANN)
        try
        {
            document = nlohmann::json::parse(std::string(str.asPointer(), str.getLength()));
        }
        catch (...)
        {
            document = nlohmann::json();
        }
#endif
    }

    JsonReader::~JsonReader()
    {
        close();
    }

    Outcome JsonReader::close()
    {
        if (closed)
        {
            return makeOutcome();
        }
        closed = true;
        return stream.close();
    }

    Storage::Stream& JsonReader::getStream() const
    {
        return stream;
    }

    Result<usize> JsonReader::getSize() const
    {
        return stream.getSize();
    }

    JsonReader::Value JsonReader::getRoot() const
    {
#if defined(ARK_JSON_BACKEND_ARK)
        return JsonReader::Value(&document.root);
#elif defined(ARK_JSON_BACKEND_RAPIDJSON)
        return JsonReader::Value(document);
#else
        return JsonReader::Value(document);
#endif
    }

    bool JsonReader::hasMember(StringSlice name) const
    {
#if defined(ARK_JSON_BACKEND_ARK)
        return Details::jsonArkFindMember(document.root, name) != nullptr;
#elif defined(ARK_JSON_BACKEND_RAPIDJSON)
        return document.HasMember(name.asPointer());
#else
        return document.contains(std::string(name.asPointer(), name.getLength()));
#endif
    }

    JsonReader::Value JsonReader::getMember(StringSlice name) const
    {
#if defined(ARK_JSON_BACKEND_ARK)
        return JsonReader::Value(Details::jsonArkFindMember(document.root, name));
#elif defined(ARK_JSON_BACKEND_RAPIDJSON)
        return JsonReader::Value(document[name.asPointer()]);
#else
        auto it = document.find(std::string(name.asPointer(), name.getLength()));
        if (it != document.end())
        {
            return JsonReader::Value(*it);
        }
        static nlohmann::json nullJson;
        return JsonReader::Value(nullJson);
#endif
    }

    bool JsonReader::Value::hasMember(StringSlice name) const
    {
#if defined(ARK_JSON_BACKEND_ARK)
        if (value == nullptr)
        {
            return false;
        }
        return Details::jsonArkFindMember(*value, name) != nullptr;
#elif defined(ARK_JSON_BACKEND_RAPIDJSON)
        return value.HasMember(name.asPointer());
#else
        if (!value.is_object())
        {
            return false;
        }
        return value.contains(std::string(name.asPointer(), name.getLength()));
#endif
    }

    JsonReader::Value JsonReader::Value::getMember(StringSlice name) const
    {
#if defined(ARK_JSON_BACKEND_ARK)
        if (value == nullptr)
        {
            return JsonReader::Value(nullptr);
        }
        return JsonReader::Value(Details::jsonArkFindMember(*value, name));
#elif defined(ARK_JSON_BACKEND_RAPIDJSON)
        return JsonReader::Value(value[name.asPointer()]);
#else
        if (!value.is_object())
        {
            static nlohmann::json nullJson;
            return JsonReader::Value(nullJson);
        }
        auto it = value.find(std::string(name.asPointer(), name.getLength()));
        if (it != value.end())
        {
            return JsonReader::Value(*it);
        }
        static nlohmann::json nullJson;
        return JsonReader::Value(nullJson);
#endif
    }

    Ark::Collections::Array<String> JsonReader::Value::getMemberNames() const
    {
        Ark::Collections::Array<String> names;
#if defined(ARK_JSON_BACKEND_ARK)
        if (value == nullptr || value->type != Details::JsonArkType::Object)
        {
            return names;
        }

        names.reserve(value->objectValues.getCount());
        for (auto const& member : value->objectValues)
        {
            names.appendMake(member.first.asPointer(), member.first.getLength());
        }
#elif defined(ARK_JSON_BACKEND_RAPIDJSON)
        if (!value.IsObject())
        {
            return names;
        }

        names.reserve(value.MemberCount());
        for (auto iter = value.MemberBegin(); iter != value.MemberEnd(); ++iter)
        {
            names.appendMake(iter->name.GetString(), iter->name.GetStringLength());
        }
#else
        if (!value.is_object())
        {
            return names;
        }

        names.reserve(value.size());
        for (auto it = value.begin(); it != value.end(); ++it)
        {
            auto const& key = it.key();
            names.appendMake(key.c_str(), key.size());
        }
#endif
        return names;
    }

    JsonReader::Value JsonReader::Value::getMember(uint index) const
    {
#if defined(ARK_JSON_BACKEND_ARK)
        if (value == nullptr || value->type != Details::JsonArkType::Array || index >= value->arrayValues.getCount())
        {
            return JsonReader::Value(nullptr);
        }
        return JsonReader::Value(&value->arrayValues[index]);
#elif defined(ARK_JSON_BACKEND_RAPIDJSON)
        return JsonReader::Value(value[index]);
#else
        if (!value.is_array())
        {
            static nlohmann::json nullJson;
            return JsonReader::Value(nullJson);
        }
        if (index >= value.size())
        {
            static nlohmann::json nullJson;
            return JsonReader::Value(nullJson);
        }
        return JsonReader::Value(value[index]);
#endif
    }

    uint JsonReader::Value::getCount() const
    {
#if defined(ARK_JSON_BACKEND_ARK)
        if (value == nullptr || value->type != Details::JsonArkType::Array)
        {
            return 0;
        }
        return static_cast<uint>(value->arrayValues.getCount());
#elif defined(ARK_JSON_BACKEND_RAPIDJSON)
        if (!value.IsArray())
        {
            return 0;
        }
        return value.Size();
#else
        if (!value.is_array())
        {
            return 0;
        }
        return static_cast<uint>(value.size());
#endif
    }

    bool JsonReader::Value::getBool(StringSlice name, bool defaultValue) const
    {
#if defined(ARK_JSON_BACKEND_ARK)
        if (value == nullptr)
        {
            return defaultValue;
        }
        auto const* member = Details::jsonArkFindMember(*value, name);
        if (member == nullptr || member->type != Details::JsonArkType::Bool)
        {
            return defaultValue;
        }
        return member->boolValue;
#elif defined(ARK_JSON_BACKEND_RAPIDJSON)
        if (!value.HasMember(name.asPointer()))
        {
            return defaultValue;
        }
        auto const& member = value[name.asPointer()];
        if (!member.IsBool())
        {
            return defaultValue;
        }
        return member.GetBool();
#else
        if (!value.is_object())
        {
            return defaultValue;
        }
        auto it = value.find(std::string(name.asPointer(), name.getLength()));
        if (it == value.end() || !it->is_boolean())
        {
            return defaultValue;
        }
        return it->get<bool>();
#endif
    }

    uint JsonReader::Value::getUint(StringSlice name, uint defaultValue) const
    {
#if defined(ARK_JSON_BACKEND_ARK)
        if (value == nullptr)
        {
            return defaultValue;
        }
        auto const* member = Details::jsonArkFindMember(*value, name);
        if (member == nullptr || member->type != Details::JsonArkType::Number || !member->hasUnsigned)
        {
            return defaultValue;
        }
        return static_cast<uint>(member->unsignedValue);
#elif defined(ARK_JSON_BACKEND_RAPIDJSON)
        if (!value.HasMember(name.asPointer()))
        {
            return defaultValue;
        }
        auto const& member = value[name.asPointer()];
        if (!member.IsUint())
        {
            return defaultValue;
        }
        return member.GetUint();
#else
        if (!value.is_object())
        {
            return defaultValue;
        }
        auto it = value.find(std::string(name.asPointer(), name.getLength()));
        if (it == value.end() || !it->is_number_unsigned())
        {
            return defaultValue;
        }
        return static_cast<uint>(it->get<uint64_t>());
#endif
    }

    int JsonReader::Value::getInt(StringSlice name, int defaultValue) const
    {
#if defined(ARK_JSON_BACKEND_ARK)
        if (value == nullptr)
        {
            return defaultValue;
        }
        auto const* member = Details::jsonArkFindMember(*value, name);
        if (member == nullptr || member->type != Details::JsonArkType::Number || !member->hasSigned)
        {
            return defaultValue;
        }
        return static_cast<int>(member->signedValue);
#elif defined(ARK_JSON_BACKEND_RAPIDJSON)
        if (!value.HasMember(name.asPointer()))
        {
            return defaultValue;
        }
        auto const& member = value[name.asPointer()];
        if (!member.IsInt())
        {
            return defaultValue;
        }
        return member.GetInt();
#else
        if (!value.is_object())
        {
            return defaultValue;
        }
        auto it = value.find(std::string(name.asPointer(), name.getLength()));
        if (it == value.end() || !it->is_number_integer())
        {
            return defaultValue;
        }
        return static_cast<int>(it->get<int64_t>());
#endif
    }

    String JsonReader::Value::getString(StringSlice name, String const& defaultValue) const
    {
#if defined(ARK_JSON_BACKEND_ARK)
        if (value == nullptr)
        {
            return defaultValue;
        }
        auto const* member = Details::jsonArkFindMember(*value, name);
        if (member == nullptr || member->type != Details::JsonArkType::String)
        {
            return defaultValue;
        }
        return String(member->stringValue.asPointer(), member->stringValue.getLength());
#elif defined(ARK_JSON_BACKEND_RAPIDJSON)
        if (!value.HasMember(name.asPointer()))
        {
            return defaultValue;
        }
        auto const& member = value[name.asPointer()];
        if (!member.IsString())
        {
            return defaultValue;
        }
        return member.GetString();
#else
        if (!value.is_object())
        {
            return defaultValue;
        }
        auto it = value.find(std::string(name.asPointer(), name.getLength()));
        if (it == value.end() || !it->is_string())
        {
            return defaultValue;
        }
        auto s = it->get<std::string>();
        return String(s.c_str());
#endif
    }

    bool JsonReader::Value::isObject() const
    {
#if defined(ARK_JSON_BACKEND_ARK)
        return value != nullptr && value->type == Details::JsonArkType::Object;
#elif defined(ARK_JSON_BACKEND_RAPIDJSON)
        return value.IsObject();
#else
        return value.is_object();
#endif
    }

    bool JsonReader::Value::isArray() const
    {
#if defined(ARK_JSON_BACKEND_ARK)
        return value != nullptr && value->type == Details::JsonArkType::Array;
#elif defined(ARK_JSON_BACKEND_RAPIDJSON)
        return value.IsArray();
#else
        return value.is_array();
#endif
    }

    bool JsonReader::Value::isString() const
    {
#if defined(ARK_JSON_BACKEND_ARK)
        return value != nullptr && value->type == Details::JsonArkType::String;
#elif defined(ARK_JSON_BACKEND_RAPIDJSON)
        return value.IsString();
#else
        return value.is_string();
#endif
    }

    bool JsonReader::Value::isBool() const
    {
#if defined(ARK_JSON_BACKEND_ARK)
        return value != nullptr && value->type == Details::JsonArkType::Bool;
#elif defined(ARK_JSON_BACKEND_RAPIDJSON)
        return value.IsBool();
#else
        return value.is_boolean();
#endif
    }

    bool JsonReader::Value::isNumber() const
    {
#if defined(ARK_JSON_BACKEND_ARK)
        return value != nullptr && value->type == Details::JsonArkType::Number;
#elif defined(ARK_JSON_BACKEND_RAPIDJSON)
        return value.IsNumber();
#else
        return value.is_number();
#endif
    }

    bool JsonReader::Value::isUint64() const
    {
#if defined(ARK_JSON_BACKEND_ARK)
        return value != nullptr && value->type == Details::JsonArkType::Number && value->hasUnsigned;
#elif defined(ARK_JSON_BACKEND_RAPIDJSON)
        return value.IsUint64();
#else
        return value.is_number_unsigned();
#endif
    }

    bool JsonReader::Value::isInt64() const
    {
#if defined(ARK_JSON_BACKEND_ARK)
        return value != nullptr && value->type == Details::JsonArkType::Number && value->hasSigned;
#elif defined(ARK_JSON_BACKEND_RAPIDJSON)
        return value.IsInt64();
#else
        return value.is_number_integer();
#endif
    }

    String JsonReader::Value::asString(String const& defaultValue) const
    {
#if defined(ARK_JSON_BACKEND_ARK)
        if (value == nullptr || value->type != Details::JsonArkType::String)
        {
            return defaultValue;
        }
        return String(value->stringValue.asPointer(), value->stringValue.getLength());
#elif defined(ARK_JSON_BACKEND_RAPIDJSON)
        if (!value.IsString())
        {
            return defaultValue;
        }
        return String(value.GetString(), value.GetStringLength());
#else
        if (!value.is_string())
        {
            return defaultValue;
        }
        auto s = value.get<std::string>();
        return String(s.c_str(), s.size());
#endif
    }

    bool JsonReader::Value::asBool(bool defaultValue) const
    {
#if defined(ARK_JSON_BACKEND_ARK)
        if (value == nullptr || value->type != Details::JsonArkType::Bool)
        {
            return defaultValue;
        }
        return value->boolValue;
#elif defined(ARK_JSON_BACKEND_RAPIDJSON)
        if (!value.IsBool())
        {
            return defaultValue;
        }
        return value.GetBool();
#else
        if (!value.is_boolean())
        {
            return defaultValue;
        }
        return value.get<bool>();
#endif
    }

    uint64 JsonReader::Value::asUint64(uint64 defaultValue) const
    {
#if defined(ARK_JSON_BACKEND_ARK)
        if (value == nullptr || value->type != Details::JsonArkType::Number || !value->hasUnsigned)
        {
            return defaultValue;
        }
        return value->unsignedValue;
#elif defined(ARK_JSON_BACKEND_RAPIDJSON)
        if (!value.IsUint64())
        {
            return defaultValue;
        }
        return value.GetUint64();
#else
        if (!value.is_number_unsigned())
        {
            return defaultValue;
        }
        return value.get<uint64_t>();
#endif
    }

    int64 JsonReader::Value::asInt64(int64 defaultValue) const
    {
#if defined(ARK_JSON_BACKEND_ARK)
        if (value == nullptr || value->type != Details::JsonArkType::Number || !value->hasSigned)
        {
            return defaultValue;
        }
        return value->signedValue;
#elif defined(ARK_JSON_BACKEND_RAPIDJSON)
        if (!value.IsInt64())
        {
            return defaultValue;
        }
        return value.GetInt64();
#else
        if (!value.is_number_integer())
        {
            return defaultValue;
        }
        return value.get<int64_t>();
#endif
    }
}
