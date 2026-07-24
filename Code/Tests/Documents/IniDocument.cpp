#include "Ark/Testing/Test.hpp"

#include "Ark/Documents/IniDocument.hpp"

using namespace Ark;
using namespace Ark::Documents;

ARK_TEST_CASE("IniDocument", "[ini]")
{
    SECTION("IniDocument string value operations")
    {
        IniDocument doc;

        doc.setValue("section", "key", "value");
        REQUIRE(doc.getValue("section", "key") == "value");
    }

    SECTION("IniDocument integer value operations")
    {
        IniDocument doc;

        doc.setInt("section", "number", 42);
        REQUIRE(doc.getInt("section", "number") == 42);
        REQUIRE(doc.getInt("section", "missing", 10) == 10);
    }

    SECTION("IniDocument unsigned integer value operations")
    {
        IniDocument doc;

        doc.setUInt("section", "unsigned", 123u);
        REQUIRE(doc.getUInt("section", "unsigned") == 123u);
        REQUIRE(doc.getUInt("section", "missing", 99u) == 99u);
    }

    SECTION("IniDocument float value operations")
    {
        IniDocument doc;

        doc.setFloat("section", "pi", 3.14159);
        REQUIRE(doc.getFloat("section", "pi") == 3.14159);
        REQUIRE(doc.getFloat("section", "missing", 2.71828) == 2.71828);
    }

    SECTION("IniDocument boolean value operations")
    {
        IniDocument doc;

        doc.setBool("section", "enabled", true);
        doc.setBool("section", "disabled", false);
        REQUIRE(doc.getBool("section", "enabled") == true);
        REQUIRE(doc.getBool("section", "disabled") == false);
        REQUIRE(doc.getBool("section", "missing", true) == true);
    }

    SECTION("IniDocument global section")
    {
        IniDocument doc;

        doc.setValue("", "globalKey", "globalValue");
        doc.setInt("", "globalNumber", 100);

        REQUIRE(doc.getValue("", "globalKey") == "globalValue");
        REQUIRE(doc.getInt("", "globalNumber") == 100);
    }

    SECTION("IniDocument section operations")
    {
        IniDocument doc;

        doc.setValue("section1", "key1", "value1");
        doc.setValue("section2", "key2", "value2");
        doc.setValue("section3", "key3", "value3");

        SECTION("Get section names")
        {
            auto sections = doc.getSectionNames();
            REQUIRE(sections.getCount() == 3);
        }

        SECTION("Get section")
        {
            auto const* section = doc.getSection("section1");
            REQUIRE(section != nullptr);
            REQUIRE(section->getCount() == 1);
        }

        SECTION("Remove section")
        {
            auto result = doc.removeSection("section2");
            REQUIRE(result.isOk());
            REQUIRE(doc.getSectionCount() == 2);

            auto failResult = doc.removeSection("nonexistent");
            REQUIRE(failResult.isError());
        }

        SECTION("Cannot remove global section")
        {
            auto result = doc.removeSection("");
            REQUIRE(result.isError());
        }
    }

    SECTION("IniDocument key operations")
    {
        IniDocument doc;

        doc.setValue("section", "key1", "value1");
        doc.setValue("section", "key2", "value2");

        SECTION("Get keys")
        {
            auto keys = doc.getKeys("section");
            REQUIRE(keys.getCount() == 2);
        }

        SECTION("Remove key")
        {
            auto result = doc.removeKey("section", "key1");
            REQUIRE(result.isOk());

            auto keys = doc.getKeys("section");
            REQUIRE(keys.getCount() == 1);

            auto failResult = doc.removeKey("section", "nonexistent");
            REQUIRE(failResult.isError());
        }
    }

    SECTION("IniDocument comments")
    {
        IniDocument doc;

        doc.setComment("section", "This is a comment");

        auto comment = doc.getComment("section");
        REQUIRE(comment.hasValue());
        REQUIRE(comment.getValue() == "This is a comment");

        auto missing = doc.getComment("nonexistent");
        REQUIRE(!missing.hasValue());
    }

    SECTION("IniDocument clear and isEmpty")
    {
        IniDocument doc;

        REQUIRE(doc.isEmpty());

        doc.setValue("section", "key", "value");
        REQUIRE(!doc.isEmpty());

        doc.clear();
        REQUIRE(doc.isEmpty());
        REQUIRE(doc.getSectionCount() == 0);
    }

    SECTION("IniDocument boolean parsing")
    {
        IniDocument doc;

        SECTION("Various true representations")
        {
            doc.setValue("bool", "true1", "true");
            doc.setValue("bool", "true2", "True");
            doc.setValue("bool", "true3", "TRUE");
            doc.setValue("bool", "true4", "1");
            doc.setValue("bool", "true5", "yes");
            doc.setValue("bool", "true6", "on");

            REQUIRE(doc.getBool("bool", "true1") == true);
            REQUIRE(doc.getBool("bool", "true2") == true);
            REQUIRE(doc.getBool("bool", "true3") == true);
            REQUIRE(doc.getBool("bool", "true4") == true);
            REQUIRE(doc.getBool("bool", "true5") == true);
            REQUIRE(doc.getBool("bool", "true6") == true);
        }

        SECTION("Various false representations")
        {
            doc.setValue("bool", "false1", "false");
            doc.setValue("bool", "false2", "False");
            doc.setValue("bool", "false3", "FALSE");
            doc.setValue("bool", "false4", "0");
            doc.setValue("bool", "false5", "no");
            doc.setValue("bool", "false6", "off");

            REQUIRE(doc.getBool("bool", "false1") == false);
            REQUIRE(doc.getBool("bool", "false2") == false);
            REQUIRE(doc.getBool("bool", "false3") == false);
            REQUIRE(doc.getBool("bool", "false4") == false);
            REQUIRE(doc.getBool("bool", "false5") == false);
            REQUIRE(doc.getBool("bool", "false6") == false);
        }
    }

    SECTION("IniDocument numeric parsing edge cases")
    {
        IniDocument doc;

        SECTION("Invalid integer returns default")
        {
            doc.setValue("test", "invalid", "not_a_number");
            REQUIRE(doc.getInt("test", "invalid", 999) == 999);
        }

        SECTION("Invalid float returns default")
        {
            doc.setValue("test", "invalid", "not_a_float");
            REQUIRE(doc.getFloat("test", "invalid", 1.5) == 1.5);
        }

        SECTION("Negative numbers")
        {
            doc.setInt("test", "negative", -42);
            REQUIRE(doc.getInt("test", "negative") == -42);
        }

        SECTION("Large numbers")
        {
            doc.setInt("test", "large", 2147483647);
            REQUIRE(doc.getInt("test", "large") == 2147483647);
        }
    }
}
