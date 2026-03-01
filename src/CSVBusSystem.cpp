#include "CSVBusSystem.h"
#include <unordered_map>
#include <unordered_set>

struct CCSVBusSystem::SImplementation{

    const std::string STOP_ID_HEADER    = "stop_id";
    const std::string NODE_ID_HEADER    = "node_id";
    const std::string ROUTE_HEADER    = "route";

    /////////////////////////////////// SStop ///////////////////////////////////
    //SStop inherites the abstract interface CBusSystem::SStop
    struct SStop: public CBusSystem::SStop{
        TStopID DID;
        CStreetMap::TNodeID DNodeID;

        SStop(TStopID StopID, CStreetMap::TNodeID NodeID) : DID(StopID),DNodeID(NodeID){}
        
        ~SStop(){

        }

        TStopID ID() const noexcept{
            return DID;
        }

        CStreetMap::TNodeID NodeID() const noexcept{
            return DNodeID;
        }
    };


    /////////////////////////////////// SRoute///////////////////////////////////
    //SRoute inherites the abstract interface CBusSystem::SRoute
    struct SRoute: public CBusSystem::SRoute{

        std::string DName;//name of route is std::string
        std::vector<TStopID> DStops; //Dstops stores a vector of stopids in this route

        SRoute(std::string Name,std::vector<TStopID> Stops): DName(Name), DStops(Stops){}
    
        ~SRoute(){

        }

        std::string Name() const noexcept{
            return DName;
        }

        std::size_t StopCount() const noexcept{
            return DStops.size();
        }

        //this is a safe id accesser interface
        TStopID GetStopID(std::size_t index) const noexcept{
            //be mindful of index out of range
            if(index < DStops.size()){
                return DStops[index];
            }
            else{
                return InvalidStopID;
            }
        }
    };

    // These 2 const strings are used to create identifier of the first line of csv file
    // if we want to change, we can just change it here.
    // Since the ordering of two columns are not guaranteed, we must find the ordering of them dynamically


    ///////////////////////////////////////////SImplementation Member Variables/////////////////////////////////////
    std::vector< std::shared_ptr< SStop > > DStopsByIndex;//this vector stores the Stops in order and we can use the index of a stop to access the ptr to this stop 
    std::unordered_map< TStopID, std::shared_ptr< SStop > > DStopsByID;//this unordered map maps from stopid to the stop object
    //why do we need sharedptr here: because we want to store the same stop in different containers like vector and unordered map here.

    ///////////////////////////////////////////SImplementation Member Functions/////////////////////////////////////
 
    // This function will:
    // 1.Read the first row to find which column stores the stop_id and which column stores the node_id 
    // 2.Read the rest of the files till the end
    // 3.After Reading a row, create a new stop object, extract and store the stopid and nodeid into this stop object 
    // 4.Pushes this new stop object into our vector DStopsByIndex
    // 5.Store this new stop into our unorderedmap DStopsByID with key:ID value:SStop object(ptr)
    // note: from piazza discussion, we are expected to stop parsing and clean the container whenever we have an error.
    bool ReadStops(std::shared_ptr< CDSVReader > stopsrc){
        std::vector<std::string> TempRow;
        //This if will only read the first row for sanity check
        if(stopsrc->ReadRow(TempRow)){
            size_t StopColumn = -1;
            size_t NodeColumn = -1;
            for(size_t Index = 0; Index < TempRow.size(); Index++){
                if(TempRow[Index] == STOP_ID_HEADER){
                    StopColumn = Index;
                }
                else if(TempRow[Index] == NODE_ID_HEADER){
                    NodeColumn = Index;
                }
            }
            // Invalid header
            if(StopColumn == -1 || NodeColumn == -1){
                //clear the container when we have an error
                DStopsByIndex.clear();
                DStopsByID.clear();
                return false;
            }
            //From DSVReader, TempRow is a vector of strings and ReadRow will read a row into this vector
            //With info in this vector, we are able to create the node that we just read in
            //We also want to change the type from string to unsigned long long to match their type
            //Then for DStopsByIndex, we want to  access the node uusing the index of that node
            //For  DStopsByID, we want to access this node via its id, so we need to have a mapping from id to the node object ptr.
            while(stopsrc->ReadRow(TempRow)){
                try{
                    TStopID StopID = std::stoull(TempRow[StopColumn]);
                    //check if we have duplicate stop id, if that exist, just return false.
                    if(DStopsByID.find(StopID) != DStopsByID.end()){
                        //clear the container when we have an error
                        DStopsByIndex.clear();
                        DStopsByID.clear();
                        return false;
                    }
                    CStreetMap::TNodeID NodeID = std::stoull(TempRow[NodeColumn]);
                    auto NewStop = std::make_shared< SStop >(StopID,NodeID); //equivalent to DID = StopID, DNodeID = NodeID
                    DStopsByIndex.push_back(NewStop);
                    DStopsByID[StopID] = NewStop;// key=StopID, value = Newstop
                }
                // Stop parsing if conversion fails
                catch(const std::exception &e) {
                    //clear the container when we have an error
                    DStopsByIndex.clear();
                    DStopsByID.clear();
                    return false; 
                }
            }

            return true;
        }
        // we can't even read the first row, return false.
        return false;
    }

    std::vector< std::shared_ptr< SRoute > > DRouteByIndex;//this vector stores the Routes in order and we can use the index of a route to access the ptr to this route
    std::unordered_map< std::string, std::shared_ptr< SRoute > > DRouteByName;//this unordered map maps from routename to the route object ptr
    //why do we need sharedptr here: because we want to store the same route object in different containers like vector and unordered map here.

    //This Function will:
    // 1.Read the first row to find which column stores the stop_id and which column stores the route_id 
    // 2.Read the rest of the files till the end
    // 3.Create a new route object(this is the route obj for what we just read in, not the new one) if we meet a new route_id
    // 4.If the next route_id is equal to the current route_id, it means we are still processing the same route, just push stop_id into CurrStops. 
    // 5.Flush out the last route because we flush out the previous Route object only if we encounter a new route_id, and that will not happen for the last route.
    // note: from piazza discussion, we are expected to stop parsing and clean the container whenever we have an error.
    bool ReadRoutes(std::shared_ptr< CDSVReader > routesrc) {
        std::vector<std::string> TempRow;

        if(routesrc->ReadRow(TempRow)){
            size_t StopColumn = -1;
            size_t RouteColumn = -1;
            for(size_t Index = 0; Index < TempRow.size(); Index++){
                if(TempRow[Index] == STOP_ID_HEADER){
                    StopColumn = Index;
                }
                else if(TempRow[Index] == ROUTE_HEADER){
                    RouteColumn = Index;
                }
            }
            // Invalid header
            if(StopColumn == -1 || RouteColumn == -1){
                //clear the container when we have an error
                DRouteByIndex.clear();
                DRouteByName.clear();
                return false;
            }
            //A route contains multiple stops, so we need to store the current route name and create a new route when we encounter a new route name
            // We also want to keep track of all the stops in this route via a vector.
            std::string CurrName = "";
            std::vector<TStopID> CurrStops;
            //In this while loop, we read until the end of the file
            // We also need to check if it is the first route by comparing name with "", if it is the first route update the name and push the stopid into CurrStops.
            // (this doesn't need to be explicitly implemented because the conditional statement below will handle this automatically once we update CurrName)
            //case 1: if we encounter a new routename, finish the previous route by creating a Sroute object and pushing it into the vector and unordered map and we empty the vector CurrStops.
            //case 2: if the route name we read in is the same as Current name, we are still in th esame route, just push stopid into the vector that contains the stopids of this route, which is CurrStops.
            while(routesrc->ReadRow(TempRow)){
                //check to make sure index will not go out of bound(deal with lines like "A", only route id no stop id)
                if(TempRow.size() <= StopColumn || TempRow.size() <= RouteColumn){
                    //clear the container when we have an error
                    DRouteByIndex.clear();
                    DRouteByName.clear();
                    return false;
                }
                if (CurrName == ""){
                    CurrName = TempRow[RouteColumn];
                }
                if (CurrName != TempRow[RouteColumn]){
                    auto NewRoute = std::make_shared<SRoute>(CurrName,CurrStops);
                    DRouteByIndex.push_back(NewRoute);
                    DRouteByName[CurrName] = NewRoute;
                    CurrName = TempRow[RouteColumn];
                    CurrStops.clear();                    
                }
                try{
                    TStopID StopID = std::stoull(TempRow[StopColumn]);
                    //is it possible that we will get to the same stop twice in a route? I believe it could, so we might not need to check duplicacity of stop inside a route.
                    //if it is a invalid stop (not a stop in the route)
                    if(DStopsByID.find(StopID) == DStopsByID.end()){
                        //clear the container when we have an error
                        DRouteByIndex.clear();
                        DRouteByName.clear();
                        return false;
                    }
                    CurrStops.push_back(StopID);
                }
                // Stop parsing if conversion fails
                catch(const std::exception &e) {
                    //clear the container when we have an error
                    DRouteByIndex.clear();
                    DRouteByName.clear();
                    return false; 
                }
            }
            //!!! Important: We need to create a new route here because when we reach to the end of the file, the buffer(CurrStops) is not empty and we want to flush it out.
            // we need an if statement incase of empty file.(we cannot directly flush out eveything at the end because if we have a empty file, it will also try to crete a new route object)
            if(!CurrStops.empty()){
                auto NewRoute = std::make_shared<SRoute>(CurrName,CurrStops);
                DRouteByIndex.push_back(NewRoute);
                DRouteByName[CurrName] = NewRoute;
            }
            return true;
        }
        // we can't even read the first row, return false.
        return false;
    }

    SImplementation(std::shared_ptr< CDSVReader > stopsrc, std::shared_ptr< CDSVReader > routesrc){
        //Only if we have successfully read the stops can we read the route.
        //Or we might access invalid stopid 
        if(ReadStops(stopsrc)){
            ReadRoutes(routesrc);
        }
    }

    // We don't necessarily need to store the number of nodes, we can just access it via the size of the vector of SStops
    std::size_t StopCount() const noexcept{
        return DStopsByIndex.size();
    }

    std::size_t RouteCount() const noexcept{
        return DRouteByIndex.size();
    }
    
    // Returns the SStop specified by the index, nullptr is returned if index is
    // greater than equal to StopCount()
    std::shared_ptr<SStop> StopByIndex(std::size_t index) const noexcept{
        if(index >= StopCount()){
            return nullptr;
        }
        else{
            return DStopsByIndex[index];
        }
    }

    // Returns the SStop specified by the stop id, nullptr is returned if id is
    // not in the stops
    // Note: we don't use operator[] here because it will try to add a new pair if id not exist, 
    // but it's a const function and doesn't allow us to modify the object that calls it.
    std::shared_ptr<SStop> StopByID(TStopID id) const noexcept{
        if(DStopsByID.find(id) == DStopsByID.end()){
            return nullptr;
        }
        else{
            return DStopsByID.at(id);
        }
    }

    // Returns the SRoute specified by the index, nullptr is returned if index is
    // greater than equal to RouteCount()
    std::shared_ptr<SRoute> RouteByIndex(std::size_t index) const noexcept{
        if(index >= RouteCount()){
            return nullptr;
        }
        else{
            return DRouteByIndex[index];
        }
        
    }

    // Returns the SRoute specified by the name, nullptr is returned if name is
    // not in the routes
    //Here, I tried another way to access the value via key in an unordered map just for practice.
    //this method is safer because it checks if the key exixts instead of assuming everything is fine.
    std::shared_ptr<SRoute> RouteByName(const std::string &name) const noexcept{
        auto it = DRouteByName.find(name);//this will return an iterator pointing to the key value pair if key exists
        if (it != DRouteByName.end()){
            return it->second;
        }
        else{
            return nullptr;
        }
    }


};
//end of SImplementation definition





//////////////////////////////////////// CCSVBusSystem member functions////////////////////////////

//All of these CSVBusSystem function we just defer to DImplementation
// Constructor for the CSV Bus System
CCSVBusSystem::CCSVBusSystem(std::shared_ptr< CDSVReader > stopsrc, std::shared_ptr< CDSVReader > routesrc){
    DImplementation = std::make_unique<SImplementation>(stopsrc,routesrc);
}

// Destructor for the CSV Bus System
CCSVBusSystem::~CCSVBusSystem(){

}

// Returns the number of stops in the system
std::size_t CCSVBusSystem::StopCount() const noexcept{
    return DImplementation->StopCount();
}

// Returns the number of routes in the system
std::size_t CCSVBusSystem::RouteCount() const noexcept{
    return DImplementation->RouteCount();
}

// Returns the SStop specified by the index, nullptr is returned if index is
// greater than equal to StopCount()
std::shared_ptr<CBusSystem::SStop> CCSVBusSystem::StopByIndex(std::size_t index) const noexcept{
    return DImplementation->StopByIndex(index);
}

// Returns the SStop specified by the stop id, nullptr is returned if id is
// not in the stops
std::shared_ptr<CBusSystem::SStop> CCSVBusSystem::StopByID(TStopID id) const noexcept{
    return DImplementation->StopByID(id);
}

// Returns the SRoute specified by the index, nullptr is returned if index is
    // greater than equal to RouteCount()
std::shared_ptr<CBusSystem::SRoute> CCSVBusSystem::RouteByIndex(std::size_t index) const noexcept{
    return DImplementation->RouteByIndex(index);
}

// Returns the SRoute specified by the name, nullptr is returned if name is
// not in the routes
std::shared_ptr<CBusSystem::SRoute> CCSVBusSystem::RouteByName(const std::string &name) const noexcept{
    return DImplementation->RouteByName(name);
}



