#pragma once

#include "Ark/Storage/Stream.hpp"
#include "Ark/Strings/StringSlice.hpp"

#if defined(ARK_JSON_BACKEND_ARK)
#include "Ark/Collections/Array.hpp"
#include "Ark/Documents/JsonBackend.hpp"
#include "Ark/Strings/String.hpp"
#elif defined(ARK_JSON_BACKEND_RAPIDJSON)
#include <rapidjson/prettywriter.h>
#elif defined(ARK_JSON_BACKEND_NLOHMANN)
#include "Ark/Collections/Array.hpp"
#include <nlohmann/json.hpp>
#include <string>
#else
#error "A supported JSON backend must be selected."
#endif

namespace Ark::Documents
{
    class JsonWriter final
    {
    private:
        Storage::Stream& stream;
        bool closed = false;

#if defined(ARK_JSON_BACKEND_ARK)
        struct Context final
        {
            enum class Type : uint8
            {
                Object,
                Array,
            };

            Type type = Type::Object;
            bool first = true;
        };

        String output;
        Collections::Array<Context> stack;
        String pendingKey;
        bool hasRoot = false;
#elif defined(ARK_JSON_BACKEND_RAPIDJSON)
        rapidjson::StringBuffer stringBuffer;
        rapidjson::PrettyWriter<rapidjson::StringBuffer> writer;
#elif defined(ARK_JSON_BACKEND_NLOHMANN)
        nlohmann::json root;
        Ark::Collections::Array<nlohmann::json*> nodeStack;
        std::string pendingKey;
#endif

    public:
        JsonWriter(Storage::Stream& stream);

        ~JsonWriter();

        Outcome close();

        Storage::Stream& getStream() const;

        Result<usize> getSize() const;

        void writeBool(bool value);

        void writeUint8(uint8 value);

        void writeInt8(int8 value);

        void writeUint16(uint16 value);

        void writeInt16(int16 value);

        void writeUint32(uint32 value);

        void writeInt32(int32 value);

        void writeUint64(uint64 value);

        void writeInt64(int64 value);

        void writeFloat32(float32 value);

        void writeFloat64(float64 value);

        void writeKey(StringSlice str);

        void writeString(StringSlice str);

        void startObject();

        void endObject();

        void startArray();

        void endArray(usize count = 0);
    };
}
