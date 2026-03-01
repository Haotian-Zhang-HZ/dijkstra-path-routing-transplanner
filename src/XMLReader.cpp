#include "XMLReader.h"
#include <expat.h>
#include <queue>

// <tag attr="val">CharData</tag>

struct CXMLReader::SImplementation{
    std::shared_ptr< CDataSource > DSource;//datasource to read XML data from
    XML_Parser DParser;//expat parser object
    std::queue<SXMLEntity> DQueue;//queue to hold parsed XML entities

    // <tag attr="val">
    //userData is  a pointer, name is tag, atts is array of attributes (key,value,key,value,...,nullptr)
    //callback function called by expat library when a start element is encountered
    static void StartElementHandler(void *userData, const XML_Char *name, const XML_Char **atts){
        //this functionsignature is defined by expat library we cannot chage void *userData to SImplementation*
        SImplementation *This = (SImplementation *)userData;//cast user data back to our implementation struct
        SXMLEntity NewEntity;//initialize a new XML entity
        
        NewEntity.DType = SXMLEntity::EType::StartElement;//set type to start element since we are in start element handler
        NewEntity.DNameData = name;//set the tag name
        //process attributes
        for(int i = 0; atts[i]; i += 2){
            NewEntity.DAttributes.push_back({atts[i],atts[i+1]});
        }
        This->DQueue.push(NewEntity);
    }
    //</tag>
    //callback function called by expat library when an end element is encountered
    static void EndElementHandler(void *userData, const XML_Char *name){
        SImplementation *This = (SImplementation *)userData;
        SXMLEntity NewEntity;
        
        NewEntity.DType = SXMLEntity::EType::EndElement;
        NewEntity.DNameData = name;
        This->DQueue.push(NewEntity);
    }
    //CharData
    //callback function called by expat library when character data is encountered
    static void CharacterDataHandler(void *userData, const XML_Char *s, int len){
        SImplementation *This = (SImplementation *)userData;
        SXMLEntity NewEntity;
        
        NewEntity.DType = SXMLEntity::EType::CharData;
        NewEntity.DNameData = std::string(s,len);//string constructor that takes char pointer and length
        This->DQueue.push(NewEntity);
    }

    SImplementation(std::shared_ptr< CDataSource > src){
        DSource = src;//store datasource
        DParser = XML_ParserCreate(nullptr);//API provided by expat library, create parser instance with default encoding (nullptr) UTF-8
        XML_SetUserData(DParser,(void *)this);//API provided by expat library, set user data to this implementation struct pointer.(void *)this is the ptr to this SImplementation struct 
        // binding the parser with the 'SImplementation' object so that the the parser can access the data source and queue inside the 'SImplementation' object during parsing.
        XML_SetStartElementHandler(DParser,StartElementHandler);//register start element handler
        // XML_SetStartElementHandler is a function that will ensure when the parser encounters the start tag, please call this function StartElementHandler. 
        // After registration, we do not directly call StartElementHandler, the parser will automatically call it when needed.
        XML_SetEndElementHandler(DParser,EndElementHandler);//register end element handler
        XML_SetCharacterDataHandler(DParser,CharacterDataHandler);//register character data handler

    }

    bool End() const{
        // End when we reach the end of the data source and the queue is empty
        if(DSource->End() && DQueue.empty()){
            return true;
        }
        return false;
    }

    //this function reads and parses XML data
    //first it checks if the queue is empty, if it is, it reads more data from the datasource and feeds it to the expat parser 
    //and put parsed entities into the queue
    // we finish parsing or there is already data in the queue, we pop the front entity from the queue and return it
    bool ReadEntity(SXMLEntity &entity, bool skipcdata){
        //if queue is empty, read more data from datasource and parse it
        if(DQueue.empty()){
            std::vector<char> Buffer(512);
            DSource->Read(Buffer,Buffer.size());
            XML_Parse(DParser,Buffer.data(),Buffer.size(),DSource->End());
            // function signature: XML_Parse(XML_Parser parser, const char *s, int len, int isFinal);
            // this function feeds data to the parser by providing a pointer to the data (Buffer.data()), the length of the data (Buffer.size()), and a flag indicating whether this is the final chunk of data (DSource->End()). 
            // the fucntion of this function is to process the provided data and invoke the appropriate callback functions (StartElementHandler, EndElementHandler, CharacterDataHandler) based on the content of the XML data.
        }
        while(!DQueue.empty()){
            entity = DQueue.front();
            DQueue.pop();//we don't need to store the entity because callback has already stored it in 'entity'
            // check if we need to skip char data entities
            if(skipcdata && entity.DType == SXMLEntity::EType::CharData){
                continue;
            }
            return true;
        }

        return false;
    }

};

        

CXMLReader::CXMLReader(std::shared_ptr< CDataSource > src){
    DImplementation = std::make_unique<SImplementation>(src);
}

CXMLReader::~CXMLReader(){

}

bool CXMLReader::End() const{
    return DImplementation->End();
}

bool CXMLReader::ReadEntity(SXMLEntity &entity, bool skipcdata){
    return DImplementation->ReadEntity(entity,skipcdata);
}
