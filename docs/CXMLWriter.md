# CXMLWriter

## Overview
CXMLWriter is responsible for writing XML entities to a data sink.
It supports writing start elements, end elements, character data, and complete elements.
Special characters in attributes, or character data are automatically escaped.
The writer can also flush any unclosed tags.

## Public Interface
The CXMLWriter class provides functionality to write XML entities to a CDataSink:
- `bool WriteEntity(const SXMLEntity &entity)`
    - Writes the given XML entity to the data sink.
    - Returns true if the entity is written successfully.
    - Handles StartElement, EndElement, CharData, and CompleteElement.
    - Automatically escapes special characters (<, >, &, ", ') in attribute values and character data.
- `bool Flush()`
    - Closes any unclosed start tags by writing the corresponding end tags.
    - Returns true when flush completes successfully.

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

### Escape Rules
- & → &amp;
- < → &lt;
- \> → &gt;
- " → &quot;
- ' → &apos;
- Escaping applies to both attribute values and CharData.

### Stack and Flush
- StartElement pushes the tag name onto a stack.
- EndElement pops the tag name if it matches the top of the stack.
- Flush() writes end tags for all remaining open tags.

## Usage Example
### Example 1: 
```cpp
std::shared_ptr<CStringDataSink> DataSink = std::make_shared<CStringDataSink>();
CXMLWriter Writer(DataSink);

SXMLEntity Start;
Start.DType = SXMLEntity::EType::StartElement;
Start.DNameData = "tag";
Writer.WriteEntity(Start); // true : <tag>

SXMLEntity End;
End.DType = SXMLEntity::EType::EndElement;
End.DNameData = "tag";
Writer.WriteEntity(End);   // true :  </tag>

Writer.Flush(); // No effect, all tags closed
DataSink->String(); // "<tag></tag>"
```

### Example 2: 
```cpp
std::shared_ptr<CStringDataSink> DataSink = std::make_shared<CStringDataSink>();
CXMLWriter Writer(DataSink);

SXMLEntity Start;
Start.DType = SXMLEntity::EType::StartElement;
Start.DNameData = "Mahiru";
Start.SetAttribute("birthday", "20061206");
Start.SetAttribute("pets", "Cat & Dog");
Writer.WriteEntity(Start)//true : <Mahiru birthday="20061206" pets="Cat &amp; Dog">


SXMLEntity Text;
Text.DType = SXMLEntity::EType::CharData;
Text.DNameData = "<Hello & World>";
Writer.WriteEntity(Text)//true : &lt;Hello &amp; World&gt;

SXMLEntity End;
End.DType = SXMLEntity::EType::EndElement;
End.DNameData = "Mahiru";
Writer.WriteEntity(End)// true : </Mahiru>

DataSink->String() // "<Mahiru birthday=\"20061206\" pets=\"Cat &amp; Dog\">&lt;Hello &amp; World&gt;</Mahiru>");
```