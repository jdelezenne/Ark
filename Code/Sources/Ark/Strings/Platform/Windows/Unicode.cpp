#if defined(ARK_PLATFORM_WINDOWS)

#include "Ark/Strings/Unicode.hpp"

#include <Windows.h>

namespace Ark::Unicode
{
    String fromWide(wchar_t const* wideString)
    {
        if (wideString == nullptr)
        {
            return {};
        }

        int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, wideString, -1, nullptr, 0, nullptr, nullptr);
        if (sizeNeeded <= 1)
        {
            return {};
        }

        String out;
        out.resize(static_cast<String::SizeType>(sizeNeeded - 1));
        WideCharToMultiByte(CP_UTF8, 0, wideString, -1, out.asPointer(), sizeNeeded, nullptr, nullptr);
        return out;
    }

    Collections::Array<wchar_t> toWide(StringSlice utf8String)
    {
        Collections::Array<wchar_t> out;

        if (utf8String.isEmpty())
        {
            out.resize(1);
            out[0] = L'\0';
            return out;
        }

        int len = static_cast<int>(utf8String.getCount());
        int required = MultiByteToWideChar(CP_UTF8, 0, utf8String.asPointer(), len, nullptr, 0);
        if (required <= 0)
        {
            out.resize(1);
            out[0] = L'\0';
            return out;
        }

        out.resize(static_cast<usize>(required + 1));
        int written = MultiByteToWideChar(CP_UTF8, 0, utf8String.asPointer(), len, out.asMutablePointer(), required);
        out[static_cast<usize>(written)] = L'\0';
        return out;
    }
}

#endif
