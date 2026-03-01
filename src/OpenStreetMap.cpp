#include "OpenStreetMap.h"
#include <unordered_map>

struct COpenStreetMap::SImplementation{
    const std::string DOSMTag = "osm";//we don't want to put string literals as parameters, in case we have typo and if we want to change anything, we can just change it here.
    const std::string DNodeTag = "node";
    const std::string DAttributeTag = "tag";
    const std::string DAttributeKeyTag = "k";
    const std::string DAttributeValueTag = "v";
    const std::string DWayTag = "way";
    const std::string DNodeIDAttr = "id";
    const std::string DWayIDAttr = "id";
    const std::string DWayNodeTag = "nd";
    const std::string DWayNodeIDAttr = "ref";
    const std::string DNodeLatAttr = "lat";
    const std::string DNodeLonAttr = "lon";

    /////////////////////////////////// SNode ///////////////////////////////////
    struct SNode: public CStreetMap::SNode{

        TNodeID DID;
        SLocation DLocation;

        std::vector<std::string> DAttributeKeys;
        std::unordered_map<std::string,std::string> DAttributesMap;

        ~SNode(){};

        // Returns the id of the SNode
        TNodeID ID() const noexcept override{
            return DID;
        }

        // Returns the lat/lon location of the SNode
        SLocation Location() const noexcept override{
            return DLocation;
        }

        // Returns the number of attributes attached to the SNode
        std::size_t AttributeCount() const noexcept override{
            return DAttributeKeys.size();
        }

        // Returns the key of the attribute at index, returns empty string if index
        // is greater than or equal to AttributeCount()
        std::string GetAttributeKey(std::size_t index) const noexcept override{
            if(index >= AttributeCount()){
                return "";
            }
            else{
                return DAttributeKeys[index];
            }
        }

        //std::unordered_map::find(key) will return a iterator pointing to key-value pair if key exists, else return std::unordered_map::end() 
        // Returns if the attribute is attached to the SNode
        bool HasAttribute(const std::string &key) const noexcept override{
            return DAttributesMap.find(key) != DAttributesMap.end();
        }

        // Returns the value of the attribute specified by key, returns empty string  
        // if key hasn't been attached to SNode
        std::string GetAttribute(const std::string &key) const noexcept override{
            auto it = DAttributesMap.find(key);//this will return an iterator pointing to the key value pair if key exists
            if(it != DAttributesMap.end()){
                return it->second;
            }
            else{
                return "";
            }
        }
    };
    ////////////////////////////END OF SNODE/////////////////////////////////


    ////////////////////////////START OF SWAY////////////////////////////////
    struct SWay: public CStreetMap::SWay{

        TWayID DWayID;
        std::vector<TNodeID> DNodes;//This vector stores the Node in this way
        std::vector<std::string> DAttributeKeys;// This vector stores the attribute keys of this way 
        std::unordered_map<std::string,std::string> DAttributesMap;// This unordered map stores the attributes key-value pair ofthis way.


        ~SWay(){};

        // Returns the id of the SWay
        TWayID ID() const noexcept override{
            return DWayID;
        }

        // Returns the number of nodes in the way
        std::size_t NodeCount() const noexcept override{
            return DNodes.size();
        }

        // Returns the node id of the node at index, returns InvalidNodeID if index
        // is greater than or equal to NodeCount()
        TNodeID GetNodeID(std::size_t index) const noexcept override{
            if(index >= NodeCount()){
                return InvalidNodeID;
            }
            else{ 
                return DNodes[index];
            }
        }

        // Returns the number of attributes attached to the SWay
        std::size_t AttributeCount() const noexcept override{
            return DAttributesMap.size();
        }

        // Returns the key of the attribute at index, returns empty string if index
        // is greater than or equal to AttributeCount()
        std::string GetAttributeKey(std::size_t index) const noexcept override{
            if(index >= AttributeCount()){
                return "";
            }
            else{
                return DAttributeKeys[index];
            }
        }
        
        // Returns if the attribute is attached to the SWay
        bool HasAttribute(const std::string &key) const noexcept override{
            return DAttributesMap.find(key) != DAttributesMap.end();
        }
        
        // Returns the value of the attribute specified by key, returns empty string 
        // if key hasn't been attached to SWay
        std::string GetAttribute(const std::string &key) const noexcept override{
            auto it = DAttributesMap.find(key);//this will return an iterator pointing to the key value pair if key exists
            if(it != DAttributesMap.end()){
                return it->second;
            }
            else{
                return "";
            }
        }
    };
    ////////////////////////// END OF SWAY ////////////////////////////////////



    /////////////////////////////////////////SImplementation Member Variables/////////////////////////////////////

    std::vector<std::shared_ptr<SNode>> DNodesByIndex;//this vector stores the Nodes in order and we can use the index of a node to access the ptr to this node 
    std::unordered_map<TNodeID,std::shared_ptr<SNode>> DNodesByID;//this unordered map maps from nodeid to the node object
    //why do we need sharedptr here: because we want to store the same node in different containers like vector and unordered map here.
    
    std::vector<std::shared_ptr<SWay>> DWaysByIndex;//this vector stores the Ways in order and we can use the index of a way to access the ptr to this way 
    std::unordered_map<TWayID,std::shared_ptr<SWay>> DWaysByID;//this unordered map maps from wayid to the way object
    //why do we need sharedptr here: because we want to store the same way in different containers like vector and unordered map here.


    ////////////////////////// Helper Functions ///////////////////////////////
    // This is a function template (helper function) for both ParseWays and ParseNodes (It's nice that both of them have the same name of DAttributeKeys and DAttributesMap)
    // This function will take in a pointer to the template and an XMLEntity Object
    // It will extract key and value of the attribute and store it properly.
    // An improvement of it is I turn it into a bool function that will check for duplicates. 
    template <typename T>
    bool AddAttributes(std::shared_ptr<T> &Obj, SXMLEntity &entity){

        // XMLentity object have a member function AttributeValue(const std::string &name) that will return tghe value to the key that we passed in.
        // e.g. <tag k="highway" v="stop"/> the key to the attribute of this way is "highway" (k is the key of xmlentity object's attribute,"highway" is the value of xmlentity object's attribute ), the value of the attribute of this way is "stop" ()
        // here,we passed in DAttributeKeyTag('k') and we get "highway", which is the key of the attribute of this way
        auto AttributeKey = entity.AttributeValue(DAttributeKeyTag);
        auto AttributeValue = entity.AttributeValue(DAttributeValueTag);
        if(Obj->DAttributesMap.find(AttributeKey) != Obj->DAttributesMap.end()){
            return false;
        }
        Obj->DAttributeKeys.push_back(AttributeKey);
        Obj->DAttributesMap[AttributeKey] = AttributeValue;//operator[] will create a new pair if key does not exist
        return true;
    }

    //  This function will:
    //  1.Read until the end of node entity and set nextentity to the first way entity（CXMLReader is a XML DParser under the hood, and it use SAX stream parsing, so we must readin way obj to check, we cannot just peek without comsuing it）
    //  The state transformation logic look like this:
    //  We keep reading nodes:
    //  state 1: start entity of a node
    //          extract its id/lat/lon
    //          create a SNode object and initailize it with id/lat/lon
    //          keep reading until:
    //              substate 1: end entity of node -> break 
    //              substate 2: start of tag(attribute) -> store the key in DAttributes, store key-value pair into DAtrributesMap
    //              substate 3: end of tag -> nothing need to be done, we can just ignore it
    //  state 2: start of way: set nextentity to the current entity for future use in ParseWays
    //  state 3: end of node -> nothing need to be done, we can just ignore it
    // note: from piazza discussion, we are expected to stop parsing and clean the container whenever we have an error.

    bool ParseNodes(std::shared_ptr<CXMLReader> src, SXMLEntity &nextentity){
        SXMLEntity TempEntity;

        while(src->ReadEntity(TempEntity)){
            //if we read in a start entity with valid tag name
            if(TempEntity.DType == SXMLEntity::EType::StartElement && TempEntity.DNameData == DNodeTag){
                try{
                    auto NodeID = std::stoull(TempEntity.AttributeValue(DNodeIDAttr));//extract and store the nodeid attr
                    auto NodeLat = std::stod(TempEntity.AttributeValue(DNodeLatAttr));//extract and store the lat attr
                    auto NodeLon = std::stod(TempEntity.AttributeValue(DNodeLonAttr));//extract and store the lon attr

                    // if this node already exists,clean the container and rertun false
                    if(DNodesByID.find(NodeID) != DNodesByID.end()){
                        DNodesByIndex.clear();
                        DNodesByID.clear();
                        return false;
                    }

                    // create new node after checking duplicate node.
                    auto NewNode = std::make_shared<SNode>();//create a new SNode object with all the attrs we just get
                    NewNode->DID = NodeID;
                    NewNode->DLocation = SLocation(NodeLat,NodeLon);
                
                    // read in all attributes until end tag
                    while(src->ReadEntity(TempEntity)){
                        // if we meet end entity of node tag, it means we have finished this node, break
                        if(TempEntity.DType == SXMLEntity::EType::EndElement && TempEntity.DNameData == DNodeTag){
                            break;
                        }
                        // if we meet start of attribute tag, we read this in. Store the key in DAttributes, store key-value pair in DAttributeMap.
                        else if(TempEntity.DType == SXMLEntity::EType::StartElement && TempEntity.DNameData == DAttributeTag)
                        {
                            // fuction AddAttributes will check for duplicate internally and return true if no duplicate
                            if(!AddAttributes(NewNode, TempEntity)){
                                DNodesByIndex.clear();
                                DNodesByID.clear();
                                return false;
                            }
                            // Below are my first version, in version 2.0 I enclosed all of them in a helper function
                            // // XMLentity object have a member function AttributeValue(const std::string &name) that will return tghe value to the key that we passed in.
                            // // e.g. <tag k="highway" v="stop"/> the key to the attribute of this way is "highway" (k is the key of xmlentity object's attribute,"highway" is the value of xmlentity object's attribute ), the value of the attribute of this way is "stop" ()
                            // // here,we passed in DAttributeKeyTag('k') and we get "highway", which is the key of the attribute of this way
                            // auto AttributeKey = TempEntity.AttributeValue(DAttributeKeyTag);
                            // auto AttributeValue = TempEntity.AttributeValue(DAttributeValueTag);
                            // NewNode->DAttributeKeys.push_back(AttributeKey);
                            // NewNode->DAttributesMap[AttributeKey] = AttributeValue;//operator[] will create a new pair if key does not exist
                        }
                    }
                    //push node after successfully parsing
                    DNodesByIndex.push_back(NewNode);
                    DNodesByID[NodeID] = NewNode;
                }
                // Stop parsing if conversion fails
                catch(const std::exception &e) {
                    DNodesByIndex.clear();
                    DNodesByID.clear();
                    return false; 
                }
            }
            //if we read in the first way entity, we set it to nextentity and stop parsing node.
            else if(TempEntity.DType == SXMLEntity::EType::StartElement && TempEntity.DNameData == DWayTag){
                nextentity = TempEntity;
                // Important Note: this is Way entity but we cannot parse it here, so we need to store it and pass it into ParseWays as the first entity to process 
                break;
            }
        }
        return true;
    }
    // This is a helper function for ParseWays
    // This function will create a new SWay obj with the start entity of sway.
    // It will be used twice(first entity & meet start of way in while) so I choose to enclose it as a helper function.
    std::shared_ptr<SWay> CreateSWay(SXMLEntity &startentity){
        auto WayID = std::stoull(startentity.AttributeValue(DWayIDAttr));//extract and store the wayid attr
        auto NewWay = std::make_shared<SWay>();
        NewWay->DWayID = WayID;
        return NewWay;
    }

    // The problem is in the last part of ParseNodes, we've already read through the first way entity but we haven't processed it
    // So, we must pass that entity into our Parseways function and parse it first.
    //  This function will:
    //  Read through all way entities and store the id/nodes/attrbutes that belong to this way object
    //  The state transformation logic look like this:
    //  At first, we need to parse the first entity passed in extract its way id, create a way obj with it.
    //  We keep reading ways until we meet:
    //      state 1: start of node -> push it into Dnodes
    //      state 2: start of attribute tag -> we read this in, store the key in DAttributes, store key-value pair in DAttributeMap.
    //      state 3: end of way, we push current way to DWaysByIndex and store the mapping of id and way to DWaysByID
    //      state 4: start of way, create a new way with its entity
    // note: from piazza discussion, we are expected to stop parsing and clean the container whenever we have an error.

    bool ParseWays(std::shared_ptr<CXMLReader> src, SXMLEntity &firstentity){
        SXMLEntity TempEntity;

        // Parse the first entity extract the way id and create a Sway obj with it and it will be reused when we read in other ways.
        std::shared_ptr<SWay> NewWay;
        try {
            NewWay = CreateSWay(firstentity); // safely parse first way entity incase of invalid wayID
        } 
        catch (const std::exception &e) {
            DWaysByIndex.clear();
            DWaysByID.clear();
            return false;
        }
        while(src->ReadEntity(TempEntity)){
            //safely read in and parse entity
            try{
                // read in a node in this way, push it into DNodes
                if(TempEntity.DType == SXMLEntity::EType::StartElement && TempEntity.DNameData == DWayNodeTag){
                    auto NodeID = std::stoull(TempEntity.AttributeValue(DWayNodeIDAttr));//extract and store the nodeid attr
                    //check if this node exist or not
                    if(DNodesByID.find(NodeID) == DNodesByID.end()){
                        DWaysByIndex.clear();
                        DWaysByID.clear();
                        return false;
                    }
                    NewWay->DNodes.push_back(NodeID);
                }
                //if we meet start of attribute tag, we read this in with my helper function. Store the key in DAttributes, store key-value pair in DAttributeMap.
                else if(TempEntity.DType == SXMLEntity::EType::StartElement && TempEntity.DNameData == DAttributeTag){
                    // AddAttributes will add this attribute if it's not a duplicate or return flase
                    if(!AddAttributes(NewWay, TempEntity)){ 
                        DWaysByIndex.clear();
                        DWaysByID.clear();
                        return false;
                    }
                }
                //if we read in end of way, we push way to DWaysByIndex and store the mapping of id and way to DWaysByID
                else if(TempEntity.DType == SXMLEntity::EType::EndElement && TempEntity.DNameData == DWayTag){
                    // check for duplicate wayid
                    if(DWaysByID.find(NewWay->DWayID) != DWaysByID.end()){
                        DWaysByIndex.clear();
                        DWaysByID.clear();
                        return false;
                    }

                    DWaysByIndex.push_back(NewWay);
                    DWaysByID[NewWay->DWayID] = NewWay;
                }
                //if we read in start of way, create a new way with its entity
                else if(TempEntity.DType == SXMLEntity::EType::StartElement && TempEntity.DNameData == DWayTag){
                    NewWay = CreateSWay(TempEntity);
                }
            }
            catch (const std::exception &e){
                // conversion failed
                DWaysByIndex.clear();
                DWaysByID.clear();
                return false;
            }
        }
        return true;
    }

    // This function will read in the first entity to check if it is a valid osm files
    // And then, it will call ParseNodes to read in all the nodes in this map
    // After paring all the nodes, it will call ParseWays to parse all the ways in this map
    // return true if both parsenode and parseway are successful
    // src is a pointer to CXMLReader it will keep track of "where we at" internally
    // This is neabled by DParser(XMl_Parser Obj) inside the reader object 
    bool ParseOpenStreetMap(std::shared_ptr<CXMLReader> src){
        SXMLEntity TempEntity;// this is a temporary variable that will be reused 
                              // At first, it is used to store the first entity of the whole xml file
                              // and then be passed into ParseNodes to store the first way entity 
                              // and then passed into ParseWays as the first entity 
        //src->ReadEntity(TempEntity will read in the header like <osm version="0.6" generator="osmconvert 0.8.5">  
        if(src->ReadEntity(TempEntity)){
            //check if it's a valid header
            if(TempEntity.DType == SXMLEntity::EType::StartElement && TempEntity.DNameData == DOSMTag){
                // if it is a vlid header, we parse nodes
                if(ParseNodes(src,TempEntity)){
                    //if successfully parse nodes, parse ways.
                    if(ParseWays(src,TempEntity)){
                        return true;
                    }
                }
            }
        }
        return false;
    }

    SImplementation(std::shared_ptr<CXMLReader> src){
        ParseOpenStreetMap(src);
    }


    ///////////////////////////////////////////SImplementation Member Functions/////////////////////////////////////

    // Returns the number of nodes in the map
    std::size_t NodeCount() const noexcept{
        return DNodesByIndex.size();
    }

    // Returns the number of ways in the map
    std::size_t WayCount() const noexcept{
        return DWaysByIndex.size();
    }
    
    // Returns the SNode associated with index, returns nullptr if index is 
    // larger than or equal to NodeCount()
    std::shared_ptr<CStreetMap::SNode> NodeByIndex(std::size_t index) const noexcept{
        if(index >= NodeCount()){
            return nullptr;
        }
        else{
            return DNodesByIndex[index];
        }
    }

    // Returns the SNode with the id of id, returns nullptr if doesn't exist
    std::shared_ptr<CStreetMap::SNode> NodeByID(TNodeID id) const noexcept{
        auto it = DNodesByID.find(id);//this will return an iterator pointing to the key value pair if key exists
            if(it != DNodesByID.end()){
                return it->second;
            }
            else{
                return nullptr;
            }
    }
    
    // Returns the SWay associated with index, returns nullptr if index is 
    // larger than or equal to WayCount()
    std::shared_ptr<CStreetMap::SWay> WayByIndex(std::size_t index) const noexcept{
        if(index >= WayCount()){
            return nullptr;
        }
        else{
            return DWaysByIndex[index];
        }
    }
    // Returns the SWay with the id of id, returns nullptr if doesn't exist
    std::shared_ptr<CStreetMap::SWay> WayByID(TWayID id) const noexcept{
        auto it = DWaysByID.find(id);//this will return an iterator pointing to the key value pair if key exists
            if(it != DWaysByID.end()){
                return it->second;
            }
            else{
                return nullptr;
            }
    }
};
//////////////////////////////////////// End of SImplementation ////////////////////////////////////////




//////////////////////////////////////// COpenStreetMap member functions////////////////////////////
// All of these CSVBusSystem function wejust defer to DImplementation
// Constructor for the Open Street Map
COpenStreetMap::COpenStreetMap(std::shared_ptr<CXMLReader> src){
    DImplementation = std::make_unique<SImplementation>(src);
}

// Destructor for the Open Street Map
COpenStreetMap::~COpenStreetMap(){

}

// Returns the number of nodes in the map
std::size_t COpenStreetMap::NodeCount() const noexcept {
    return DImplementation->NodeCount();
}
// Returns the number of ways in the map
std::size_t COpenStreetMap::WayCount() const noexcept{
    return DImplementation->WayCount();
}

// Returns the SNode associated with index, returns nullptr if index is 
// larger than or equal to NodeCount()
std::shared_ptr<CStreetMap::SNode> COpenStreetMap::NodeByIndex(std::size_t index) const noexcept{
    return DImplementation->NodeByIndex(index);
}

// Returns the SNode with the id of id, returns nullptr if doesn't exist
std::shared_ptr<CStreetMap::SNode> COpenStreetMap::NodeByID(TNodeID id) const noexcept{
    return DImplementation->NodeByID(id);
}

// Returns the SWay associated with index, returns nullptr if index is 
// larger than or equal to WayCount()
std::shared_ptr<CStreetMap::SWay> COpenStreetMap::WayByIndex(std::size_t index) const noexcept{
    return DImplementation->WayByIndex(index);
}

// Returns the SWay with the id of id, returns nullptr if doesn't exist
std::shared_ptr<CStreetMap::SWay> COpenStreetMap::WayByID(TWayID id) const noexcept{
    return DImplementation->WayByID(id);
}
