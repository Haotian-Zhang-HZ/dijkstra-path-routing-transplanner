#include "XMLWriter.h"
#include <stack>

//definition of the Simplementation struct
struct CXMLWriter::SImplementation{
    std::shared_ptr< CDataSink > DSink;
    std::stack<std::string> Stack;//This stack keeps track of open tags 

    SImplementation(std::shared_ptr< CDataSink > sink){
        DSink = sink;
    }
    ~SImplementation(){

    }
    //This function writes an XML entity to the data sink
    //it gets an SXMLEntity object as input and writes it to the data sink in XML format according to its type
    bool WriteEntity(const SXMLEntity &entity){
        switch(entity.DType){
            //if it's a start element, write it to data sink and push the tag name onto the stack
            case SXMLEntity::EType::StartElement:{
                //write to data sink
                DSink->Put('<');//write opening angle bracket [<]
                DSink->Write(std::vector<char>(entity.DNameData.begin(),entity.DNameData.end()));//Dsink->Write takes a vector<char>, so we convert string to vector<char>
                //At this stage, [<tag]
                //for each attribute, write it in the format of name=\"value\"
                for(const TAttribute &attri : entity.DAttributes){
                    DSink->Put(' ');//We put a space before attribute [<tag ]
                    DSink->Write(std::vector<char>(std::get<0>(attri).begin(),std::get<0>(attri).end()));//write attribute name. At this stage [<tag attr1]
                    DSink->Put('=');//At this stage [<tag attr1="]
                    DSink->Put('\"');//At this stage [<tag attr1=\"]
                    for(const char &ch : std::get<1>(attri)){
                        WriteEscape(ch);//we should also write escape char in the attribute.
                    }
                    DSink->Put('\"');//closing quote //At this stage [<tag attr1=\"value1\"]
                }
                DSink->Put('>');//closing angle bracket [<tag attr1=\"value1\">]
                Stack.push(entity.DNameData);//push the tag name onto the stack
                return true;
                break;
            }
            case SXMLEntity::EType::EndElement:{
                DSink->Put('<');//write opening angle bracket [<]
                DSink->Put('/');//write slash for end tag [</]
                DSink->Write(std::vector<char>(entity.DNameData.begin(),entity.DNameData.end()));//Dsink->Write takes a vector<char>, so we convert string to vector<char>
                //At this stage, [</tag]
                DSink->Put('>');//closing angle bracket [</tag>]
                //pop the tag name from the stack
                //Sanity check: only pop if the stack is not empty and the top of the stack matches the end tag name
                if(!Stack.empty() && Stack.top() == entity.DNameData){
                    Stack.pop();
                    return true;
                }
                else{
                    return false; //mismatched end tag
                }
                break;
            }
            case SXMLEntity::EType::CharData:{
                //iterate through each character in the character data and write to data sink after handling special characters
                for(const char &ch : entity.DNameData){
                    WriteEscape(ch);
                }
                break;
            }
            case SXMLEntity::EType::CompleteElement:{
                DSink->Put('<');//write opening angle bracket [<]
                DSink->Write(std::vector<char>(entity.DNameData.begin(),entity.DNameData.end()));//Dsink->Write takes a vector<char>, so we convert string to vector<char>
                //At this stage, [<tag]
                //for each attribute, write it in the format of name=\"value\"
                for(const TAttribute &attri : entity.DAttributes){
                    DSink->Put(' ');//We put a space before attribute [<tag ]
                    DSink->Write(std::vector<char>(std::get<0>(attri).begin(),std::get<0>(attri).end()));//write attribute name. At this stage [<tag attr1]
                    DSink->Put('=');//At this stage [<tag attr1="]
                    DSink->Put('\"');//At this stage [<tag attr1=\"]
                    for(const char &ch : std::get<1>(attri)){
                        WriteEscape(ch);//we should also write escape char in the attribute.
                    }
                    DSink->Put('\"');//closing quote //At this stage [<tag attr1=\"value1\"]
                }
                DSink->Put('/');//self-closing slash [<tag attr1=\"value1\"/]
                DSink->Put('>');//closing angle bracket [<tag attr1=\"value1\">]
                //Do not push the tag name onto the stack
                return true;
                break;
            }
        }
        return true; //return true if successful
    }
    
    bool Flush(){
        while(!Stack.empty()){
            //get tag name
            std::string TopTag = Stack.top();
            //write end tag
            DSink->Put('<');//write opening angle bracket [<]
            DSink->Put('/');//write slash for end tag [</]
            DSink->Write(std::vector<char>(TopTag.begin(),TopTag.end()));//Dsink->Write takes a vector<char>, so we convert string to vector<char>
            DSink->Put('>');//closing angle bracket [</tag>]
            Stack.pop();
        }
        return true; 
    }
    bool WriteEscape(const char &ch){
        switch(ch){
            case '&':
                DSink->Write(std::vector<char>{'&','a','m','p',';'});//& -> &amp;
                break;
            case '<':
                DSink->Write(std::vector<char>{'&','l','t',';'});//< -> &lt;
                break;
            case '>':
                DSink->Write(std::vector<char>{'&','g','t',';'});//> -> &gt;
                break;
            case '\"':
                DSink->Write(std::vector<char>{'&','q','u','o','t',';'});//" -> &quot;
                break;
            case '\'':
                DSink->Write(std::vector<char>{'&','a','p','o','s',';'});//' -> &apos;
                break;
            default:
                DSink->Put(ch);//regular character, just write 
        } 
        return true;      
    }
};
       
        


CXMLWriter::CXMLWriter(std::shared_ptr< CDataSink > sink){
    DImplementation = std::make_unique<SImplementation>(sink);
}

CXMLWriter::~CXMLWriter(){

}
        
bool CXMLWriter::Flush(){
    return DImplementation->Flush();
}
bool CXMLWriter::WriteEntity(const SXMLEntity &entity){
    return DImplementation->WriteEntity(entity);
}
