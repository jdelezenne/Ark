#include "Ark/Testing/Test.hpp"

#include "Ark/Documents/CsvDocument.hpp"

using namespace Ark;
using namespace Ark::Documents;

ARK_TEST_CASE("CsvDocument", "[csv]")
{
    SECTION("CsvDocument basic operations")
    {
        CsvDocument doc;

        REQUIRE(doc.isEmpty());
        REQUIRE(doc.getRowCount() == 0);
        REQUIRE(doc.getColumnCount() == 0);
    }

    SECTION("CsvDocument append rows")
    {
        CsvDocument doc;

        CsvDocument::Row row1;
        row1.append("Alice");
        row1.append("25");
        doc.appendRow(row1);

        REQUIRE(!doc.isEmpty());
        REQUIRE(doc.getRowCount() == 1);
        REQUIRE(doc.getColumnCount() == 2);

        CsvDocument::Row row2;
        row2.append("Bob");
        row2.append("30");
        doc.appendRow(row2);

        REQUIRE(doc.getRowCount() == 2);
    }

    SECTION("CsvDocument clear document")
    {
        CsvDocument doc;
        CsvDocument::Row row;
        row.append("test");
        doc.appendRow(row);

        REQUIRE(!doc.isEmpty());
        doc.clear();
        REQUIRE(doc.isEmpty());
        REQUIRE(doc.getRowCount() == 0);
    }

    SECTION("CsvDocument with headers")
    {
        CsvDocument doc{true};

        CsvDocument::Row headers;
        headers.append("Name");
        headers.append("Age");
        headers.append("Score");
        doc.setHeaders(headers);

        REQUIRE(doc.getHasHeaders());
        auto headersOpt = doc.getHeaders();
        REQUIRE(headersOpt.hasValue());

        CsvDocument::Row const& retrievedHeaders = *headersOpt.getValue();
        REQUIRE(retrievedHeaders.getCount() == 3);
        REQUIRE(retrievedHeaders[0] == "Name");
        REQUIRE(retrievedHeaders[1] == "Age");
        REQUIRE(retrievedHeaders[2] == "Score");
    }

    SECTION("CsvDocument row count excludes header")
    {
        CsvDocument doc{true};
        CsvDocument::Row headers;
        headers.append("Col1");
        headers.append("Col2");
        doc.setHeaders(headers);

        REQUIRE(doc.getRowCount() == 0);

        CsvDocument::Row row1;
        row1.append("A");
        row1.append("B");
        doc.appendRow(row1);

        REQUIRE(doc.getRowCount() == 1);
    }

    SECTION("CsvDocument getValue by index")
    {
        CsvDocument doc;

        CsvDocument::Row row1;
        row1.append("Alice");
        row1.append("25");
        row1.append("95.5");
        doc.appendRow(row1);

        CsvDocument::Row row2;
        row2.append("Bob");
        row2.append("30");
        row2.append("87.3");
        doc.appendRow(row2);

        SECTION("Valid indices")
        {
            auto value = doc.getValue(0, 0);
            REQUIRE(value.hasValue());
            REQUIRE(value.getValue() == "Alice");

            value = doc.getValue(1, 1);
            REQUIRE(value.hasValue());
            REQUIRE(value.getValue() == "30");
        }

        SECTION("Invalid row index")
        {
            auto value = doc.getValue(10, 0);
            REQUIRE(!value.hasValue());
        }

        SECTION("Invalid column index")
        {
            auto value = doc.getValue(0, 10);
            REQUIRE(!value.hasValue());
        }
    }

    SECTION("CsvDocument getValue by valid column name")
    {
        CsvDocument doc{true};

        CsvDocument::Row headers;
        headers.append("Name");
        headers.append("Age");
        headers.append("Score");
        doc.setHeaders(headers);

        CsvDocument::Row row1;
        row1.append("Alice");
        row1.append("25");
        row1.append("95.5");
        doc.appendRow(row1);

        auto value = doc.getValue(0, "Name");
        REQUIRE(value.hasValue());
        REQUIRE(value.getValue() == "Alice");

        value = doc.getValue(0, "Age");
        REQUIRE(value.hasValue());
        REQUIRE(value.getValue() == "25");
    }

    SECTION("CsvDocument getValue by invalid column name")
    {
        CsvDocument doc{true};

        CsvDocument::Row headers;
        headers.append("Name");
        doc.setHeaders(headers);

        CsvDocument::Row row;
        row.append("Alice");
        doc.appendRow(row);

        auto value = doc.getValue(0, "InvalidColumn");
        REQUIRE(!value.hasValue());
    }

    SECTION("CsvDocument getValue by column name without headers")
    {
        CsvDocument docNoHeaders;
        CsvDocument::Row row;
        row.append("test");
        docNoHeaders.appendRow(row);

        auto value = docNoHeaders.getValue(0, "Name");
        REQUIRE(!value.hasValue());
    }

    SECTION("CsvDocument integer and unsigned conversions")
    {
        CsvDocument doc{true};

        CsvDocument::Row headers;
        headers.append("Age");
        doc.setHeaders(headers);

        CsvDocument::Row row;
        row.append("25");
        doc.appendRow(row);

        int64 age = doc.getInt(0, "Age");
        REQUIRE(age == 25);

        int64 defaultInt = doc.getInt(0, "Missing", 99);
        REQUIRE(defaultInt == 99);

        uint64 unsignedAge = doc.getUInt(0, "Age");
        REQUIRE(unsignedAge == 25u);

        uint64 defaultUInt = doc.getUInt(0, "Missing", 99u);
        REQUIRE(defaultUInt == 99u);
    }

    SECTION("CsvDocument float and bool conversions")
    {
        CsvDocument doc{true};

        CsvDocument::Row headers;
        headers.append("Score");
        headers.append("Active");
        doc.setHeaders(headers);

        CsvDocument::Row row;
        row.append("95.5");
        row.append("true");
        doc.appendRow(row);

        float64 score = doc.getFloat(0, "Score");
        REQUIRE(score == 95.5);

        float64 defaultFloat = doc.getFloat(0, "Missing", 1.5);
        REQUIRE(defaultFloat == 1.5);

        bool active = doc.getBool(0, "Active");
        REQUIRE(active == true);

        bool defaultBool = doc.getBool(0, "Missing", false);
        REQUIRE(defaultBool == false);
    }

    SECTION("CsvDocument invalid conversions use default")
    {
        CsvDocument::Row invalidRow;
        invalidRow.append("not_a_number");
        invalidRow.append("not_a_float");
        invalidRow.append("not_a_bool");
        invalidRow.append("invalid");

        CsvDocument invalidDoc;
        invalidDoc.appendRow(invalidRow);

        REQUIRE(invalidDoc.getInt(0, 0, 999) == 999);
        REQUIRE(invalidDoc.getFloat(0, 1, 1.5) == 1.5);
        REQUIRE(invalidDoc.getBool(0, 2, false) == false);
    }

    SECTION("CsvDocument boolean parsing")
    {
        CsvDocument doc;

        CsvDocument::Row row;
        row.append("true");
        row.append("True");
        row.append("TRUE");
        row.append("1");
        row.append("yes");
        row.append("on");
        row.append("false");
        row.append("False");
        row.append("FALSE");
        row.append("0");
        row.append("no");
        row.append("off");
        doc.appendRow(row);

        SECTION("True representations")
        {
            REQUIRE(doc.getBool(0, 0) == true);
            REQUIRE(doc.getBool(0, 1) == true);
            REQUIRE(doc.getBool(0, 2) == true);
            REQUIRE(doc.getBool(0, 3) == true);
            REQUIRE(doc.getBool(0, 4) == true);
            REQUIRE(doc.getBool(0, 5) == true);
        }

        SECTION("False representations")
        {
            REQUIRE(doc.getBool(0, 6) == false);
            REQUIRE(doc.getBool(0, 7) == false);
            REQUIRE(doc.getBool(0, 8) == false);
            REQUIRE(doc.getBool(0, 9) == false);
            REQUIRE(doc.getBool(0, 10) == false);
            REQUIRE(doc.getBool(0, 11) == false);
        }
    }

    SECTION("CsvDocument setValue")
    {
        CsvDocument doc{true};

        CsvDocument::Row headers;
        headers.append("Name");
        headers.append("Age");
        doc.setHeaders(headers);

        CsvDocument::Row row;
        row.append("Alice");
        row.append("25");
        doc.appendRow(row);

        auto resultByIndex = doc.setValue(0, 1, "30");
        REQUIRE(resultByIndex.isOk());

        auto value = doc.getValue(0, 1);
        REQUIRE(value.hasValue());
        REQUIRE(value.getValue() == "30");
    }

    SECTION("CsvDocument setValue by column name")
    {
        CsvDocument doc{true};

        CsvDocument::Row headers;
        headers.append("Name");
        headers.append("Age");
        doc.setHeaders(headers);

        CsvDocument::Row row;
        row.append("Alice");
        row.append("25");
        doc.appendRow(row);

        auto result = doc.setValue(0, "Name", "Bob");
        REQUIRE(result.isOk());

        auto value = doc.getValue(0, "Name");
        REQUIRE(value.hasValue());
        REQUIRE(value.getValue() == "Bob");
    }

    SECTION("CsvDocument setValue invalid row index")
    {
        CsvDocument doc{true};
        CsvDocument::Row headers;
        headers.append("Name");
        doc.setHeaders(headers);

        CsvDocument::Row row;
        row.append("Alice");
        doc.appendRow(row);

        auto result = doc.setValue(10, 0, "test");
        REQUIRE(result.isError());
    }

    SECTION("CsvDocument setValue invalid column index")
    {
        CsvDocument doc{true};
        CsvDocument::Row headers;
        headers.append("Name");
        doc.setHeaders(headers);

        CsvDocument::Row row;
        row.append("Alice");
        doc.appendRow(row);

        auto result = doc.setValue(0, 10, "test");
        REQUIRE(result.isError());
    }

    SECTION("CsvDocument setValue invalid column name")
    {
        CsvDocument doc{true};
        CsvDocument::Row headers;
        headers.append("Name");
        doc.setHeaders(headers);

        CsvDocument::Row row;
        row.append("Alice");
        doc.appendRow(row);

        auto result = doc.setValue(0, "InvalidColumn", "test");
        REQUIRE(result.isError());
    }

    SECTION("CsvDocument removeRow")
    {
        CsvDocument doc;

        CsvDocument::Row row1;
        row1.append("A");
        doc.appendRow(row1);

        CsvDocument::Row row2;
        row2.append("B");
        doc.appendRow(row2);

        CsvDocument::Row row3;
        row3.append("C");
        doc.appendRow(row3);

        REQUIRE(doc.getRowCount() == 3);

        SECTION("Remove valid row")
        {
            auto result = doc.removeRow(1);
            REQUIRE(result.isOk());
            REQUIRE(doc.getRowCount() == 2);

            auto value = doc.getValue(1, 0);
            REQUIRE(value.hasValue());
            REQUIRE(value.getValue() == "C");
        }

        SECTION("Remove invalid row")
        {
            auto result = doc.removeRow(10);
            REQUIRE(result.isError());
        }
    }

    SECTION("CsvDocument findColumnIndex")
    {
        CsvDocument doc{true};

        CsvDocument::Row headers;
        headers.append("Name");
        headers.append("Age");
        headers.append("Score");
        doc.setHeaders(headers);

        SECTION("Find existing column")
        {
            auto index = doc.findColumnIndex("Age");
            REQUIRE(index.hasValue());
            REQUIRE(index.getValue() == 1);
        }

        SECTION("Find non-existing column")
        {
            auto index = doc.findColumnIndex("Missing");
            REQUIRE(!index.hasValue());
        }

        SECTION("Without headers")
        {
            CsvDocument docNoHeaders;
            auto index = docNoHeaders.findColumnIndex("Name");
            REQUIRE(!index.hasValue());
        }
    }

    SECTION("CsvDocument getRow")
    {
        CsvDocument doc{true};

        CsvDocument::Row headers;
        headers.append("Col1");
        headers.append("Col2");
        doc.setHeaders(headers);

        CsvDocument::Row row;
        row.append("A");
        row.append("B");
        doc.appendRow(row);

        SECTION("Get valid row")
        {
            auto rowOpt = doc.getRow(0);
            REQUIRE(rowOpt.hasValue());

            CsvDocument::Row const& retrievedRow = *rowOpt.getValue();
            REQUIRE(retrievedRow.getCount() == 2);
            REQUIRE(retrievedRow[0] == "A");
            REQUIRE(retrievedRow[1] == "B");
        }

        SECTION("Get invalid row")
        {
            auto rowOpt = doc.getRow(10);
            REQUIRE(!rowOpt.hasValue());
        }
    }

    SECTION("CsvDocument negative numbers")
    {
        CsvDocument doc;

        CsvDocument::Row row;
        row.append("-42");
        row.append("-3.14");
        doc.appendRow(row);

        SECTION("Negative integer")
        {
            int64 value = doc.getInt(0, 0);
            REQUIRE(value == -42);
        }

        SECTION("Negative float")
        {
            float64 value = doc.getFloat(0, 1);
            REQUIRE(value == -3.14);
        }
    }
}
