#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "StringUtils.h"


TEST(StringUtilsTest, SliceTest){
    //construct test string
    const std::string testString = "TestForSlice";
    const std::string emptyString = "";
    const std::string singleChar = "A";

    //normal cases
    EXPECT_EQ(StringUtils::Slice(testString, 4, 7), std::string("For"));
    EXPECT_EQ(StringUtils::Slice(testString, 0, 4), std::string("Test"));
    EXPECT_EQ(StringUtils::Slice(testString, 0, 0), std::string("TestForSlice"));
    EXPECT_EQ(StringUtils::Slice(testString, 4, 0), std::string("ForSlice"));
    
    //negative indecies
    EXPECT_EQ(StringUtils::Slice(testString, -5, 0), std::string("Slice"));
    EXPECT_EQ(StringUtils::Slice(testString, -8, -5), std::string("For"));
    EXPECT_EQ(StringUtils::Slice(testString, 0, -5), std::string("TestFor"));
    EXPECT_EQ(StringUtils::Slice(testString, -5, 0), std::string("Slice"));
    
    //edge cases:
    //edge case 1: start index greater than end index
    EXPECT_EQ(StringUtils::Slice(testString, 2, 1), std::string(""));
    EXPECT_EQ(StringUtils::Slice(testString, -1, -2), std::string(""));
    
    //edge case 2: start index or end index out of bounds
    EXPECT_EQ(StringUtils::Slice(testString, 100, 0), std::string(""));
    
    //edge case 3: empty string
    EXPECT_EQ(StringUtils::Slice(emptyString, 0, 0), std::string(""));
    EXPECT_EQ(StringUtils::Slice(emptyString, 2, -3), std::string(""));
    
    //edge case 4: start index equals end index
    EXPECT_EQ(StringUtils::Slice(testString, 1, 1), std::string(""));
    EXPECT_EQ(StringUtils::Slice(testString, -1, -1), std::string(""));

    //edge case 5: single character string
    EXPECT_EQ(StringUtils::Slice(singleChar, 0, 1), std::string("A"));
    EXPECT_EQ(StringUtils::Slice(singleChar, 1, 1), std::string(""));
    //EXPECT_EQ(StringUtils::Slice(singleChar, -1, 0), std::string(""));

    //additional cases to ensure correctness
    EXPECT_EQ(StringUtils::Slice(testString, -100, 7), std::string("TestFor"));
    EXPECT_EQ(StringUtils::Slice(testString, -100, 100), std::string("TestForSlice"));
    EXPECT_EQ(StringUtils::Slice(testString, 4, 100), std::string("ForSlice"));

    EXPECT_EQ(StringUtils::Slice(testString, -100, -8), std::string("Test"));
    EXPECT_EQ(StringUtils::Slice(testString, -8, 7), std::string("For"));



}

TEST(StringUtilsTest, Capitalize){
    //construct test string 
    const std::string emptyString = "";
    const std::string singleChar = "a";
    const std::string mixedString = "TestForCapitalize";
    const std::string allUpper = "TESTFORCAPITALIZE";

    //normal cases
    EXPECT_EQ(StringUtils::Capitalize(mixedString),std::string("Testforcapitalize"));
    EXPECT_EQ(StringUtils::Capitalize(allUpper),std::string("Testforcapitalize"));

    //edge cases
    //edge case 1: empty string
    EXPECT_EQ(StringUtils::Capitalize(emptyString),std::string(emptyString));
    //edge case 2: single character string
    EXPECT_EQ(StringUtils::Capitalize(singleChar),std::string("A"));
    //edge case 3: number at start
    EXPECT_EQ(StringUtils::Capitalize("1test"),std::string("1test"));
    //edge case 4: special character at start
    EXPECT_EQ(StringUtils::Capitalize("#TEST"),std::string("#test"));
    //edge case 5: all spaces
    EXPECT_EQ(StringUtils::Capitalize("     "),std::string("     "));
    //edge case 6: space at start followed by letters
    EXPECT_EQ(StringUtils::Capitalize("  test"),std::string("  test"));
    EXPECT_EQ(StringUtils::Capitalize("  TEST TWO"),std::string("  test two"));
    //edge case 7: number at start followed by letters
    EXPECT_EQ(StringUtils::Capitalize("1TEST"),std::string("1test"));
    EXPECT_EQ(StringUtils::Capitalize("2test"),std::string("2test"));
    //edge case 8: special character at start followed by letters
    EXPECT_EQ(StringUtils::Capitalize("@!TEST"),std::string("@!test"));
    EXPECT_EQ(StringUtils::Capitalize("$#test"),std::string("$#test"));
}
 TEST(StringUtilsTest, Upper){
    const std::string mixedString = "TestForUpper";
    const std::string allLower = "testforupper";
    const std::string emptyString = "";
    const std::string allUpper = "TESTFORUPPER";

    //normal cases
    EXPECT_EQ(StringUtils::Upper(mixedString), std::string("TESTFORUPPER"));
    EXPECT_EQ(StringUtils::Upper(allLower), std::string("TESTFORUPPER"));

    //edge cases
    //edge case 1: empty string 
    EXPECT_EQ(StringUtils::Upper(emptyString), std::string(""));
    //edge case 2: single character string
    EXPECT_EQ(StringUtils::Upper("a"), std::string("A"));
    EXPECT_EQ(StringUtils::Upper("Z"), std::string("Z"));
    //edge case 3: string with numbers and special characters
    EXPECT_EQ(StringUtils::Upper("test123!@#"), std::string("TEST123!@#"));
    //edge case 4: string with spaces
    EXPECT_EQ(StringUtils::Upper("test for upper"), std::string("TEST FOR UPPER"));
    //edge case 5: string with mixed case and special characters
    EXPECT_EQ(StringUtils::Upper("TeSt@123"), std::string("TEST@123"));
    //edge case 6: string that is already all upper case
    EXPECT_EQ(StringUtils::Upper(allUpper), std::string("TESTFORUPPER"));
    //edge case 7: starting with spaces
    EXPECT_EQ(StringUtils::Upper("   testForUpper"), std::string("   TESTFORUPPER"));

 }

 TEST(StringUtilsTest, Lower){
    const std::string mixedString = "TestForLower";
    const std::string allUpper = "TESTFORLOWER";
    const std::string emptyString = "";
    const std::string allLower = "testforlower";
    
    //normal cases
    EXPECT_EQ(StringUtils::Lower(mixedString), std::string("testforlower"));
    EXPECT_EQ(StringUtils::Lower(allUpper), std::string("testforlower"));

    //edge cases
    //edge case 1: empty string
    EXPECT_EQ(StringUtils::Lower(emptyString), std::string(""));
    //edge case 2: single character string
    EXPECT_EQ(StringUtils::Lower("A"), std::string("a"));
    EXPECT_EQ(StringUtils::Lower("z"), std::string("z"));
    //edge case 3: string with numbers and special characters
    EXPECT_EQ(StringUtils::Lower("TEST123!@#"), std::string("test123!@#"));
    //edge case 4: string with spaces
    EXPECT_EQ(StringUtils::Lower("TEST FOR LOWER"), std::string("test for lower"));
    //edge case 5: string with mixed case and special characters
    EXPECT_EQ(StringUtils::Lower("TeSt@123"), std::string("test@123"));
    //edge case 6: string that is already all lower case
    EXPECT_EQ(StringUtils::Lower(allLower), std::string("testforlower"));
    //edge case 7: starting with spaces
    EXPECT_EQ(StringUtils::Lower("   TestForLower"), std::string("   testforlower"));
    
}

TEST(StringUtilsTest, LStrip){
    const std::string stringWithLeadingSpaces = "    TestLStrip";
    const std::string stringWithLeadingTabs = "\t\tTestLStrip";
    const std::string stringWithLeadingSpacesAndTabs = "  \t  TestLStrip";
    const std::string stringWithNewline = "\nTestLStrip";
    const std::string stringWithCarriageReturn= "\rTestLStrip";
    const std::string stringWithFormFeed = "\fTestLStrip";
    const std::string stringWithVerticalTab = "\vTestLStrip";
    const std::string stringWithoutLeadingWhitespace = "TestLStrip";
    const std::string emptyString = "";
    const std::string stringWithDifferentWhitespace = "  \t\n\r\v\f  TestLStrip";
    const std::string stringWithAllWhitespace = "  \t\n\r\v\f  ";
    

    //normal cases
    EXPECT_EQ(StringUtils::LStrip(stringWithLeadingSpaces), std::string("TestLStrip"));
    EXPECT_EQ(StringUtils::LStrip(stringWithLeadingTabs), std::string("TestLStrip"));
    EXPECT_EQ(StringUtils::LStrip(stringWithLeadingSpacesAndTabs), std::string("TestLStrip"));
    EXPECT_EQ(StringUtils::LStrip(stringWithNewline), std::string("TestLStrip"));
    EXPECT_EQ(StringUtils::LStrip(stringWithCarriageReturn), std::string("TestLStrip"));
    EXPECT_EQ(StringUtils::LStrip(stringWithFormFeed), std::string("TestLStrip"));
    EXPECT_EQ(StringUtils::LStrip(stringWithVerticalTab), std::string("TestLStrip"));
    EXPECT_EQ(StringUtils::LStrip(stringWithoutLeadingWhitespace), std::string("TestLStrip"));  
    EXPECT_EQ(StringUtils::LStrip(emptyString), std::string(""));
    
    //edge cases
    //edge case 1: string with different whitespace
    EXPECT_EQ(StringUtils::LStrip(stringWithDifferentWhitespace), std::string("TestLStrip"));
    //edge case 2: string with all whitespace
    EXPECT_EQ(StringUtils::LStrip(stringWithAllWhitespace), std::string(""));
   
}

TEST(StringUtilsTest, RStrip){
    const std::string stringWithTrailingSpaces = "TestRStrip    ";
    const std::string stringWithTrailingTabs = "TestRStrip\t\t";
    const std::string stringWithTrailingSpacesAndTabs = "TestRStrip  \t  ";     
    const std::string stringWithNewline = "TestRStrip\n";
    const std::string stringWithCarriageReturn= "TestRStrip\r";
    const std::string stringWithFormFeed = "TestRStrip\f";
    const std::string stringWithVerticalTab = "TestRStrip\v";
    const std::string stringWithoutTrailingWhitespace = "TestRStrip";
    const std::string emptyString = "";
    const std::string stringWithDifferentWhitespace = "TestRStrip  \t\n\r\v\f  ";
    const std::string stringWithAllWhitespace = "  \t\n\r\v\f  ";
  

    //normal cases
    EXPECT_EQ(StringUtils::RStrip(stringWithTrailingSpaces), std::string("TestRStrip"));
    EXPECT_EQ(StringUtils::RStrip(stringWithTrailingTabs), std::string("TestRStrip"));
    EXPECT_EQ(StringUtils::RStrip(stringWithTrailingSpacesAndTabs), std::string("TestRStrip"));
    EXPECT_EQ(StringUtils::RStrip(stringWithNewline), std::string("TestRStrip"));
    EXPECT_EQ(StringUtils::RStrip(stringWithCarriageReturn), std::string("TestRStrip"));
    EXPECT_EQ(StringUtils::RStrip(stringWithFormFeed), std::string("TestRStrip"));
    EXPECT_EQ(StringUtils::RStrip(stringWithVerticalTab), std::string("TestRStrip"));
    EXPECT_EQ(StringUtils::RStrip(stringWithoutTrailingWhitespace), std::string("TestRStrip"));  
    EXPECT_EQ(StringUtils::RStrip(emptyString), std::string(""));
    
    //edge cases
    //edge case 1: string with different whitespace
    EXPECT_EQ(StringUtils::RStrip(stringWithDifferentWhitespace), std::string("TestRStrip"));
    //edge case 2: string with all whitespace
    EXPECT_EQ(StringUtils::RStrip(stringWithAllWhitespace), std::string(""));
    //edge case 3: trailing whitespace only
    EXPECT_EQ(StringUtils::RStrip("Hello \n \r \t"), std::string("Hello"));
    
}

TEST(StringUtilsTest, Strip){
    const std::string stringWithLeadingAndTrailingSpaces = "    TestStrip    ";
    const std::string stringWithLeadingAndTrailingTabs = "\t\tTestStrip\t\t";
    const std::string stringWithLeadingAndTrailingSpacesAndTabs = "  \t  TestStrip  \t  ";     
    const std::string stringWithNewline = "\nTestStrip\n";
    const std::string stringWithCarriageReturn= "\rTestStrip\r";
    const std::string stringWithFormFeed = "\fTestStrip\f";
    const std::string stringWithVerticalTab = "\vTestStrip\v";
    const std::string stringWithoutWhitespace = "TestStrip";
    const std::string emptyString = "";
    const std::string stringWithDifferentWhitespace = "  \t\n\r\v\f  TestStrip  \t\n\r\v\f  ";
    const std::string stringWithAllWhitespace = "  \t\n\r\v\f  ";

    //normal cases
    EXPECT_EQ(StringUtils::Strip(stringWithLeadingAndTrailingSpaces), std::string("TestStrip"));
    EXPECT_EQ(StringUtils::Strip(stringWithLeadingAndTrailingTabs), std::string("TestStrip"));
    EXPECT_EQ(StringUtils::Strip(stringWithLeadingAndTrailingSpacesAndTabs), std::string("TestStrip"));
    EXPECT_EQ(StringUtils::Strip(stringWithNewline), std::string("TestStrip"));
    EXPECT_EQ(StringUtils::Strip(stringWithCarriageReturn), std::string("TestStrip"));
    EXPECT_EQ(StringUtils::Strip(stringWithFormFeed), std::string("TestStrip"));
    EXPECT_EQ(StringUtils::Strip(stringWithVerticalTab), std::string("TestStrip"));
    EXPECT_EQ(StringUtils::Strip(stringWithoutWhitespace), std::string("TestStrip"));  
    EXPECT_EQ(StringUtils::Strip(emptyString), std::string(""));
    
    //edge cases
    //edge case 1: string with different whitespace
    EXPECT_EQ(StringUtils::Strip(stringWithDifferentWhitespace), std::string("TestStrip"));
    //edge case 2: string with all whitespace
    EXPECT_EQ(StringUtils::Strip(stringWithAllWhitespace), std::string(""));
    
}

 TEST(StringUtilsTest, Center){
    const std::string testString = "Test";
    const std::string emptyString = "";

    //normal cases
    EXPECT_EQ(StringUtils::Center(testString, 9), std::string("   Test  "));
    EXPECT_EQ(StringUtils::Center(testString, 10, '*'), std::string("***Test***"));
    EXPECT_EQ(StringUtils::Center(testString, 9, '-'), std::string("---Test--"));
    //edge cases
    //edge case 1: width less than string length
    EXPECT_EQ(StringUtils::Center(testString, 2, '*'), std::string("Test"));
    //edge case 2: width equal to string length
    EXPECT_EQ(StringUtils::Center(testString, 4, '*'), std::string("Test"));
    //edge case 3: empty string
    EXPECT_EQ(StringUtils::Center(emptyString, 6, '-'), std::string("------"));
    //edge case 4: width is zero
    EXPECT_EQ(StringUtils::Center(testString, 0, '*'), std::string("Test"));
    //edge case 5: width is negative
    EXPECT_EQ(StringUtils::Center(testString, -5, '*'), std::string("Test"));
    EXPECT_EQ(StringUtils::Center(emptyString, -9, '-'), std::string(""));

 }

 TEST(StringUtilsTest, LJust){
    const std::string testString = "Test";
    const std::string emptyString = "";

    //normal cases
    EXPECT_EQ(StringUtils::LJust(testString, 8), std::string("Test    "));
    EXPECT_EQ(StringUtils::LJust(testString, 10, '*'), std::string("Test******"));
    EXPECT_EQ(StringUtils::LJust(testString, 9, '-'), std::string("Test-----"));
    
    // //edge cases
    //edge case 1: width less than string length
    EXPECT_EQ(StringUtils::LJust(testString, 2, '*'), std::string("Test"));
    //edge case 2: width equal to string length
    EXPECT_EQ(StringUtils::LJust(testString, 4, '*'), std::string("Test"));
    //edge case 3: empty string
    EXPECT_EQ(StringUtils::LJust(emptyString, 5, '-'), std::string("-----"));
    //edge case 4: width is zero
    EXPECT_EQ(StringUtils::LJust(testString, 0, '*'), std::string("Test"));
    //edge case 5: width is negative
    EXPECT_EQ(StringUtils::LJust(testString, -3, '*'), std::string("Test"));
    EXPECT_EQ(StringUtils::LJust(emptyString, -7, '-'), std::string(""));

 }

 TEST(StringUtilsTest, RJust){
    const std::string testString = "Test";
    const std::string emptyString = "";

    //normal cases
    EXPECT_EQ(StringUtils::RJust(testString, 8), std::string("    Test"));
    EXPECT_EQ(StringUtils::RJust(testString, 10, '*'), std::string("******Test"));
    EXPECT_EQ(StringUtils::RJust(testString, 9, '-'), std::string("-----Test"));
    
    //edge cases
    //edge case 1: width less than string length
    EXPECT_EQ(StringUtils::RJust(testString, 2, '*'), std::string("Test"));
    //edge case 2: width equal to string length
    EXPECT_EQ(StringUtils::RJust(testString, 4, '*'), std::string("Test"));
    //edge case 3: empty string
    EXPECT_EQ(StringUtils::RJust(emptyString, 5, '-'), std::string("-----"));
    //edge case 4: width is zero
    EXPECT_EQ(StringUtils::RJust(testString, 0, '*'), std::string("Test"));
    //edge case 5: width is negative
    EXPECT_EQ(StringUtils::RJust(testString, -3, '*'), std::string("Test"));
    EXPECT_EQ(StringUtils::RJust(emptyString, -7, '-'), std::string(""));
    
 }

 TEST(StringUtilsTest, Replace){
    const std::string testString = "Hello world, this is Haotian.";
    const std::string emptyString = "";

    //normal cases
    //case1: single character replacement
    EXPECT_EQ(StringUtils::Replace(testString, "o", "a"), std::string("Hella warld, this is Haatian."));
    //case2: multi-character replacement
    EXPECT_EQ(StringUtils::Replace(testString, "is", "was"), std::string("Hello world, thwas was Haotian."));
    //case3: replacement with empty string
    EXPECT_EQ(StringUtils::Replace(testString, " ", ""), std::string("Helloworld,thisisHaotian."));
    //case4: replacing something not present
    EXPECT_EQ(StringUtils::Replace(testString, "Python", "C++"), std::string("Hello world, this is Haotian."));
    //case5: replacing with same string
    EXPECT_EQ(StringUtils::Replace(testString, "Haotian", "Haotian"), std::string("Hello world, this is Haotian."));
    
    //edge cases
    //edge case1: original string is empty
    EXPECT_EQ(StringUtils::Replace(emptyString, "a", "b"), std::string(""));
    //edge case2: old string is empty
    EXPECT_EQ(StringUtils::Replace(testString, "", "X"), std::string("XHXeXlXlXoX XwXoXrXlXdX,X XtXhXiXsX XiXsX XHXaXoXtXiXaXnX.X"));
    //edge case3: new string is empty
    EXPECT_EQ(StringUtils::Replace(testString, "o", ""), std::string("Hell wrld, this is Hatian."));

    //special cases
    //case1: overlapping old strings
    EXPECT_EQ(StringUtils::Replace("aaaaa", "aa", "b"), std::string("bba"));
    //case2: white space characters
    EXPECT_EQ(StringUtils::Replace("a b c \t \n \t", "\t", "-"), std::string("a b c - \n -"));
    //case3: case sensitive replacement
    EXPECT_EQ(StringUtils::Replace("Hello hello HeLLo", "hello", "hi"), std::string("Hello hi HeLLo"));
    //case4: old string lomnger than original string
    EXPECT_EQ(StringUtils::Replace("short", "longerstring", "new"), std::string("short"));
 }

 TEST(StringUtilsTest, Split){
    const std::string testString = "one,two,three,four";
    const std::string testString2 = "A B C D E";
    const std::string testString3 = "12    34    56    78";
    const std::string testString4 = "|start|middle|end|";
    const std::string emptyString = "";
    const std::string testString5 = "\n a \t b\rc\n";
    const std::string testString6 = "one,,two,,three,,four";
    


    //default delimeter
    EXPECT_EQ(StringUtils::Split(testString2), std::vector<std::string>({"A", "B", "C", "D", "E"}));
    EXPECT_EQ(StringUtils::Split(testString3), std::vector<std::string>({"12", "34", "56", "78"}));
    EXPECT_EQ(StringUtils::Split(testString5), std::vector<std::string>({"a", "b", "c"}));
    
    //user-defined delimeter
    EXPECT_EQ(StringUtils::Split(testString, ","), std::vector<std::string>({"one", "two", "three", "four"}));
    EXPECT_EQ(StringUtils::Split(testString, "o"), std::vector<std::string>({"", "ne,tw", ",three,f", "ur"}));
    EXPECT_EQ(StringUtils::Split(testString, "x"), std::vector<std::string>({"one,two,three,four"})); 
    EXPECT_EQ(StringUtils::Split(testString4, "|"), std::vector<std::string>({"", "start", "middle", "end", ""}));
    EXPECT_EQ(StringUtils::Split(emptyString), std::vector<std::string>({})); 
   
    
    //edge cases
    //edge case 1: empty string
    EXPECT_EQ(StringUtils::Split(emptyString, ","), std::vector<std::string>({""}));
    //edge case 2: delimiter at start
    EXPECT_EQ(StringUtils::Split(",start,delimiter", ","), std::vector<std::string>({"", "start", "delimiter"}));
    //edge case 3: delimiter at end
    EXPECT_EQ(StringUtils::Split("end,delimiter,", ","), std::vector<std::string>({"end", "delimiter", ""}));
    //edge case 4: consecutive delimiters
    EXPECT_EQ(StringUtils::Split("one,,two,,,three", ","), std::vector<std::string>({"one", "", "two", "", "", "three"}));
    //edge case 5: delimiter is empty string
    EXPECT_EQ(StringUtils::Split("abc", ""), std::vector<std::string>({"abc"}));
    //edge case 6: string consists only of delimiters
    EXPECT_EQ(StringUtils::Split("A", "A"), std::vector<std::string>({"", ""}));
    EXPECT_EQ(StringUtils::Split(",,,", ","), std::vector<std::string>({"", "", "", ""}));
    //edge case 7: original string is empty
    EXPECT_EQ(StringUtils::Split("", ","), std::vector<std::string>({""}));
    EXPECT_EQ(StringUtils::Split(""), std::vector<std::string>({}));
    //edge case 8: no occurrence of delimiter
    EXPECT_EQ(StringUtils::Split("nodelemiterhere", ","), std::vector<std::string>({"nodelemiterhere"}));
    //edge case 9: multiple delimiters
    EXPECT_EQ(StringUtils::Split(testString6, ","), std::vector<std::string>({"one", "", "two", "", "three", "", "four"})); 
    //edge case 10: multiple delimiters of varying lengths
    EXPECT_EQ(StringUtils::Split("a--b----c--d", "--"), std::vector<std::string>({"a", "b", "", "c", "d"}));
    //edge case 11: delimiter with multiple characters and at the edges
    EXPECT_EQ(StringUtils::Split(",,,,one,,two,,,,three", ",,"), std::vector<std::string>({"", "", "one", "two", "", "three"}));
   
  

 }

 TEST(StringUtilsTest, Join){

    const std::string emptyString = "";
    const std::string comma = ",";
    const std::string space = " ";
    const std::string chararcter = "A";
    const std::string dash = "-";

    const std::vector<std::string> vec1 = {"one", "two", "three"};
    const std::vector<std::string> vec2 = {"Hello", "world", "from", "Haotian"};
    const std::vector<std::string> vec3 = {};
    const std::vector<std::string> vec4 = {"singleElement"};
    const std::vector<std::string> vec5 = {"a","","b"};


    //normal cases
    EXPECT_EQ(StringUtils::Join(comma, vec1), std::string("one,two,three"));
    EXPECT_EQ(StringUtils::Join(space, vec2), std::string("Hello world from Haotian"));
    EXPECT_EQ(StringUtils::Join(dash, vec1), std::string("one-two-three"));
    EXPECT_EQ(StringUtils::Join(chararcter, vec1), std::string("oneAtwoAthree"));

    //edge cases
    //edge case 1: empty vector
    EXPECT_EQ(StringUtils::Join(comma, vec3), std::string(""));
    //edge case 2: single element vector
    EXPECT_EQ(StringUtils::Join(chararcter, vec4), std::string("singleElement"));
    //edge case 3: delimiter is empty string
    EXPECT_EQ(StringUtils::Join(emptyString, vec1), std::string("onetwothree"));
    //edge case 4: vector with empty strings
    EXPECT_EQ(StringUtils::Join(comma, vec5), std::string("a,,b"));
    
 }

 TEST(StringUtilsTest, ExpandTabs){
    const std::string strWithTabStart = "\tTestExpandTabs";
    const std::string strWithTabMid1 = "a\tTest";
    const std::string strWithTabMid3 = "abc\tTest";
    const std::string strWithTabMid4 = "abcd\tTest";
    const std::string strMultipleTabs = "Test\tExpand\tTabs";
    const std::string strJustTabs = "\t\t\t";
    const std::string emptyString = "";
    const std::string strNoTabs = "ThisStringHasNoTabs";
    const std::string tabMixedWithSpaces = "a \t b \t c";
    const std::string tabMixedWithNewline = "a\tb\nc\td";   


    //normal cases
    EXPECT_EQ(StringUtils::ExpandTabs(strWithTabStart, 4), std::string("    TestExpandTabs"));
    EXPECT_EQ(StringUtils::ExpandTabs(strWithTabMid1, 4), std::string("a   Test"));
    EXPECT_EQ(StringUtils::ExpandTabs(strWithTabMid3, 4), std::string("abc Test"));
    EXPECT_EQ(StringUtils::ExpandTabs(strWithTabMid4, 4), std::string("abcd    Test"));
    EXPECT_EQ(StringUtils::ExpandTabs(strMultipleTabs, 4), std::string("Test    Expand  Tabs"));

    //Different tab sizes
    EXPECT_EQ(StringUtils::ExpandTabs(strWithTabStart, 2), std::string("  TestExpandTabs"));
    EXPECT_EQ(StringUtils::ExpandTabs(strWithTabMid1, 2), std::string("a Test"));
    EXPECT_EQ(StringUtils::ExpandTabs(strWithTabMid3, 3), std::string("abc   Test"));
    EXPECT_EQ(StringUtils::ExpandTabs(strWithTabMid4, 2), std::string("abcd  Test"));
    EXPECT_EQ(StringUtils::ExpandTabs(strMultipleTabs, 2), std::string("Test  Expand  Tabs"));
    
    //edge cases
    //edge case 1: string with only tabs
    EXPECT_EQ(StringUtils::ExpandTabs(strJustTabs, 4), std::string("            "));
    //edge case 2: empty string
    EXPECT_EQ(StringUtils::ExpandTabs(emptyString, 4), std::string(""));
    //edge case 3: string with no tabs
    EXPECT_EQ(StringUtils::ExpandTabs(strNoTabs, 4), std::string("ThisStringHasNoTabs"));
    //edge case 4: tab size is 0
    EXPECT_EQ(StringUtils::ExpandTabs(strWithTabMid1, 0), std::string("aTest"));
    //edge case 5: tab size is negative
    EXPECT_EQ(StringUtils::ExpandTabs(strWithTabMid1, -3), std::string("aTest"));
    EXPECT_EQ(StringUtils::ExpandTabs(strMultipleTabs, -1), std::string("TestExpandTabs"));
    //edge case 6: tab mixed with spaces
    EXPECT_EQ(StringUtils::ExpandTabs(tabMixedWithSpaces, 4), std::string("a    b   c"));
    //edge case 7: tab mixed with newline
    EXPECT_EQ(StringUtils::ExpandTabs(tabMixedWithNewline, 4), std::string("a   b\nc   d"));
 }

 TEST(StringUtilsTest, EditDistance){
    const std::string str1 = "kitten"; //example from Wikipedia
    const std::string str2 = "sitting";
    const std::string str3 = "flaw";
    const std::string str4 = "lawn";
    const std::string str5 = "Haotian";
    const std::string str6 = "haotian";
    const std::string emptyString = "";

    //normal cases
    EXPECT_EQ(StringUtils::EditDistance(str1, str2, false), 3);
    EXPECT_EQ(StringUtils::EditDistance(str3, str4, false), 2);
    EXPECT_EQ(StringUtils::EditDistance("cat", "cats", false), 1);
    EXPECT_EQ(StringUtils::EditDistance("cats", "cat", false), 1);
    EXPECT_EQ(StringUtils::EditDistance("cat", "cut", false), 1);

    //edge cases
    //edge case 1: one string is empty
    EXPECT_EQ(StringUtils::EditDistance(emptyString, str1, false), str1.length());
    EXPECT_EQ(StringUtils::EditDistance(str2, emptyString, false), str2.length());
    //edge case 2: both strings are empty
    EXPECT_EQ(StringUtils::EditDistance(emptyString, emptyString, false), 0);
    //edge case 3: identical strings
    EXPECT_EQ(StringUtils::EditDistance(str1, str1, false), 0);
    //edge case 4: case sensitivity
    EXPECT_EQ(StringUtils::EditDistance(str5, str6, false), 1);
    EXPECT_EQ(StringUtils::EditDistance(str5, str6, true), 0);
    EXPECT_EQ(StringUtils::EditDistance("Apple", "aPPLE", false), 5);
    EXPECT_EQ(StringUtils::EditDistance("Apple", "aPPLE", true), 0);
    //After coverage tests, add 2 more cases where after all lowered, two strings are different
    // one is first chars are the same and the other test is first elements are different 
    EXPECT_EQ(StringUtils::EditDistance("Apple", "Banana", true), 5);
    EXPECT_EQ(StringUtils::EditDistance("Apple", "abandon", true), 6);


 }
