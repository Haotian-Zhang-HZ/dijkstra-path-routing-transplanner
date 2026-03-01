#include <gtest/gtest.h>
#include "DSVWriter.h"
#include "DSVReader.h"
#include "StringDataSource.h"
#include "StringDataSink.h"

TEST(DSVWriterTest, EmptyRowTest){
    std::shared_ptr<CStringDataSink> DataSink = std::make_shared<CStringDataSink>();
    CDSVWriter Writer(DataSink,',');

    EXPECT_TRUE(DataSink->String().empty());//test initial state of the data sink is empty
    EXPECT_EQ(DataSink->String(),"");
    EXPECT_TRUE(Writer.WriteRow({}));
    EXPECT_EQ(DataSink->String(),"");

}

TEST(DSVWriterTest, EmptyStringTest){
    std::shared_ptr<CStringDataSink> DataSink = std::make_shared<CStringDataSink>();
    CDSVWriter Writer(DataSink,',');

    EXPECT_TRUE(DataSink->String().empty());//test initial state of the data sink is empty
    EXPECT_EQ(DataSink->String(),"");
    EXPECT_TRUE(Writer.WriteRow({""}));
    EXPECT_EQ(DataSink->String(),"");

}

TEST(DSVWriterTest, SingleRowTest){
    std::shared_ptr<CStringDataSink> DataSink = std::make_shared<CStringDataSink>();
    CDSVWriter Writer(DataSink,',');

    EXPECT_TRUE(DataSink->String().empty());
    EXPECT_EQ(DataSink->String(),"");
    EXPECT_TRUE(Writer.WriteRow({"A","B","C"}));
    EXPECT_EQ(DataSink->String(),"A,B,C");

}


TEST(DSVWriterTest, ValueContainingDelimiterTest){
    std::shared_ptr<CStringDataSink> DataSink = std::make_shared<CStringDataSink>();
    CDSVWriter Writer(DataSink,',');

    EXPECT_TRUE(DataSink->String().empty());
    EXPECT_EQ(DataSink->String(),"");
    EXPECT_TRUE(Writer.WriteRow({"A,B","C","D"}));
    EXPECT_EQ(DataSink->String(),"\"A,B\",C,D");

}

TEST(DSVWriterTest, DifferentDelimiterTest){
    std::shared_ptr<CStringDataSink> DataSink = std::make_shared<CStringDataSink>();
    CDSVWriter Writer(DataSink,'|');

    EXPECT_TRUE(DataSink->String().empty());
    EXPECT_EQ(DataSink->String(),"");
    EXPECT_TRUE(Writer.WriteRow({"A|B","C","D"}));
    EXPECT_EQ(DataSink->String(),"\"A|B\"|C|D");

}

TEST(DSVWriterTest, DoubleQuoteAsDelimiterTest){
    std::shared_ptr<CStringDataSink> DataSink = std::make_shared<CStringDataSink>();
    CDSVWriter Writer(DataSink,'\"');

    EXPECT_TRUE(DataSink->String().empty());
    EXPECT_EQ(DataSink->String(),"");
    EXPECT_TRUE(Writer.WriteRow({"A","B","C"}));
    EXPECT_EQ(DataSink->String(),"A,B,C");

}

TEST(DSVWriterTest, ValueContainingQuoteTest){
    std::shared_ptr<CStringDataSink> DataSink = std::make_shared<CStringDataSink>();
    CDSVWriter Writer(DataSink,',');

    EXPECT_TRUE(DataSink->String().empty());
    EXPECT_EQ(DataSink->String(),"");
    EXPECT_TRUE(Writer.WriteRow({"Hello \"World\"","Test"}));
    EXPECT_EQ(DataSink->String(),"\"Hello \"\"World\"\"\",Test");

}

TEST(DSVWriterTest, ValueContainingQuoteAndDelimeterTest){
    std::shared_ptr<CStringDataSink> DataSink = std::make_shared<CStringDataSink>();
    CDSVWriter Writer(DataSink,',');

    EXPECT_TRUE(DataSink->String().empty());
    EXPECT_EQ(DataSink->String(),"");
    EXPECT_TRUE(Writer.WriteRow({"Hello \"World\"","Test,ing"}));
    EXPECT_EQ(DataSink->String(),"\"Hello \"\"World\"\"\",\"Test,ing\"");

}

TEST(DSVWriterTest, ValueContainingNewlineTest){
    std::shared_ptr<CStringDataSink> DataSink = std::make_shared<CStringDataSink>();
    CDSVWriter Writer(DataSink,',');

    EXPECT_TRUE(DataSink->String().empty());
    EXPECT_EQ(DataSink->String(),"");
    EXPECT_TRUE(Writer.WriteRow({"A\nB","C"}));
    EXPECT_EQ(DataSink->String(),"\"A\nB\",C");

}

TEST(DSVWriterTest, EmptyValuesTest){
    std::shared_ptr<CStringDataSink> DataSink = std::make_shared<CStringDataSink>();
    CDSVWriter Writer(DataSink,',');

    EXPECT_TRUE(DataSink->String().empty());
    EXPECT_EQ(DataSink->String(),"");
    EXPECT_TRUE(Writer.WriteRow({"","",""}));
    EXPECT_EQ(DataSink->String(),",,");

}

TEST(DSVWriterTest, SingleRowQuoteallTest){
    std::shared_ptr<CStringDataSink> DataSink = std::make_shared<CStringDataSink>();
    CDSVWriter Writer(DataSink,',', true);

    EXPECT_TRUE(DataSink->String().empty());
    EXPECT_EQ(DataSink->String(),"");
    EXPECT_TRUE(Writer.WriteRow({"A","B","C"}));
    EXPECT_EQ(DataSink->String(),"\"A\",\"B\",\"C\"");

}

TEST(DSVWriterTest, QuoteallContainingDoubleQuoteTest){
    std::shared_ptr<CStringDataSink> DataSink = std::make_shared<CStringDataSink>();
    CDSVWriter Writer(DataSink,',', true);

    EXPECT_TRUE(DataSink->String().empty());
    EXPECT_EQ(DataSink->String(),"");
    EXPECT_TRUE(Writer.WriteRow({"A,B","Hello\n World", "\"ZHT\""}));
    EXPECT_EQ(DataSink->String(),"\"A,B\",\"Hello\n World\",\"\"\"ZHT\"\"\"");

}

TEST(DSVWriterTest, WriteMultipleTimesTest){
    std::shared_ptr<CStringDataSink> DataSink = std::make_shared<CStringDataSink>();
    CDSVWriter Writer(DataSink,',');

    EXPECT_TRUE(DataSink->String().empty());
    EXPECT_EQ(DataSink->String(),"");
    EXPECT_TRUE(Writer.WriteRow({"A","B"}));
    EXPECT_TRUE(Writer.WriteRow({"C","D"}));
    EXPECT_TRUE(Writer.WriteRow({"E","F","G"}));
    EXPECT_EQ(DataSink->String(),"A,B\nC,D\nE,F,G");
}

TEST(DSVWriterTest, WriteMultipleTimesQuoteallTest){
    std::shared_ptr<CStringDataSink> DataSink = std::make_shared<CStringDataSink>();
    CDSVWriter Writer(DataSink,',', true);

    EXPECT_TRUE(DataSink->String().empty());
    EXPECT_EQ(DataSink->String(),"");
    EXPECT_TRUE(Writer.WriteRow({"A","B"}));
    EXPECT_TRUE(Writer.WriteRow({"C","D"}));
    EXPECT_EQ(DataSink->String(),"\"A\",\"B\"\n\"C\",\"D\"");
}

TEST(DSVWriterTest, FirstRowEmptyTest){
    std::shared_ptr<CStringDataSink> DataSink = std::make_shared<CStringDataSink>();
    CDSVWriter Writer(DataSink,',');

    EXPECT_TRUE(DataSink->String().empty());
    EXPECT_EQ(DataSink->String(),"");
    EXPECT_TRUE(Writer.WriteRow({}));
    EXPECT_TRUE(Writer.WriteRow({"A","B"}));
    EXPECT_EQ(DataSink->String(),"\nA,B");
}


///////////////////////////////////////////Below are tests for CDSVReader ////////////////////////////////////////////////
TEST(DSVReaderTest, ReadSingleRowTest){
    std::shared_ptr<CStringDataSource> DataSource = std::make_shared<CStringDataSource>("A,B,C");
    CDSVReader Reader(DataSource,',');
    std::vector<std::string> Row;

    EXPECT_FALSE(Reader.End());
    EXPECT_TRUE(Reader.ReadRow(Row));
    EXPECT_EQ(Row.size(),3);
    EXPECT_EQ(Row[0],"A");
    EXPECT_EQ(Row[1],"B");
    EXPECT_EQ(Row[2],"C");
    EXPECT_TRUE(Reader.End());
}

TEST(DSVReaderTest, DifferentDelimiterTest){
    std::shared_ptr<CStringDataSource> DataSource = std::make_shared<CStringDataSource>("A\tB\tC");
    CDSVReader Reader(DataSource,'\t');
    std::vector<std::string> Row;

    EXPECT_FALSE(Reader.End());
    EXPECT_TRUE(Reader.ReadRow(Row));
    EXPECT_EQ(Row.size(),3);
    EXPECT_EQ(Row[0],"A");
    EXPECT_EQ(Row[1],"B");
    EXPECT_EQ(Row[2],"C");
    EXPECT_TRUE(Reader.End());
}

TEST(DSVReaderTest, EmptySourceTest){
    std::shared_ptr<CStringDataSource> DataSource = std::make_shared<CStringDataSource>("");
    CDSVReader Reader(DataSource,',');
    std::vector<std::string> Row;

    EXPECT_FALSE(Reader.ReadRow(Row));
    EXPECT_TRUE(Reader.End());
    EXPECT_EQ(Row.size(), 0);

}

TEST(DSVReaderTest,MultipleRowTest){
    std::shared_ptr<CStringDataSource> DataSource = std::make_shared<CStringDataSource>("A,B,C\nD,E\nF");
    CDSVReader Reader(DataSource,',');
    std::vector<std::string> Row;

    EXPECT_FALSE(Reader.End());
    EXPECT_TRUE(Reader.ReadRow(Row));//first empty row
    EXPECT_EQ(Row.size(),3);
    EXPECT_EQ(Row[0],"A");
    EXPECT_EQ(Row[1],"B");
    EXPECT_EQ(Row[2],"C");
 

    EXPECT_FALSE(Reader.End());
    EXPECT_TRUE(Reader.ReadRow(Row));
    EXPECT_EQ(Row.size(),2);
    EXPECT_EQ(Row[0],"D");
    EXPECT_EQ(Row[1],"E");

    EXPECT_FALSE(Reader.End());
    EXPECT_TRUE(Reader.ReadRow(Row));
    EXPECT_EQ(Row.size(),1);
    EXPECT_EQ(Row[0],"F");

    EXPECT_TRUE(Reader.End());
}

TEST(DSVReaderTest, BeginningEmptyRowTest){
    std::shared_ptr<CStringDataSource> DataSource = std::make_shared<CStringDataSource>("\n\nA,B,C");
    CDSVReader Reader(DataSource,',');
    std::vector<std::string> Row;

    EXPECT_FALSE(Reader.End());
    EXPECT_TRUE(Reader.ReadRow(Row));//first empty row
    EXPECT_EQ(Row.size(),1);//this row has one empty string the size id 1 not 0 because current filed is initialized as empty string and we push it immediately when we encounter the newline
    EXPECT_EQ(Row[0],"");

    EXPECT_FALSE(Reader.End());
    EXPECT_TRUE(Reader.ReadRow(Row));//second empty row
    EXPECT_EQ(Row.size(),1);
    EXPECT_EQ(Row[0],"");

    EXPECT_FALSE(Reader.End());
    EXPECT_TRUE(Reader.ReadRow(Row));//third row with data
    EXPECT_EQ(Row.size(),3);
    EXPECT_EQ(Row[0],"A");
    EXPECT_EQ(Row[1],"B");
    EXPECT_EQ(Row[2],"C");

    EXPECT_TRUE(Reader.End());
}

TEST(DSVReaderTest, LastEmptyRowTest){
    std::shared_ptr<CStringDataSource> DataSource = std::make_shared<CStringDataSource>("A,B,C\n");
    CDSVReader Reader(DataSource,',');
    std::vector<std::string> Row;

    
    EXPECT_FALSE(Reader.End());
    EXPECT_TRUE(Reader.ReadRow(Row));
    EXPECT_EQ(Row.size(),3);
    EXPECT_EQ(Row[0],"A");
    EXPECT_EQ(Row[1],"B");
    EXPECT_EQ(Row[2],"C");

    EXPECT_TRUE(Reader.End());
    EXPECT_FALSE(Reader.ReadRow(Row));
    EXPECT_EQ(Row.size(),0);//readrow will call row.clear() at the beginning, before entering the loop, so size is 0

    EXPECT_TRUE(Reader.End());
}

TEST(DSVReaderTest, QuotedValuesTest){
    std::shared_ptr<CStringDataSource> DataSource = std::make_shared<CStringDataSource>("\"A,B\",\"Hello\nWorld\",\"Hi \"\"ZHT\"\"\",C");
    CDSVReader Reader(DataSource, ',');
    std::vector<std::string> Row;

    EXPECT_FALSE(Reader.End());
    EXPECT_TRUE(Reader.ReadRow(Row));
    EXPECT_EQ(Row.size(), 4);
    EXPECT_EQ(Row[0], "A,B");
    EXPECT_EQ(Row[1], "Hello\nWorld");
    EXPECT_EQ(Row[2], "Hi \"ZHT\"");
    EXPECT_EQ(Row[3], "C"); 

    EXPECT_TRUE(Reader.End());
}

TEST(DSVReaderTest, MixedEmptyAndQuotedTest){
    std::shared_ptr<CStringDataSource> DataSource = std::make_shared<CStringDataSource>(",\"A,B\",,\"\"");
    CDSVReader Reader(DataSource, ',');
    std::vector<std::string> Row;

    EXPECT_FALSE(Reader.End());
    EXPECT_TRUE(Reader.ReadRow(Row));
    EXPECT_EQ(Row.size(), 4);
    EXPECT_EQ(Row[0], "");
    EXPECT_EQ(Row[1], "A,B");
    EXPECT_EQ(Row[2], "");
    EXPECT_EQ(Row[3], "");

    EXPECT_TRUE(Reader.End());
}

TEST(DSVReaderTest, EndWithDelimiterTest){

    std::shared_ptr<CStringDataSource> DataSource = std::make_shared<CStringDataSource>("A,B,");
    CDSVReader Reader(DataSource, ',');
    std::vector<std::string> Row;

    EXPECT_FALSE(Reader.End());
    EXPECT_TRUE(Reader.ReadRow(Row));

    EXPECT_EQ(Row.size(), 3);
    EXPECT_EQ(Row[0], "A");
    EXPECT_EQ(Row[1], "B");
    EXPECT_EQ(Row[2], ""); 

    EXPECT_TRUE(Reader.End());
    EXPECT_FALSE(Reader.ReadRow(Row));
}


