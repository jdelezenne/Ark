#include "Ark/Testing/Test.hpp"

#include "Ark/Strings/String.hpp"
#include "Ark/System/Clipboard.hpp"

ARK_TEST_CASE("Clipboard", "[clipboard]")
{
    SECTION("basic text roundtrip")
    {
        Ark::String const text = "Hello Clipboard";
        REQUIRE(Ark::System::Clipboard::setText(text));
        REQUIRE(Ark::System::Clipboard::hasText());

        Ark::String got = Ark::System::Clipboard::getText();
        REQUIRE(got == text);
    }

    SECTION("primary selection local storage")
    {
        Ark::String const text = "Primary";
        REQUIRE(Ark::System::Clipboard::setPrimarySelectionText(text));
        REQUIRE(Ark::System::Clipboard::hasPrimarySelectionText());

        Ark::String got = Ark::System::Clipboard::getPrimarySelectionText();
        REQUIRE(got == text);
    }
}
