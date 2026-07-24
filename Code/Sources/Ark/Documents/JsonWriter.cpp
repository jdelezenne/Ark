#include "Ark/Documents/JsonWriter.hpp"

#include <cinttypes>
#include <cstdio>

namespace Ark::Documents
{
    namespace
    {
        void appendIndent(String& output, usize depth)
        {
            for (usize i = 0; i < depth * 2; ++i)
            {
                output.append(' ');
            }
        }
    }

    JsonWriter::JsonWriter(Storage::Stream& stream)
        : stream{stream}
#if defined(ARK_JSON_BACKEND_RAPIDJSON)
        , stringBuffer()
        , writer(stringBuffer)
#elif defined(ARK_JSON_BACKEND_NLOHMANN)
        , root(nlohmann::json())
        , nodeStack()
        , pendingKey()
#endif
    {
#if defined(ARK_JSON_BACKEND_NLOHMANN)
        nodeStack.clear();
        nodeStack.append(&root);
#endif
    }

    JsonWriter::~JsonWriter()
    {
        close();
    }

    Outcome JsonWriter::close()
    {
        if (closed)
        {
            return makeOutcome();
        }
        closed = true;

#if defined(ARK_JSON_BACKEND_ARK)
        auto const written = stream.write(output.asPointer(), output.getLength());
        if (!written.isOk())
        {
            return makeError();
        }
#elif defined(ARK_JSON_BACKEND_RAPIDJSON)
        auto const written = stream.write(stringBuffer.GetString(), stringBuffer.GetSize());
        if (!written.isOk())
        {
            return makeError();
        }
#elif defined(ARK_JSON_BACKEND_NLOHMANN)
        std::string dumped = root.dump(2);
        auto const written = stream.write(dumped.c_str(), dumped.size());
        if (!written.isOk())
        {
            return makeError();
        }
#endif
        return stream.close();
    }

    Storage::Stream& JsonWriter::getStream() const
    {
        return stream;
    }

    Result<usize> JsonWriter::getSize() const
    {
        return stream.getSize();
    }

    void JsonWriter::writeBool(bool value)
    {
#if defined(ARK_JSON_BACKEND_ARK)
        if (!stack.isEmpty())
        {
            auto& context = stack.getLast();
            if (!context.first)
            {
                output += ",\n";
            }
            else
            {
                output += "\n";
                context.first = false;
            }
            appendIndent(output, stack.getCount());
            if (context.type == Context::Type::Object)
            {
                Details::jsonArkAppendEscaped(output, StringSlice(pendingKey.asPointer(), pendingKey.getLength()));
                output += ": ";
                pendingKey.clear();
            }
        }
        output += value ? "true" : "false";
        hasRoot = true;
#elif defined(ARK_JSON_BACKEND_RAPIDJSON)
        writer.Bool(value);
#elif defined(ARK_JSON_BACKEND_NLOHMANN)
        auto* node = nodeStack.getLast();
        if (node->is_array())
        {
            node->push_back(value);
        }
        else if (node->is_object())
        {
            (*node)[pendingKey] = value;
        }
#endif
    }

    void JsonWriter::writeUint8(uint8 value)
    {
        writeUint64(value);
    }

    void JsonWriter::writeInt8(int8 value)
    {
        writeInt64(value);
    }

    void JsonWriter::writeUint16(uint16 value)
    {
        writeUint64(value);
    }

    void JsonWriter::writeInt16(int16 value)
    {
        writeInt64(value);
    }

    void JsonWriter::writeUint32(uint32 value)
    {
        writeUint64(value);
    }

    void JsonWriter::writeInt32(int32 value)
    {
        writeInt64(value);
    }

    void JsonWriter::writeUint64(uint64 value)
    {
#if defined(ARK_JSON_BACKEND_ARK)
        if (!stack.isEmpty())
        {
            auto& context = stack.getLast();
            if (!context.first)
            {
                output += ",\n";
            }
            else
            {
                output += "\n";
                context.first = false;
            }
            appendIndent(output, stack.getCount());
            if (context.type == Context::Type::Object)
            {
                Details::jsonArkAppendEscaped(output, StringSlice(pendingKey.asPointer(), pendingKey.getLength()));
                output += ": ";
                pendingKey.clear();
            }
        }
        char buffer[32];
        int const length = std::snprintf(buffer, sizeof(buffer), "%" PRIu64, value);
        if (length > 0)
        {
            output.append(buffer, static_cast<usize>(length));
        }
        hasRoot = true;
#elif defined(ARK_JSON_BACKEND_RAPIDJSON)
        writer.Uint64(value);
#elif defined(ARK_JSON_BACKEND_NLOHMANN)
        auto* node = nodeStack.getLast();
        if (node->is_array())
        {
            node->push_back(static_cast<uint64_t>(value));
        }
        else if (node->is_object())
        {
            (*node)[pendingKey] = static_cast<uint64_t>(value);
        }
#endif
    }

    void JsonWriter::writeInt64(int64 value)
    {
#if defined(ARK_JSON_BACKEND_ARK)
        if (!stack.isEmpty())
        {
            auto& context = stack.getLast();
            if (!context.first)
            {
                output += ",\n";
            }
            else
            {
                output += "\n";
                context.first = false;
            }
            appendIndent(output, stack.getCount());
            if (context.type == Context::Type::Object)
            {
                Details::jsonArkAppendEscaped(output, StringSlice(pendingKey.asPointer(), pendingKey.getLength()));
                output += ": ";
                pendingKey.clear();
            }
        }
        char buffer[32];
        int const length = std::snprintf(buffer, sizeof(buffer), "%" PRId64, value);
        if (length > 0)
        {
            output.append(buffer, static_cast<usize>(length));
        }
        hasRoot = true;
#elif defined(ARK_JSON_BACKEND_RAPIDJSON)
        writer.Int64(value);
#elif defined(ARK_JSON_BACKEND_NLOHMANN)
        auto* node = nodeStack.getLast();
        if (node->is_array())
        {
            node->push_back(static_cast<int64_t>(value));
        }
        else if (node->is_object())
        {
            (*node)[pendingKey] = static_cast<int64_t>(value);
        }
#endif
    }

    void JsonWriter::writeFloat32(float32 value)
    {
        writeFloat64(value);
    }

    void JsonWriter::writeFloat64(float64 value)
    {
#if defined(ARK_JSON_BACKEND_ARK)
        if (!stack.isEmpty())
        {
            auto& context = stack.getLast();
            if (!context.first)
            {
                output += ",\n";
            }
            else
            {
                output += "\n";
                context.first = false;
            }
            appendIndent(output, stack.getCount());
            if (context.type == Context::Type::Object)
            {
                Details::jsonArkAppendEscaped(output, StringSlice(pendingKey.asPointer(), pendingKey.getLength()));
                output += ": ";
                pendingKey.clear();
            }
        }
        char buffer[64];
        int const length = std::snprintf(buffer, sizeof(buffer), "%.17g", value);
        if (length > 0)
        {
            output.append(buffer, static_cast<usize>(length));
        }
        hasRoot = true;
#elif defined(ARK_JSON_BACKEND_RAPIDJSON)
        writer.Double(value);
#elif defined(ARK_JSON_BACKEND_NLOHMANN)
        auto* node = nodeStack.getLast();
        if (node->is_array())
        {
            node->push_back(static_cast<float64>(value));
        }
        else if (node->is_object())
        {
            (*node)[pendingKey] = static_cast<float64>(value);
        }
#endif
    }

    void JsonWriter::writeKey(StringSlice str)
    {
#if defined(ARK_JSON_BACKEND_ARK)
        pendingKey.assign(str.asPointer(), str.getLength());
#elif defined(ARK_JSON_BACKEND_RAPIDJSON)
        writer.Key(str.asPointer());
#elif defined(ARK_JSON_BACKEND_NLOHMANN)
        pendingKey.assign(str.asPointer());
#endif
    }

    void JsonWriter::writeString(StringSlice str)
    {
#if defined(ARK_JSON_BACKEND_ARK)
        if (!stack.isEmpty())
        {
            auto& context = stack.getLast();
            if (!context.first)
            {
                output += ",\n";
            }
            else
            {
                output += "\n";
                context.first = false;
            }
            appendIndent(output, stack.getCount());
            if (context.type == Context::Type::Object)
            {
                Details::jsonArkAppendEscaped(output, StringSlice(pendingKey.asPointer(), pendingKey.getLength()));
                output += ": ";
                pendingKey.clear();
            }
        }
        Details::jsonArkAppendEscaped(output, str);
        hasRoot = true;
#elif defined(ARK_JSON_BACKEND_RAPIDJSON)
        writer.String(str.asPointer());
#elif defined(ARK_JSON_BACKEND_NLOHMANN)
        auto* node = nodeStack.getLast();
        if (node->is_array())
        {
            node->push_back(std::string(str.asPointer()));
        }
        else if (node->is_object())
        {
            (*node)[pendingKey] = std::string(str.asPointer());
        }
#endif
    }

    void JsonWriter::startObject()
    {
#if defined(ARK_JSON_BACKEND_ARK)
        if (!stack.isEmpty())
        {
            auto& context = stack.getLast();
            if (!context.first)
            {
                output += ",\n";
            }
            else
            {
                output += "\n";
                context.first = false;
            }
            appendIndent(output, stack.getCount());
            if (context.type == Context::Type::Object)
            {
                Details::jsonArkAppendEscaped(output, StringSlice(pendingKey.asPointer(), pendingKey.getLength()));
                output += ": ";
                pendingKey.clear();
            }
        }
        output += "{";
        stack.append(Context{Context::Type::Object, true});
        hasRoot = true;
#elif defined(ARK_JSON_BACKEND_RAPIDJSON)
        writer.StartObject();
#elif defined(ARK_JSON_BACKEND_NLOHMANN)
        auto* node = nodeStack.getLast();
        if (node->is_null())
        {
            *node = nlohmann::json::object();
        }
        else if (node->is_array())
        {
            node->push_back(nlohmann::json::object());
            nodeStack.append(&node->back());
        }
        else if (node->is_object())
        {
            (*node)[pendingKey] = nlohmann::json::object();
            nodeStack.append(&(*node)[pendingKey]);
        }
#endif
    }

    void JsonWriter::endObject()
    {
#if defined(ARK_JSON_BACKEND_ARK)
        if (stack.isEmpty())
        {
            return;
        }
        auto const context = stack.getLast();
        stack.pop();
        if (!context.first)
        {
            output += "\n";
            appendIndent(output, stack.getCount());
        }
        output += "}";
#elif defined(ARK_JSON_BACKEND_RAPIDJSON)
        writer.EndObject();
#elif defined(ARK_JSON_BACKEND_NLOHMANN)
        if (nodeStack.getCount() > 1)
        {
            nodeStack.pop();
        }
#endif
    }

    void JsonWriter::startArray()
    {
#if defined(ARK_JSON_BACKEND_ARK)
        if (!stack.isEmpty())
        {
            auto& context = stack.getLast();
            if (!context.first)
            {
                output += ",\n";
            }
            else
            {
                output += "\n";
                context.first = false;
            }
            appendIndent(output, stack.getCount());
            if (context.type == Context::Type::Object)
            {
                Details::jsonArkAppendEscaped(output, StringSlice(pendingKey.asPointer(), pendingKey.getLength()));
                output += ": ";
                pendingKey.clear();
            }
        }
        output += "[";
        stack.append(Context{Context::Type::Array, true});
        hasRoot = true;
#elif defined(ARK_JSON_BACKEND_RAPIDJSON)
        writer.StartArray();
#elif defined(ARK_JSON_BACKEND_NLOHMANN)
        auto* node = nodeStack.getLast();
        if (node->is_null())
        {
            *node = nlohmann::json::array();
        }
        else if (node->is_array())
        {
            node->push_back(nlohmann::json::array());
            nodeStack.append(&node->back());
        }
        else if (node->is_object())
        {
            (*node)[pendingKey] = nlohmann::json::array();
            nodeStack.append(&(*node)[pendingKey]);
        }
#endif
    }

    void JsonWriter::endArray([[maybe_unused]] usize count)
    {
#if defined(ARK_JSON_BACKEND_ARK)
        if (stack.isEmpty())
        {
            return;
        }
        auto const context = stack.getLast();
        stack.pop();
        if (!context.first)
        {
            output += "\n";
            appendIndent(output, stack.getCount());
        }
        output += "]";
#elif defined(ARK_JSON_BACKEND_RAPIDJSON)
        writer.EndArray(count);
#elif defined(ARK_JSON_BACKEND_NLOHMANN)
        if (nodeStack.getCount() > 1)
        {
            nodeStack.pop();
        }
#endif
    }
}
