# CDSVWriter

## Overview

CDSVWriter is responsible for writing rows of string values to a data sink
using a user-provided delimiter. It follows DSV formatting rules and will
take care of special situations where delimeter/newline/double quotes presents
in the value to be written.

## Public Interface

The CDSVWriter class provides functionality to write rows of string values
to an output sink.

- `bool WriteRow(const std::vector<std::string>& row)`
  - Writes a single row of values to the data sink.
  - Returns true if the row is written successfully.

## Behavior and Rules

### Quoting Rules
A value is written with double quotes if any of the following is true:
- The value contains the delimiter 
- The value contains a newline character
- The value contains a double quote 
- Quote-all mode is enabled

### Newline Handling
- If WriteRow function is called multiple times, a newline character is written between rows only.
- An empty row is considered valid and produces no output.

### Double Quote Escaping
- Any double quote character(`"`) appearing inside a value is replaced with two
  double quotes (`""`).
- After escaping, the entire value is enclosed in double quotes.

### Delimiter Handling
- The delimiter character is written only between values, not after the last value.
- If the delimiter is specified as a double quote (`"`), it is interpreted
  as a comma (`,`).

## Usage Example
### Example 1：Basic Usage
```cpp
std::shared_ptr<CStringDataSink> DataSink = std::make_shared<CStringDataSink>();
CDSVWriter Writer(DataSink,',');

writer.WriteRow({"Mahiru", "Shiina"});
writer.WriteRow({"Hello, world", "42"});

std::string output = sink->String();
// Output:
// Mahiru,Shiina
// "Hello, world",42
```
### Example 2：Quote-all mode on and dealing with specail characters
```cpp
std::shared_ptr<CStringDataSink> DataSink = std::make_shared<CStringDataSink>();

CDSVWriter writer(DataSink, ',', true);

writer.WriteRow({"A,B", "Hello\n World", "\"ZHT\""});

std::string output = sink->String();
// Output:
// "A,B","Hello
//  World","""ZHT"""
```
