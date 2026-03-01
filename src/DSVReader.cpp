#include "DSVReader.h"
#include "StringUtils.h"


struct CDSVReader::SImplementation{
    std::shared_ptr< CDataSource > DSource;
    char DDelimiter;

    //constructor
    SImplementation(std::shared_ptr< CDataSource > src, char delimiter){
        DSource = src;
        DDelimiter = delimiter;
    }   

    ~SImplementation(){

    }
    //This function is very complicated due to the various edge cases we need to consider when reading DSV data.
    // So I draw a state machine to handle different scenarios.
    // Thi sfunction will read a string from the data source until it reaches the end of a row (newline character) or the end of the data source.
    // and push each field into the same row vector. 
    bool ReadRow(std::vector<std::string> &row){
        row.clear();//clear the row vector before reading new data
        std::string CurrentField;//keep track of the current field being read
        char CurrentChar;//keep track of the current character being processed
        bool InQuotes = false;//state variable to track if we are inside quotes
        while(DSource->Get(CurrentChar)){
            if(InQuotes){     
                //if we are in quotes and we encounter a quote character
                if(CurrentChar =='\"'){
                    char NextChar;
                    DSource->Peek(NextChar);//peek at the next character without consuming it
                    //if the next character is also a quote, it's an escaped quote
                    if(NextChar == '\"'){
                        //if the next character is also a quote, it's an escaped quote
                        CurrentField += '\"';//add a single quote to the current field
                        DSource->Get(NextChar);//consume the next quote character so that it won't be processed again
                    }
                    else{
                        //if the next character is not a quote, we are exiting the quoted field 
                        // ****** but we shouldn't push the current field to the row at this stage 
                        // for example, in the case of "A,B",C, after reading "A,B", we encounter a quote, we are exiting the quoted field
                        // if we push the current field to the row here, the  next iteratrion will process the comma outside of quote 
                        // and push an empty field to the row, which is incorrect
                        InQuotes = false;
                    }    
                }
                else{
                    //regular character inside quotes, just add to the current field
                    CurrentField += CurrentChar;
                }
            }
            //Below is the situation when we are not in quotes
            else{
                //if we get a delimiter, it means the current field has ended
                if(CurrentChar == DDelimiter){
                    row.push_back(CurrentField);
                    CurrentField.clear();
                }
                //if wen get a newline character outside the quote, it means the row has ended
                else if(CurrentChar == '\n'){
                    row.push_back(CurrentField);
                    return true;//row read successfully
                }
                //if we get a quote character, it means we are entering a quoted field
                else if(CurrentChar == '\"'){
                    InQuotes = true;
                }
                //regular character, just add to the current field
                else{
                    CurrentField += CurrentChar;
                }
            }
        }
        //Here we have reached the end of row or end of file
        // if the end of file is reached, we need to push the last field to the row unless both the current field and the row are empty
        //case 1：“A，B，C” ->After reading C, we reach the end of file, but we haven't pushed C to the row
        //case 2: "A,B," -> After Reaching the last comma, we reach the end of file, but we need to push an empty string to the row 
        //and we cannot do this without checking if the current row is empty
        if(!CurrentField.empty() || !row.empty()){
            row.push_back(CurrentField);
            return true;;
        }
        return false;//no more data to read
    }
};


CDSVReader::CDSVReader(std::shared_ptr< CDataSource > src, char delimiter){
    DImplementation = std::make_unique<SImplementation>(src,delimiter);
}

CDSVReader::~CDSVReader(){
    
}
        
bool CDSVReader::End() const{
    return DImplementation->DSource->End();
}

bool CDSVReader::ReadRow(std::vector<std::string> &row){
    return DImplementation->ReadRow(row);
}



