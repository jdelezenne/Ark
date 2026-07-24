#if (ARK_USE_STANDARD_LIBRARY == ARK_STANDARD_LIBRARY_CPP)

#include "Ark/Strings/Unicode.hpp"

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#elif defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4996)
#endif

#include <codecvt>
#include <locale>

namespace Ark::Unicode
{
    String fromWide(wchar_t const* wideString)
    {
        if (wideString == nullptr)
        {
            return {};
        }

#if WCHAR_MAX == 0xFFFF || WCHAR_MAX == 0xFFFFu
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> converter;
        std::string utf8 = converter.to_bytes(wideString);
#else
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        std::string utf8 = converter.to_bytes(wideString);
#endif
        return utf8.c_str();
    }

    Collections::Array<wchar_t> toWide(StringSlice utf8String)
    {
#if WCHAR_MAX == 0xFFFF || WCHAR_MAX == 0xFFFFu
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> converter;
        std::wstring wide = converter.from_bytes(utf8String.asPointer(), utf8String.asPointer() + utf8String.getCount());
#else
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        std::wstring wide = converter.from_bytes(utf8String.asPointer(), utf8String.asPointer() + utf8String.getCount());
#endif

        Collections::Array<wchar_t> out;
        out.resize(static_cast<usize>(wide.size() + 1));
        for (usize i = 0; i < wide.size(); ++i)
        {
            out[i] = wide[i];
        }
        out[static_cast<usize>(wide.size())] = L'\0';
        return out;
    }
}

#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#elif defined(_MSC_VER)
#pragma warning(pop)
#endif

#endif
