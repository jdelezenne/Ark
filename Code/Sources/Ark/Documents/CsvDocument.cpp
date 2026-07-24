#include "Ark/Documents/CsvDocument.hpp"

#include <charconv>
#include <cstdlib>

namespace Ark::Documents
{
    CsvDocument::CsvDocument()
        : rows{}
        , hasHeaderRow{false}
    {
    }

    CsvDocument::CsvDocument(bool hasHeaders)
        : rows{}
        , hasHeaderRow{hasHeaders}
    {
    }

    void CsvDocument::setHasHeaders(bool value)
    {
        hasHeaderRow = value;
    }

    bool CsvDocument::getHasHeaders() const
    {
        return hasHeaderRow;
    }

    usize CsvDocument::getRowCount() const
    {
        if (rows.isEmpty())
        {
            return 0;
        }

        if (hasHeaderRow)
        {
            return rows.getCount() > 0 ? rows.getCount() - 1 : 0;
        }

        return rows.getCount();
    }

    usize CsvDocument::getColumnCount() const
    {
        if (rows.isEmpty())
        {
            return 0;
        }

        return rows[0].getCount();
    }

    bool CsvDocument::isEmpty() const
    {
        return rows.isEmpty();
    }

    void CsvDocument::clear()
    {
        rows.clear();
    }

    Option<CsvDocument::Row const*> CsvDocument::getHeaders() const
    {
        if (!hasHeaderRow || rows.isEmpty())
        {
            return none;
        }

        return Option<Row const*>{&rows[0]};
    }

    void CsvDocument::setHeaders(Row const& headers)
    {
        if (rows.isEmpty())
        {
            rows.append(headers);
        }
        else if (hasHeaderRow)
        {
            rows[0] = headers;
        }
        else
        {
            rows.insert(0, headers);
        }

        hasHeaderRow = true;
    }

    void CsvDocument::appendRow(Row const& row)
    {
        rows.append(row);
    }

    Option<CsvDocument::Row const*> CsvDocument::getRow(usize rowIndex) const
    {
        usize const offset = getDataRowOffset();
        usize const actualIndex = offset + rowIndex;

        if (actualIndex >= rows.getCount())
        {
            return none;
        }

        return Option<Row const*>{&rows[actualIndex]};
    }

    Collections::Array<CsvDocument::Row> const& CsvDocument::getRows() const
    {
        return rows;
    }

    Option<String> CsvDocument::getValue(usize rowIndex, usize columnIndex) const
    {
        auto const rowOpt = getRow(rowIndex);
        if (!rowOpt.hasValue())
        {
            return none;
        }

        Row const& row = *rowOpt.getValue();
        if (columnIndex >= row.getCount())
        {
            return none;
        }

        return Option<String>{row[columnIndex]};
    }

    Option<String> CsvDocument::getValue(usize rowIndex, StringSlice columnName) const
    {
        auto const columnIndexOpt = findColumnIndex(columnName);
        if (!columnIndexOpt.hasValue())
        {
            return none;
        }

        return getValue(rowIndex, columnIndexOpt.getValue());
    }

    int64 CsvDocument::getInt(usize rowIndex, usize columnIndex, int64 defaultValue) const
    {
        auto const valueOpt = getValue(rowIndex, columnIndex);
        if (!valueOpt.hasValue())
        {
            return defaultValue;
        }

        String const& value = valueOpt.getValue();
        if (value.isEmpty())
        {
            return defaultValue;
        }

        int64 result = 0;
        auto const [ptr, ec] = std::from_chars(value.asPointer(), value.asPointer() + value.getLength(), result);

        if (ec != std::errc{} || ptr != value.asPointer() + value.getLength())
        {
            return defaultValue;
        }

        return result;
    }

    int64 CsvDocument::getInt(usize rowIndex, StringSlice columnName, int64 defaultValue) const
    {
        auto const columnIndexOpt = findColumnIndex(columnName);
        if (!columnIndexOpt.hasValue())
        {
            return defaultValue;
        }

        return getInt(rowIndex, columnIndexOpt.getValue(), defaultValue);
    }

    uint64 CsvDocument::getUInt(usize rowIndex, usize columnIndex, uint64 defaultValue) const
    {
        auto const valueOpt = getValue(rowIndex, columnIndex);
        if (!valueOpt.hasValue())
        {
            return defaultValue;
        }

        String const& value = valueOpt.getValue();
        if (value.isEmpty())
        {
            return defaultValue;
        }

        uint64 result = 0;
        auto const [ptr, ec] = std::from_chars(value.asPointer(), value.asPointer() + value.getLength(), result);

        if (ec != std::errc{})
        {
            return defaultValue;
        }

        return result;
    }

    uint64 CsvDocument::getUInt(usize rowIndex, StringSlice columnName, uint64 defaultValue) const
    {
        auto const columnIndexOpt = findColumnIndex(columnName);
        if (!columnIndexOpt.hasValue())
        {
            return defaultValue;
        }

        return getUInt(rowIndex, columnIndexOpt.getValue(), defaultValue);
    }

    float64 CsvDocument::getFloat(usize rowIndex, usize columnIndex, float64 defaultValue) const
    {
        auto const valueOpt = getValue(rowIndex, columnIndex);
        if (!valueOpt.hasValue())
        {
            return defaultValue;
        }

        String const& value = valueOpt.getValue();
        if (value.isEmpty())
        {
            return defaultValue;
        }

        char* end = nullptr;
        float64 result = std::strtod(value.asPointer(), &end);

        if (end == value.asPointer() || *end != '\0')
        {
            return defaultValue;
        }

        return result;
    }

    float64 CsvDocument::getFloat(usize rowIndex, StringSlice columnName, float64 defaultValue) const
    {
        auto const columnIndexOpt = findColumnIndex(columnName);
        if (!columnIndexOpt.hasValue())
        {
            return defaultValue;
        }

        return getFloat(rowIndex, columnIndexOpt.getValue(), defaultValue);
    }

    bool CsvDocument::getBool(usize rowIndex, usize columnIndex, bool defaultValue) const
    {
        auto const valueOpt = getValue(rowIndex, columnIndex);
        if (!valueOpt.hasValue())
        {
            return defaultValue;
        }

        String const& value = valueOpt.getValue();
        if (value.isEmpty())
        {
            return defaultValue;
        }

        String const lower = value.toLowercase();

        if (lower == "true" || lower == "1" || lower == "yes" || lower == "on")
        {
            return true;
        }

        if (lower == "false" || lower == "0" || lower == "no" || lower == "off")
        {
            return false;
        }

        return defaultValue;
    }

    bool CsvDocument::getBool(usize rowIndex, StringSlice columnName, bool defaultValue) const
    {
        auto const columnIndexOpt = findColumnIndex(columnName);
        if (!columnIndexOpt.hasValue())
        {
            return defaultValue;
        }

        return getBool(rowIndex, columnIndexOpt.getValue(), defaultValue);
    }

    Result<Void, CsvDocument::Error> CsvDocument::setValue(usize rowIndex, usize columnIndex, StringSlice value)
    {
        usize const offset = getDataRowOffset();
        usize const actualIndex = offset + rowIndex;

        if (actualIndex >= rows.getCount())
        {
            return Ark::Error{Error::InvalidRowIndex};
        }

        Row& row = rows[actualIndex];
        if (columnIndex >= row.getCount())
        {
            return Ark::Error{Error::InvalidColumnIndex};
        }

        row[columnIndex] = String{value};
        return Result<Void, Error>{};
    }

    Result<Void, CsvDocument::Error> CsvDocument::setValue(usize rowIndex, StringSlice columnName, StringSlice value)
    {
        auto const columnIndexOpt = findColumnIndex(columnName);
        if (!columnIndexOpt.hasValue())
        {
            return Ark::Error{Error::InvalidColumnName};
        }

        return setValue(rowIndex, columnIndexOpt.getValue(), value);
    }

    Result<Void, CsvDocument::Error> CsvDocument::removeRow(usize rowIndex)
    {
        usize const offset = getDataRowOffset();
        usize const actualIndex = offset + rowIndex;

        if (actualIndex >= rows.getCount())
        {
            return Ark::Error{Error::InvalidRowIndex};
        }

        rows.removeAt(actualIndex);
        return Result<Void, Error>{};
    }

    Option<usize> CsvDocument::findColumnIndex(StringSlice columnName) const
    {
        if (!hasHeaderRow || rows.isEmpty())
        {
            return none;
        }

        Row const& headers = rows[0];
        for (usize i = 0; i < headers.getCount(); ++i)
        {
            if (headers[i] == columnName)
            {
                return Option<usize>{i};
            }
        }

        return none;
    }

    usize CsvDocument::getDataRowOffset() const
    {
        return hasHeaderRow ? 1 : 0;
    }
}
