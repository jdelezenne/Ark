#include "Ark/Strings/Cp437.hpp"

namespace Ark::Strings
{
    String convertCp437ToUtf8(Collections::Array<uint8> const& inputData)
    {
        // clang-format off
        static const char* cp437_to_utf8[] = {
            "\x00", "\x01", "\x02", "\x03", "\x04", "\x05", "\x06", "\x07",
            "\x08", "\t", "\n", "\x0b", "\x0c", "\r", "\x0e", "\x0f",
            "\x10", "\x11", "\x12", "\x13", "\x14", "\x15", "\x16", "\x17",
            "\x18", "\x19", "\x1a", "\x1b", "\x1c", "\x1d", "\x1e", "\x1f",
            " ", "!", "\"", "#", "$", "%", "&", "'",
            "(", ")", "*", "+", ",", "-", ".", "/",
            "0", "1", "2", "3", "4", "5", "6", "7",
            "8", "9", ":", ";", "<", "=", ">", "?",
            "@", "A", "B", "C", "D", "E", "F", "G",
            "H", "I", "J", "K", "L", "M", "N", "O",
            "P", "Q", "R", "S", "T", "U", "V", "W",
            "X", "Y", "Z", "[", "\\", "]", "^", "_",
            "`", "a", "b", "c", "d", "e", "f", "g",
            "h", "i", "j", "k", "l", "m", "n", "o",
            "p", "q", "r", "s", "t", "u", "v", "w",
            "x", "y", "z", "{", "|", "}", "~", "\x7f",
            "\xc3\x87", "\xc3\xbc", "\xc3\xa9", "\xc3\xa2", "\xc3\xa4", "\xc3\xa0", "\xc3\xa5", "\xc3\xa7",
            "\xc3\xaa", "\xc3\xab", "\xc3\xa8", "\xc3\xaf", "\xc3\xae", "\xc3\xac", "\xc3\x84", "\xc3\x85",
            "\xc3\x89", "\xc3\xa6", "\xc3\x86", "\xc3\xb4", "\xc3\xb6", "\xc3\xb2", "\xc3\xbb", "\xc3\xb9",
            "\xc3\xbf", "\xc3\x96", "\xc3\x9c", "\xc2\xa2", "\xc2\xa3", "\xc2\xa5", "\xe2\x82\xa7", "\xc6\x92",
            "\xc3\xa1", "\xc3\xad", "\xc3\xb3", "\xc3\xba", "\xc3\xb1", "\xc3\x91", "\xc2\xaa", "\xc2\xba",
            "\xc2\xbf", "\xe2\x8c\x90", "\xc2\xac", "\xc2\xbd", "\xc2\xbc", "\xc2\xa1", "\xc2\xab", "\xc2\xbb",
            "\xe2\x96\x91", "\xe2\x96\x92", "\xe2\x96\x93", "\xe2\x94\x82", "\xe2\x94\xa4", "\xe2\x95\xa1", "\xe2\x95\xa2", "\xe2\x95\x96",
            "\xe2\x95\x95", "\xe2\x95\xa3", "\xe2\x95\x91", "\xe2\x95\x97", "\xe2\x95\x9d", "\xe2\x95\x9c", "\xe2\x95\x9b", "\xe2\x94\x90",
            "\xe2\x94\x94", "\xe2\x94\xb4", "\xe2\x94\xac", "\xe2\x94\x9c", "\xe2\x94\x80", "\xe2\x94\xbc", "\xe2\x95\x9e", "\xe2\x95\x9f",
            "\xe2\x95\x9a", "\xe2\x95\x94", "\xe2\x95\xa9", "\xe2\x95\xa6", "\xe2\x95\xa0", "\xe2\x95\x90", "\xe2\x95\xac", "\xe2\x95\xa7",
            "\xe2\x95\xa8", "\xe2\x95\xa4", "\xe2\x95\xa5", "\xe2\x95\x99", "\xe2\x95\x98", "\xe2\x95\x92", "\xe2\x95\x93", "\xe2\x95\xab",
            "\xe2\x95\xaa", "\xe2\x94\x98", "\xe2\x94\x8c", "\xe2\x96\x88", "\xe2\x96\x84", "\xe2\x96\x8c", "\xe2\x96\x90", "\xe2\x96\x80",
            "\xce\xb1", "\xc3\x9f", "\xce\x93", "\xcf\x80", "\xce\xa3", "\xcf\x83", "\xc2\xb5", "\xcf\x84",
            "\xce\xa6", "\xce\x98", "\xce\xa9", "\xce\xb4", "\xe2\x88\x9e", "\xcf\x86", "\xce\xb5", "\xe2\x88\xa9",
            "\xe2\x89\xa1", "\xc2\xb1", "\xe2\x89\xa5", "\xe2\x89\xa4", "\xe2\x8c\xa0", "\xe2\x8c\xa1", "\xc3\xb7", "\xe2\x89\x88",
            "\xc2\xb0", "\xe2\x88\x99", "\xc2\xb7", "\xe2\x88\x9a", "\xe2\x81\xbf", "\xc2\xb2", "\xe2\x96\xa0", "\xc2\xa0",
        };
        // clang-format on

        usize const inputLength = inputData.getCount();
        Collections::Array<char> utf8Buffer;
        utf8Buffer.resize(inputLength * 4);

        usize writePos = 0;
        for (usize i = 0; i < inputLength; ++i)
        {
            uint8 const byte = inputData[i];
            if (byte == 0)
            {
                // "\x00" string literals are empty for C-string walks; emit U+0000 explicitly.
                utf8Buffer[writePos++] = '\0';
                continue;
            }

            char const* replacement = cp437_to_utf8[byte];
            char const* src = replacement;
            while (*src != '\0')
            {
                utf8Buffer[writePos++] = *src++;
            }
        }

        return String{utf8Buffer.asPointer(), writePos};
    }
}
