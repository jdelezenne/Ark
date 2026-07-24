#pragma once

#include "Ark/Collections/Array.hpp"
#include "Ark/Storage/Stream.hpp"
#include "Ark/Strings/String.hpp"
#include "Ark/Strings/StringSlice.hpp"

#if defined(ARK_JSON_BACKEND_ARK)
#include "Ark/Documents/JsonBackend.hpp"
#elif defined(ARK_JSON_BACKEND_RAPIDJSON)
#include <rapidjson/document.h>
#elif defined(ARK_JSON_BACKEND_NLOHMANN)
#include <nlohmann/json.hpp>
#else
#error "A supported JSON backend must be selected."
#endif

namespace Ark::Documents
{
    class JsonReader final
    {
    public:
        class Value final
        {
        private:
#if defined(ARK_JSON_BACKEND_ARK)
            Details::JsonArkValue const* value = nullptr;
#elif defined(ARK_JSON_BACKEND_RAPIDJSON)
            rapidjson::Value const& value;
#elif defined(ARK_JSON_BACKEND_NLOHMANN)
            nlohmann::json const& value;
#endif

        public:
#if defined(ARK_JSON_BACKEND_ARK)
            explicit Value(Details::JsonArkValue const* value)
                : value{value}
            {
            }
#elif defined(ARK_JSON_BACKEND_RAPIDJSON)
            Value(rapidjson::Value const& value)
                : value{value}
            {
            }
#elif defined(ARK_JSON_BACKEND_NLOHMANN)
            explicit Value(nlohmann::json const& value)
                : value{value}
            {
            }
#endif

            bool hasMember(StringSlice name) const;

            Value getMember(StringSlice name) const;

            Ark::Collections::Array<String> getMemberNames() const;

            Value getMember(uint name) const;

            uint getCount() const;

            bool getBool(StringSlice name, bool defaultValue = {}) const;

            uint getUint(StringSlice name, uint defaultValue = {}) const;

            int getInt(StringSlice name, int defaultValue = {}) const;

            String getString(StringSlice name, String const& defaultValue = {}) const;

            bool isObject() const;

            bool isArray() const;

            bool isString() const;

            bool isBool() const;

            bool isNumber() const;

            bool isUint64() const;

            bool isInt64() const;

            String asString(String const& defaultValue = {}) const;

            bool asBool(bool defaultValue = {}) const;

            uint64 asUint64(uint64 defaultValue = {}) const;

            int64 asInt64(int64 defaultValue = {}) const;
        };

    private:
        Storage::Stream& stream;
        bool closed = false;

#if defined(ARK_JSON_BACKEND_RAPIDJSON)
        rapidjson::Document document;
#elif defined(ARK_JSON_BACKEND_ARK)
        Details::JsonArkDocument document;
#elif defined(ARK_JSON_BACKEND_NLOHMANN)
        nlohmann::json document;
#endif

    public:
        JsonReader(Storage::Stream& stream);

        ~JsonReader();

        Outcome close();

        Storage::Stream& getStream() const;

        Result<usize> getSize() const;

        Value getRoot() const;

        bool hasMember(StringSlice name) const;

        Value getMember(StringSlice name) const;
    };
}
