#include <gtest/gtest.h>
#include "XMLReader.h"
#include "StringDataSource.h"
#include "XMLWriter.h"
#include "StringDataSink.h"


TEST(XMLReader, SimpleTest){
    std::string XMLString = "<tag></tag>";
    std::shared_ptr<CStringDataSource> DataSource = std::make_shared<CStringDataSource>(XMLString);//create a string data source with the XML string
    CXMLReader Reader(DataSource);//create an XML reader with the data source
    SXMLEntity TempEntity;//temporary entity to hold parsed entities

    EXPECT_TRUE(Reader.ReadEntity(TempEntity));//successfully read first entity
    EXPECT_EQ(TempEntity.DType,SXMLEntity::EType::StartElement);//should be start element
    EXPECT_EQ(TempEntity.DNameData,"tag");//tag name should be "tag"
    EXPECT_TRUE(TempEntity.DAttributes.empty());//no attributes
    EXPECT_FALSE(Reader.End());//not end yet, there is more data to read

    EXPECT_TRUE(Reader.ReadEntity(TempEntity));//successfully read second entity
    EXPECT_EQ(TempEntity.DType,SXMLEntity::EType::EndElement); //should be end element
    EXPECT_EQ(TempEntity.DNameData,"tag");//tag name should be "tag"
    EXPECT_TRUE(TempEntity.DAttributes.empty());//no attributes

    EXPECT_FALSE(Reader.ReadEntity(TempEntity));//should be end now, all data read
    EXPECT_TRUE(Reader.End());//should be end now, all data read
}


TEST(XMLReader, CharDataTest){
    std::string XMLString = "<tag attr1=\"val1\">Some Data</tag>";
    std::shared_ptr<CStringDataSource> DataSource = std::make_shared<CStringDataSource>(XMLString);
    CXMLReader Reader(DataSource);
    SXMLEntity TempEntity;

    EXPECT_TRUE(Reader.ReadEntity(TempEntity));
    EXPECT_EQ(TempEntity.DType,SXMLEntity::EType::StartElement);
    EXPECT_EQ(TempEntity.DNameData,"tag");
    ASSERT_EQ(TempEntity.DAttributes.size(),1);
    EXPECT_EQ(TempEntity.AttributeValue("attr1"),"val1");
    EXPECT_FALSE(Reader.End());

    EXPECT_TRUE(Reader.ReadEntity(TempEntity));
    EXPECT_EQ(TempEntity.DType,SXMLEntity::EType::CharData);
    EXPECT_EQ(TempEntity.DNameData,"Some Data");
    EXPECT_FALSE(Reader.End());

    EXPECT_TRUE(Reader.ReadEntity(TempEntity));
    EXPECT_EQ(TempEntity.DType,SXMLEntity::EType::EndElement);
    EXPECT_EQ(TempEntity.DNameData,"tag");
    EXPECT_TRUE(TempEntity.DAttributes.empty());

    EXPECT_FALSE(Reader.ReadEntity(TempEntity));
    EXPECT_TRUE(Reader.End());
}

TEST(XMLReader, SkipCDataTest){
    std::string XMLString = "<tag>Hello<child>World</child>!</tag>";
    std::shared_ptr<CStringDataSource> DataSource = std::make_shared<CStringDataSource>(XMLString);
    CXMLReader Reader(DataSource);
    SXMLEntity TempEntity;

    EXPECT_TRUE(Reader.ReadEntity(TempEntity, true));
    EXPECT_EQ(TempEntity.DType, SXMLEntity::EType::StartElement);
    EXPECT_EQ(TempEntity.DNameData, "tag");

    EXPECT_TRUE(Reader.ReadEntity(TempEntity, true));
    EXPECT_EQ(TempEntity.DType, SXMLEntity::EType::StartElement);
    EXPECT_EQ(TempEntity.DNameData, "child");

    EXPECT_TRUE(Reader.ReadEntity(TempEntity, true));
    EXPECT_EQ(TempEntity.DType, SXMLEntity::EType::EndElement);
    EXPECT_EQ(TempEntity.DNameData, "child");

    EXPECT_TRUE(Reader.ReadEntity(TempEntity, true));
    EXPECT_EQ(TempEntity.DType, SXMLEntity::EType::EndElement);
    EXPECT_EQ(TempEntity.DNameData, "tag");
    
    EXPECT_FALSE(Reader.ReadEntity(TempEntity));
    EXPECT_TRUE(Reader.End());
}

TEST(XMLReader, MultipleAttributesWithCompleteTest){
    std::string XMLString = "<math a=\"1\" b=\"2\"><ECS34 c=\"3\"/></math>";
    std::shared_ptr<CStringDataSource> DataSource = std::make_shared<CStringDataSource>(XMLString);
    CXMLReader Reader(DataSource);
    SXMLEntity TempEntity;

    EXPECT_TRUE(Reader.ReadEntity(TempEntity));
    EXPECT_EQ(TempEntity.DType, SXMLEntity::EType::StartElement);
    EXPECT_EQ(TempEntity.DNameData, "math");
    ASSERT_EQ(TempEntity.DAttributes.size(), 2);
    EXPECT_EQ(TempEntity.AttributeValue("a"), "1");
    EXPECT_EQ(TempEntity.AttributeValue("b"), "2");

    EXPECT_TRUE(Reader.ReadEntity(TempEntity));
    EXPECT_EQ(TempEntity.DType, SXMLEntity::EType::StartElement);
    EXPECT_EQ(TempEntity.DNameData, "ECS34");
    ASSERT_EQ(TempEntity.DAttributes.size(), 1);
    EXPECT_EQ(TempEntity.AttributeValue("c"), "3");

    EXPECT_TRUE(Reader.ReadEntity(TempEntity));
    EXPECT_EQ(TempEntity.DType, SXMLEntity::EType::EndElement);
    EXPECT_EQ(TempEntity.DNameData, "ECS34");

    EXPECT_TRUE(Reader.ReadEntity(TempEntity));
    EXPECT_EQ(TempEntity.DType, SXMLEntity::EType::EndElement);
    EXPECT_EQ(TempEntity.DNameData, "math");


    EXPECT_FALSE(Reader.ReadEntity(TempEntity));
    EXPECT_TRUE(Reader.End());
}

TEST(XMLReader, SpecialCharactersTest){
    std::string XMLString = "<tag attr1=\"A &amp; B\" attr2=\"&lt;&gt;&quot;&apos;\">Text&lt;&amp;&gt;\" '</tag>";
    std::shared_ptr<CStringDataSource> DataSource = std::make_shared<CStringDataSource>(XMLString);
    CXMLReader Reader(DataSource);
    SXMLEntity TempEntity;

    EXPECT_TRUE(Reader.ReadEntity(TempEntity));
    EXPECT_EQ(TempEntity.DType, SXMLEntity::EType::StartElement);
    EXPECT_EQ(TempEntity.DNameData, "tag");
    ASSERT_EQ(TempEntity.DAttributes.size(), 2);
    EXPECT_EQ(TempEntity.AttributeValue("attr1"), "A & B");        // &amp; -> &
    EXPECT_EQ(TempEntity.AttributeValue("attr2"), "<>\"'");        // &lt ; &gt; &quot; &apos;

    // CharData may be split into multiple entities
    EXPECT_TRUE(Reader.ReadEntity(TempEntity));
    EXPECT_EQ(TempEntity.DType, SXMLEntity::EType::CharData);
    EXPECT_EQ(TempEntity.DNameData, "Text");

    EXPECT_TRUE(Reader.ReadEntity(TempEntity));
    EXPECT_EQ(TempEntity.DType, SXMLEntity::EType::CharData);
    EXPECT_EQ(TempEntity.DNameData, "<");

    EXPECT_TRUE(Reader.ReadEntity(TempEntity));
    EXPECT_EQ(TempEntity.DType, SXMLEntity::EType::CharData);
    EXPECT_EQ(TempEntity.DNameData, "&");

    EXPECT_TRUE(Reader.ReadEntity(TempEntity));
    EXPECT_EQ(TempEntity.DType, SXMLEntity::EType::CharData);
    EXPECT_EQ(TempEntity.DNameData, ">");

    EXPECT_TRUE(Reader.ReadEntity(TempEntity));
    EXPECT_EQ(TempEntity.DType, SXMLEntity::EType::CharData);
    EXPECT_EQ(TempEntity.DNameData, "\" '");      

    EXPECT_TRUE(Reader.ReadEntity(TempEntity));
    EXPECT_EQ(TempEntity.DType, SXMLEntity::EType::EndElement);
    EXPECT_EQ(TempEntity.DNameData, "tag");

    EXPECT_FALSE(Reader.ReadEntity(TempEntity));
    EXPECT_TRUE(Reader.End());
}

TEST(XMLReader, ConsecutiveCharDataTest){
    std::string XMLString = "<tag>Line1\nLine2 Line3</tag>";
    std::shared_ptr<CStringDataSource> DataSource = std::make_shared<CStringDataSource>(XMLString);
    CXMLReader Reader(DataSource);
    SXMLEntity TempEntity;

    EXPECT_TRUE(Reader.ReadEntity(TempEntity));
    EXPECT_EQ(TempEntity.DType, SXMLEntity::EType::StartElement);
    EXPECT_EQ(TempEntity.DNameData, "tag");

    EXPECT_TRUE(Reader.ReadEntity(TempEntity));
    EXPECT_EQ(TempEntity.DType, SXMLEntity::EType::CharData);
    EXPECT_EQ(TempEntity.DNameData, "Line1");

    EXPECT_TRUE(Reader.ReadEntity(TempEntity));
    EXPECT_EQ(TempEntity.DType, SXMLEntity::EType::CharData);
    EXPECT_EQ(TempEntity.DNameData, "\n");

    EXPECT_TRUE(Reader.ReadEntity(TempEntity));
    EXPECT_EQ(TempEntity.DType, SXMLEntity::EType::CharData);
    EXPECT_EQ(TempEntity.DNameData, "Line2 Line3");

    EXPECT_TRUE(Reader.ReadEntity(TempEntity));
    EXPECT_EQ(TempEntity.DType, SXMLEntity::EType::EndElement);
    EXPECT_EQ(TempEntity.DNameData, "tag");

    EXPECT_FALSE(Reader.ReadEntity(TempEntity));
    EXPECT_TRUE(Reader.End());
}

///////////////////////////////////////////Below are XMLWriter tests///////////////////////////////////////////////

TEST(XMLWriter, StartEndElementTest){
    std::shared_ptr<CStringDataSink> DataSink = std::make_shared<CStringDataSink>();
    CXMLWriter Writer(DataSink);

    SXMLEntity Start;//create start element entity ob
    Start.DType = SXMLEntity::EType::StartElement;
    Start.DNameData = "tag";
    EXPECT_TRUE(Writer.WriteEntity(Start));

    SXMLEntity Text;
    Text.DType = SXMLEntity::EType::CharData;
    Text.DNameData = "Hello";
    EXPECT_TRUE(Writer.WriteEntity(Text));

    SXMLEntity End;
    End.DType = SXMLEntity::EType::EndElement;
    End.DNameData = "tag";
    EXPECT_TRUE(Writer.WriteEntity(End));

    EXPECT_EQ(DataSink->String(), "<tag>Hello</tag>");
}

TEST(XMLWriter, AttributesAndSpecialCharsTest){
    std::shared_ptr<CStringDataSink> DataSink = std::make_shared<CStringDataSink>();
    CXMLWriter Writer(DataSink);
 
    SXMLEntity Start;
    Start.DType = SXMLEntity::EType::StartElement;
    Start.DNameData = "Mahiru";
    Start.SetAttribute("birthday", "20061206");
    Start.SetAttribute("pets", "Cat & Dog");

    EXPECT_TRUE(Writer.WriteEntity(Start));

    SXMLEntity Text;
    Text.DType = SXMLEntity::EType::CharData;
    Text.DNameData = "<Hello & World>";
    EXPECT_TRUE(Writer.WriteEntity(Text));

    SXMLEntity End;
    End.DType = SXMLEntity::EType::EndElement;
    End.DNameData = "Mahiru";
    EXPECT_TRUE(Writer.WriteEntity(End));

    EXPECT_EQ(DataSink->String(), "<Mahiru birthday=\"20061206\" pets=\"Cat &amp; Dog\">&lt;Hello &amp; World&gt;</Mahiru>");
}

TEST(XMLWriter, CompleteElementAndFlushTest){
    std::shared_ptr<CStringDataSink> DataSink = std::make_shared<CStringDataSink>();
    CXMLWriter Writer(DataSink);

    SXMLEntity Complete;
    Complete.DType = SXMLEntity::EType::CompleteElement;
    Complete.DNameData = "tag1";
    Complete.SetAttribute("attr1", "A");
    EXPECT_TRUE(Writer.WriteEntity(Complete));

    SXMLEntity Start;
    Start.DType = SXMLEntity::EType::StartElement;
    Start.DNameData = "tag2";
    EXPECT_TRUE(Writer.WriteEntity(Start));

    EXPECT_TRUE(Writer.Flush());

    EXPECT_EQ(DataSink->String(), "<tag1 attr1=\"A\"/><tag2></tag2>");
}


TEST(XMLWriterTest, ComprehensiveTest) {
    std::shared_ptr<CStringDataSink> DataSink = std::make_shared<CStringDataSink>();
    CXMLWriter Writer(DataSink);

    // StartElement
    SXMLEntity startTag;
    startTag.DType = SXMLEntity::EType::StartElement;
    startTag.DNameData = "tag1";
    startTag.DAttributes.push_back({"attr1","A"});
    EXPECT_TRUE(Writer.WriteEntity(startTag));

    //CharData with special characters
    SXMLEntity charData;
    charData.DType = SXMLEntity::EType::CharData;
    charData.DNameData = "<>&\"'";
    EXPECT_TRUE(Writer.WriteEntity(charData));

    //EndElement (matching) → return true
    SXMLEntity endTag;
    endTag.DType = SXMLEntity::EType::EndElement;
    endTag.DNameData = "tag1";
    EXPECT_TRUE(Writer.WriteEntity(endTag));

    //EndElement (mismatched) → return false
    SXMLEntity endTagMismatch;
    endTagMismatch.DType = SXMLEntity::EType::EndElement;
    endTagMismatch.DNameData = "tag2";
    EXPECT_FALSE(Writer.WriteEntity(endTagMismatch));

    //CompleteElement (self-closing)
    SXMLEntity completeTag;
    completeTag.DType = SXMLEntity::EType::CompleteElement;
    completeTag.DNameData = "tag3";
    completeTag.DAttributes.push_back({"attr2","B"});
    EXPECT_TRUE(Writer.WriteEntity(completeTag));

    //StartElement not closed → will be closed by Flush
    SXMLEntity startTag2;
    startTag2.DType = SXMLEntity::EType::StartElement;
    startTag2.DNameData = "tag4";
    EXPECT_TRUE(Writer.WriteEntity(startTag2));

    // Flush should close tag4
    EXPECT_TRUE(Writer.Flush());

    std::string xml = DataSink->String();

    EXPECT_NE(xml.find("<tag1 attr1=\"A\">"), std::string::npos);
    EXPECT_NE(xml.find("&lt;&gt;&amp;&quot;&apos;"), std::string::npos);
    EXPECT_NE(xml.find("</tag1>"), std::string::npos);
    EXPECT_NE(xml.find("<tag3 attr2=\"B\"/>"), std::string::npos);
    EXPECT_NE(xml.find("<tag4>"), std::string::npos);
    EXPECT_NE(xml.find("</tag4>"), std::string::npos);
}