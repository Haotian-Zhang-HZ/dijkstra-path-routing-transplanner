#include "DSVWriter.h"
#include "StringUtils.h"
//definition of the Simplementation struct
struct CDSVWriter::SImplementation{
    std::shared_ptr< CDataSink > DSink;
    char DDelimiter;
    bool DQuoteAll;
    bool DFirstRow = true;


    //constructor
    SImplementation(std::shared_ptr< CDataSink > sink, char delimiter, bool quoteall){
        DSink = sink;
        DDelimiter = delimiter=='\"' ? ',' : delimiter; //if the delimiter is a quote, we interpret it as comma
        DQuoteAll = quoteall;
    }

    ~SImplementation(){

    }
    //helper function 1: write quoted value (Professor said in discussion3 that we are welcome to create any helper functions)
    //I enclosed this function because it can be reused in multiple places like quoteall,containing delimiter,quote,newline
    void WriteQuotedValue(const std::string &Column, bool &First){
        std::vector<char> Buffer(Column.begin(),Column.end());//create a buffer to hold the column data. Syntax:vector(InputIterator first, InputIterator last); copy all the characters from the string to the vector
                if(!First){
                    DSink->Put(DDelimiter);//if not the first column, add delimiter before the column
                }
                First = false;
                DSink->Put('\"');//add prefix quote
                DSink->Write(Buffer);
                DSink->Put('\"');//add suffix quote
    }

    //helper function 2: write regular value
    void WriteRegularValue(const std::string &Column, bool &First){
        std::vector<char> Buffer(Column.begin(),Column.end());//create a buffer to hold the column data. Syntax:vector(InputIterator first, InputIterator last); copy all the characters from the string to the vector
                if(!First){
                    DSink->Put(DDelimiter);//if not the first column, add delimiter before the column
                }
                First = false;
                DSink->Write(Buffer);             
    }



    //method to write a row of data taking advantage of the CDataSink interface
    //original data is in the form of vector of strings, here, the "row" parameter.
    //there are 3 special cases to consider when writing data:
    //1. If a value contains the delimiter character, it needs to be quoted.
    //2. If a value contains a quote character, the quote character itself needs to be doubled and the whole value needs to be quoted.
    //3. If a value contains a newline character, it needs to be quoted.
    //Another special issue is that when we have multiple calls to WriteRow, newline characters should be added between rows but not after the last row.
    bool WriteRow(const std::vector<std::string> &row){
        if (!DFirstRow){
            DSink->Put('\n');//add newline character before writing a new row if it's not the first row
        }
        DFirstRow = false;
        bool First = true;          
        //iterate through each column in the row
        // auto keyword is used for type inference
        // & Means we are using reference to avoid copying the string

        /* This is my previous implementation, which turned out to be incorrect.
        //     //double quotes for all fields
        //     for(auto &Column : row){
        //         WriteQuotedValue(Column,First);
        //     }
        // } 
        // else{
        */
            for(auto &Column : row){
                //check for special characters
                //if the column contains a quote character,replace each quote with double quotes and quote the whole value
                if(Column.find("\"") != std::string::npos){
                    std::string ChangedColumn = StringUtils::Replace(Column,"\"","\"\"");//replace each quote with double quotes
                    WriteQuotedValue(ChangedColumn,First);
                }
                //contains delimiter, or newline,but not double quote just double quote.
                // I do not use another if statement here to avoid the double quoting twice in the case where a value contains both quote and delimiter/newline
                else if(Column.find(DDelimiter) != std::string::npos || Column.find('\n') != std::string::npos){
                    //value contains delimiter, quote, or newline
                    WriteQuotedValue(Column,First);
                }
                //we should not put it before the loop incase quotea=true while there is " in the value
                //I used to put it in an if and but the loop in the else but then in the case where quoteall=true and there is " in the value, it will not do "->"". 
                else if(DQuoteAll == true){           
                    WriteQuotedValue(Column,First);
                }
                else{
                    //regular value
                    WriteRegularValue(Column,First);
                }
            }
        
        
        return true;
    }

};

//CDSVWriter constructor : when the constructor is called, it creates an instance of SImplementation
//and the constructor of SImplementation is called with the provided parameters.
//DImplementation is a unique pointer that manages the lifetime of the SImplementation instance and it has 
// writerow function which can access the sink and use Dsink interface(Put/ write) to implemente the writing of data.
// And this hides the implementation details from the users of CDSVWriter class. 
// And we can simply use the writerow function of CDSVWriter class to write data without worrying about the logic behind.
CDSVWriter::CDSVWriter(std::shared_ptr< CDataSink > sink, char delimiter, bool quoteall){
    DImplementation = std::make_unique<SImplementation>(sink,delimiter,quoteall);
}

CDSVWriter::~CDSVWriter(){

}

bool CDSVWriter::WriteRow(const std::vector<std::string> &row){
    return DImplementation->WriteRow(row);
}
