# CXMLReader

## Overview
CXMLReader is responsible for reading XML entities from a data source.
It uses the Expat library to parse XML, producing entities for start 
elements, end elements, and character data. The reader also supports 
skipping character data entities when requested.

## Public Interface

The CXMLReader class provides functionality to read XML entities from an input source.
- `bool ReadEntity(SXMLEntity &entity, bool skipcdata = false)`
    - Reads the next XML entity from the data source and stores it in entity.
    - Returns true if an entity is successfully read, false if the end of the XML is reached.
    - If skipcdata is true, character data entities (CharData) are skipped.
- `bool End() const`
    - Returns true if all XML entities have been read and there is no more data to parse.

## Behavior and Rules
### Entity Types
- `SXMLEntity::EType::StartElement` – an opening tag <tag>
- `SXMLEntity::EType::EndElement` – a closing tag </tag>
- `SXMLEntity::EType::CharData` – character data between tags
- `SXMLEntity::EType::CompleteElement` – self-closing tag <tag /> (My Reader currently produces Start + End)

### Attributes
- Each StartElement may have attributes stored as TAttributes in SXMLEntity.
- Use `AttributeValue(name)` to get the value of a specific attribute.
- Use `AttributeExists(name)` to check if the attribute exists.
- Use `SetAttribute(name,value)` to set create new attributes. 

### Parsing Behavior
- The reader parses the XML stream in order, producing entities sequentially.
- Self-closing tags (<tag />) are currently parsed as a StartElement followed immediately by an EndElement.
- Consecutive or large blocks of character data may be split into multiple CharData entities.
- The skipcdata parameter allows ignoring character data if needed.

## Usage Example
### Example 1: 
```cpp
std::string XMLString = "<tag></tag>";
std::shared_ptr<CStringDataSource> DataSource = std::make_shared<CStringDataSource>(XMLString);
CXMLReader Reader(DataSource);
SXMLEntity Entity;

Reader.ReadEntity(Entity); 
// Entity.DType == StartElement
// Entity.DNameData == "tag"

Reader.ReadEntity(Entity); 
// Entity.DType == EndElement
// Entity.DNameData == "tag"

Reader.End(); // true
```

### Example 2: 
```cpp
std::string XMLString = "<tag attr1=\"val1\">Hello</tag>"
std::shared_ptr<CStringDataSource> DataSource = std::make_shared<CStringDataSource>(XMLString);
CXMLReader Reader(DataSource);
SXMLEntity Entity;

Reader.ReadEntity(Entity); 
// Entity.DType == StartElement
// Entity.DNameData == "tag"
// Entity.AttributeValue("attr1") == "val1"

Reader.ReadEntity(Entity); 
// Entity.DType == CharData
// Entity.DNameData == "Hello"

Reader.ReadEntity(Entity); 
// Entity.DType == EndElement
// Entity.DNameData == "tag"

Reader.End(); // true
```