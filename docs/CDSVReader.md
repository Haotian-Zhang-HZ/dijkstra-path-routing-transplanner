# CDSVReader

## Overview
CDSVReader is responsible for reading rows of string 
values from a data source using a user-provided delimiter.
It follows DSV formatting rules, including handling 
quoted fields, embedded delimiters, newlines, and double
quotes inside values.

## Public Interface
The CDSVReader class provides functionality to read rows of
string values from an input source.

- `bool ReadRow(std::vector<std::string>& row)`
    - Reads a single row of values from the data source and fills row with the parsed values.
    - Returns true if a row is successfully read.

- `bool End() const`
    - Returns true if the data source has no more rows to read.
## Behavior and Rules
### Quoting Rules
- Fields enclosed in double quotes are treated as a single value, even if they contain delimiters or newline characters.
- Double quotes inside a quoted value are represented as two consecutive double quotes (\"\") and should be unescaped during parsing.
### Newline Handling
- A row ends when an unquoted newline character is reached or the end of the data source is reached.
- Quoted newlines inside a field do not end the row.
### Delimiter Handling
- Fields are separated by the specified delimiter.
- A delimiter that appears inside a quoted field does not terminate the field.
- If the delimiter is specified as a double quote ("), it is interpreted as a comma (,).
### Empty Fields and Rows
- Empty fields ("") are valid.
- Empty rows are valid and produce a vector of size 1.

## Usage Example
### Example 1: Basic Reading
```cpp
std::shared_ptr<CStringDataSource> DataSource = std::make_shared<CStringDataSource>("Mahiru,Shiina\n2006,12,6");
CDSVReader Reader(DataSource, ',');

std::vector<std::string> row;

Reader.ReadRow(row);// True
// row: {"Mahiru","Shiina"}

Reader.ReadRow(row);// True
// row: {"2006", "12", "6"}

Reader.End(); // True
```

### Example 2: Handling Quoted Values
```cpp
std::shared_ptr<CStringDataSource> DataSource = std::make_shared<CStringDataSource>(",\"A,B\",,\"\"");
CDSVReader Reader(DataSource, ',');

std::vector<std::string> row;

Reader.ReadRow(row);// True
// row: {"", "A,B", "",""}

Reader.End(); // True
```