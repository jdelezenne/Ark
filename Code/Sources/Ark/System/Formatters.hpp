#pragma once

#include "Ark/Strings/Internal/Format.hpp"
#include "Ark/System/Key.hpp"
#include "Ark/System/Mouse.hpp"
#include "Ark/System/Scancode.hpp"

template <>
struct ARK_FORMAT_NAMESPACE::formatter<Ark::Key, char> : ARK_FORMAT_NAMESPACE::formatter<Ark::uint32, char>
{
    auto format(Ark::Key key, ARK_FORMAT_NAMESPACE::format_context& ctx) const
    {
        return ARK_FORMAT_NAMESPACE::formatter<Ark::uint32, char>::format(static_cast<Ark::uint32>(key), ctx);
    }
};

template <>
struct ARK_FORMAT_NAMESPACE::formatter<Ark::Scancode, char> : ARK_FORMAT_NAMESPACE::formatter<Ark::uint32, char>
{
    auto format(Ark::Scancode scancode, ARK_FORMAT_NAMESPACE::format_context& ctx) const
    {
        return ARK_FORMAT_NAMESPACE::formatter<Ark::uint32, char>::format(static_cast<Ark::uint32>(scancode), ctx);
    }
};

template <>
struct ARK_FORMAT_NAMESPACE::formatter<Ark::KeyModifiers, char> : ARK_FORMAT_NAMESPACE::formatter<Ark::uint16, char>
{
    auto format(Ark::KeyModifiers modifiers, ARK_FORMAT_NAMESPACE::format_context& ctx) const
    {
        return ARK_FORMAT_NAMESPACE::formatter<Ark::uint16, char>::format(static_cast<Ark::uint16>(modifiers), ctx);
    }
};

template <>
struct ARK_FORMAT_NAMESPACE::formatter<Ark::MouseButton, char> : ARK_FORMAT_NAMESPACE::formatter<Ark::uint32, char>
{
    auto format(Ark::MouseButton button, ARK_FORMAT_NAMESPACE::format_context& ctx) const
    {
        return ARK_FORMAT_NAMESPACE::formatter<Ark::uint32, char>::format(static_cast<Ark::uint32>(button), ctx);
    }
};

template <>
struct ARK_FORMAT_NAMESPACE::formatter<Ark::MouseButtons, char> : ARK_FORMAT_NAMESPACE::formatter<Ark::uint32, char>
{
    auto format(Ark::MouseButtons buttons, ARK_FORMAT_NAMESPACE::format_context& ctx) const
    {
        return ARK_FORMAT_NAMESPACE::formatter<Ark::uint32, char>::format(static_cast<Ark::uint32>(buttons), ctx);
    }
};

template <>
struct ARK_FORMAT_NAMESPACE::formatter<Ark::MouseWheelDirection, char> : ARK_FORMAT_NAMESPACE::formatter<Ark::uint32, char>
{
    auto format(Ark::MouseWheelDirection direction, ARK_FORMAT_NAMESPACE::format_context& ctx) const
    {
        return ARK_FORMAT_NAMESPACE::formatter<Ark::uint32, char>::format(static_cast<Ark::uint32>(direction), ctx);
    }
};
