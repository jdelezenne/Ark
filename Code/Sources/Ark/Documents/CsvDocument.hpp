#pragma once

#include "Ark/Collections/Array.hpp"
#include "Ark/Core/Option.hpp"
#include "Ark/Core/Result.hpp"
#include "Ark/Core/Types.hpp"
#include "Ark/Strings/String.hpp"
#include "Ark/Strings/StringSlice.hpp"

namespace Ark::Documents
{
    /// In-memory representation of a CSV document
    /// @details Supports header row and data rows with type conversion utilities.
    class CsvDocument final
    {
    public:
        /// Error types for CSV document operations
        enum class Error
        {
            InvalidRowIndex,
            InvalidColumnIndex,
            InvalidColumnName,
            EmptyDocument,
            NoHeaders,
        };

        using Row = Collections::Array<String>;

    private:
        Collections::Array<Row> rows;
        bool hasHeaderRow;

    public:
        CsvDocument();

        explicit CsvDocument(bool hasHeaders);

        ~CsvDocument() = default;

        /// Sets whether the first row should be treated as headers
        /// @param value True if first row contains headers
        void setHasHeaders(bool value);

        /// Checks if the document has a header row
        bool getHasHeaders() const;

        /// Gets the number of rows (excluding header row if present)
        usize getRowCount() const;

        /// Gets the number of columns (based on first row or header)
        usize getColumnCount() const;

        /// Checks if the document is empty
        bool isEmpty() const;

        /// Clears all rows
        void clear();

        /// Gets the header row
        Option<Row const*> getHeaders() const;

        /// Sets the header row
        /// @param headers The header row to set
        void setHeaders(Row const& headers);

        /// Appends a row to the document
        /// @param row The row to append
        void appendRow(Row const& row);

        /// Gets a row by index
        /// @param rowIndex The row index (0-based, excluding header)
        Option<Row const*> getRow(usize rowIndex) const;

        /// Gets all rows (excluding header if present)
        Collections::Array<Row> const& getRows() const;

        /// Gets a cell value by row and column indices
        /// @param rowIndex The row index (0-based, excluding header)
        /// @param columnIndex The column index (0-based)
        Option<String> getValue(usize rowIndex, usize columnIndex) const;

        /// Gets a cell value by row index and column name
        /// @param rowIndex The row index (0-based, excluding header)
        /// @param columnName The column name (requires headers)
        Option<String> getValue(usize rowIndex, StringSlice columnName) const;

        /// Gets a cell value as an integer
        /// @param rowIndex The row index (0-based, excluding header)
        /// @param columnIndex The column index (0-based)
        /// @param defaultValue The default value if cell doesn't exist or cannot be parsed
        int64 getInt(usize rowIndex, usize columnIndex, int64 defaultValue = 0) const;

        /// Gets a cell value as an integer by column name
        /// @param rowIndex The row index (0-based, excluding header)
        /// @param columnName The column name (requires headers)
        /// @param defaultValue The default value if cell doesn't exist or cannot be parsed
        int64 getInt(usize rowIndex, StringSlice columnName, int64 defaultValue = 0) const;

        /// Gets a cell value as an unsigned integer
        /// @param rowIndex The row index (0-based, excluding header)
        /// @param columnIndex The column index (0-based)
        /// @param defaultValue The default value if cell doesn't exist or cannot be parsed
        uint64 getUInt(usize rowIndex, usize columnIndex, uint64 defaultValue = 0) const;

        /// Gets a cell value as an unsigned integer by column name
        /// @param rowIndex The row index (0-based, excluding header)
        /// @param columnName The column name (requires headers)
        /// @param defaultValue The default value if cell doesn't exist or cannot be parsed
        uint64 getUInt(usize rowIndex, StringSlice columnName, uint64 defaultValue = 0) const;

        /// Gets a cell value as a floating-point number
        /// @param rowIndex The row index (0-based, excluding header)
        /// @param columnIndex The column index (0-based)
        /// @param defaultValue The default value if cell doesn't exist or cannot be parsed
        float64 getFloat(usize rowIndex, usize columnIndex, float64 defaultValue = 0.0) const;

        /// Gets a cell value as a floating-point number by column name
        /// @param rowIndex The row index (0-based, excluding header)
        /// @param columnName The column name (requires headers)
        /// @param defaultValue The default value if cell doesn't exist or cannot be parsed
        float64 getFloat(usize rowIndex, StringSlice columnName, float64 defaultValue = 0.0) const;

        /// Gets a cell value as a boolean
        /// @param rowIndex The row index (0-based, excluding header)
        /// @param columnIndex The column index (0-based)
        /// @param defaultValue The default value if cell doesn't exist or cannot be parsed
        bool getBool(usize rowIndex, usize columnIndex, bool defaultValue = false) const;

        /// Gets a cell value as a boolean by column name
        /// @param rowIndex The row index (0-based, excluding header)
        /// @param columnName The column name (requires headers)
        /// @param defaultValue The default value if cell doesn't exist or cannot be parsed
        bool getBool(usize rowIndex, StringSlice columnName, bool defaultValue = false) const;

        /// Sets a cell value by row and column indices
        /// @param rowIndex The row index (0-based, excluding header)
        /// @param columnIndex The column index (0-based)
        /// @param value The value to set
        Result<Void, Error> setValue(usize rowIndex, usize columnIndex, StringSlice value);

        /// Sets a cell value by row index and column name
        /// @param rowIndex The row index (0-based, excluding header)
        /// @param columnName The column name (requires headers)
        /// @param value The value to set
        Result<Void, Error> setValue(usize rowIndex, StringSlice columnName, StringSlice value);

        /// Removes a row by index
        /// @param rowIndex The row index (0-based, excluding header)
        Result<Void, Error> removeRow(usize rowIndex);

        /// Finds the column index by name
        /// @param columnName The column name
        Option<usize> findColumnIndex(StringSlice columnName) const;

    private:
        usize getDataRowOffset() const;
    };
}
