#include "BusSystemIndexer.h"
#include "BusSystem.h"
#include <vector> 
#include <unordered_map>
#include <algorithm>

struct CBusSystemIndexer::SImplementation{
    std::shared_ptr<CBusSystem> DBussystem;

    std::vector<std::shared_ptr<SStop>> DSortedStopsByIndex;// vector, store the sorted stops objptrs, enable to access by index
    std::unordered_map<TNodeID,std::shared_ptr<SStop>> DStopByNodeID;// unorderedmap, from nodeid to stop objptr, enable to access by nodeid
    std::vector<std::shared_ptr<SRoute>> DSortedRoutesByIndex;// vector, store the sorted routes objptrs, enable to access by index
    //std::unordered_map<CBusSystem::TStopID,TNodeID> DNodeIDbyStopID;// unorderedmap, from stopid to nodeid, enable O(1) access from stopid to nodeid

    // std::unordered_map does not provide a default hash implementation for std::pair, so we need to create our own.
    struct pairhash{
        std::size_t operator()(const std::pair<TNodeID,TNodeID> &x) const {
            return std::hash<TNodeID>()(x.first) ^ std::hash<TNodeID>()(x.second); // std::hash<TNodeID>()(x.first) will turn x.first into a hash value
        }
    };
    std::unordered_map<std::pair<TNodeID,TNodeID>,std::unordered_set<std::shared_ptr<SRoute>>,pairhash> DRoutesBetweenStops;// unorderedmap, from {src, dest} pair to a unordered set of route ptrs 
                                                                                                                //we don't use vector here because a route should only appear once and we will need to check for duplicate if we use a vector, 

    // Precompute all route lookups between stops during construction
    // Instead of checking every route at runtime for a given pair of stops (src, dest),
    // we can build a mapping in advance that associates each stop pair with all routes
    // that contain both stops. This transforms RoutesByNodeIDs queries into simple table
    // lookups, providing O(1) access rather than iterating through every route each time.
    SImplementation(std::shared_ptr<CBusSystem> bussystem) : DBussystem (bussystem){

        /////////////////////////Processing Stops in Bussystem/////////////////////////////////
        // This for loop will iterate through all the stops in the bussystem via original index
        // extract the ptr to stop and store it into DSortedStopsByIndex and to be sorted in the future
        // create the mapping from its node id to ptr to this stop and store it into DStopByNodeID
        for(size_t Index = 0; Index < DBussystem->StopCount(); Index++){
            auto Stop = DBussystem->StopByIndex(Index);
            DSortedStopsByIndex.push_back(Stop);
            DStopByNodeID[Stop->NodeID()] = Stop;
            //DNodeIDbyStopID[Stop->ID()] = Stop->NodeID();
        }
        // prototype std::sort(begin_iterator, end_iterator, comparison_function);
        //[](std::shared_ptr<SStop> l,std::shared_ptr<SStop> r){ return l->ID() < r->ID(); } is a lambda expression
        // Prototype : [Capture list](Parameter list) -> Return type { Function body }, 
        // Capture list [ ]: can capture external variables; 
        // Parameter list ( ): same as a normal function; 
        // Return type -> type: optional, can be omitted if it can be deduced; 
        // Function body { }: implement the comparison logic. Return true if the first element should come before the second element        //This function will sort the stop in the vector by the stop_id in ascending order
        std::sort(DSortedStopsByIndex.begin(),DSortedStopsByIndex.end(),[](std::shared_ptr<SStop> l,std::shared_ptr<SStop> r){
            return l->ID() < r->ID();
        });

        /////////////////////////Processing Routes in Bussystem/////////////////////////////////
        for(size_t Index1 = 0; Index1 < DBussystem->RouteCount(); Index1++){
            auto Route = DBussystem->RouteByIndex(Index1);
            DSortedRoutesByIndex.push_back(Route);
            // These 2 inner nested for loop are used to find all possible {src, dest} pairs and initialize DRoutesBetweenStops
            for(size_t SrcIndex = 0; SrcIndex < Route->StopCount(); SrcIndex++){
                for(size_t DestIndex = SrcIndex + 1; DestIndex < Route->StopCount(); DestIndex++){
                    // Since there's no direct mapping from stop id to node id, we need to get the stop object ptr and then access it id
                    // Both GetStopID and StopByID takes constant time.
                    auto SrcStopID = Route->GetStopID(SrcIndex);// O(1)
                    auto DestStopID = Route->GetStopID(DestIndex);
                    auto SrcStop = DBussystem->StopByID(SrcStopID);// O(1)
                    auto DestStop = DBussystem->StopByID(DestStopID);
                    TNodeID SrcID = SrcStop->NodeID();
                    TNodeID DestID = DestStop->NodeID();
                    std::pair<TNodeID, TNodeID> Key = {SrcID, DestID};
                    DRoutesBetweenStops[Key].insert(Route);
                }
            }
        }
        std::sort(DSortedRoutesByIndex.begin(),DSortedRoutesByIndex.end(),[](std::shared_ptr<SRoute> l,std::shared_ptr<SRoute> r){
            return l->Name() < r->Name();
        });

    }
    ~SImplementation(){}

    // Returns the number of stops in the CBusSystem being indexed
    std::size_t StopCount() const noexcept{
        return DBussystem->StopCount();
    }

    // Returns the number of routes in the CBusSystem being indexed
    std::size_t RouteCount() const noexcept{
        return DBussystem->RouteCount();
    }

    // Returns the SStop specified by the index where the stops are sorted by 
    // their ID, nullptr is returned if index is greater than equal to 
    // StopCount()
    std::shared_ptr<SStop> SortedStopByIndex(std::size_t index) const noexcept{
        if (index >= StopCount()){
            return nullptr;
        }
        else{
            return DSortedStopsByIndex[index];
        }
    }

    // Returns the SRoute specified by the index where the routes are sorted by 
    // their Name, nullptr is returned if index is greater than equal to 
    // RouteCount()
    std::shared_ptr<SRoute> SortedRouteByIndex(std::size_t index) const noexcept{
        if(index >= RouteCount()){
            return nullptr;
        }
        else{
            return DSortedRoutesByIndex[index];
        }
    }

    // Returns the SStop associated with the specified node ID, nullptr is 
    // returned if no SStop associated with the node ID exists
    std::shared_ptr<SStop> StopByNodeID(TNodeID id) const noexcept{
        auto it = DStopByNodeID.find(id);
        if(it == DStopByNodeID.end()){
            return nullptr;
        }
        else{
            return it->second;
        }
    }

    // Returns true if at least one route exists between the stops at the src and 
    // dest node IDs. 
    // Update routes with all possible route. All routes that have a route segment between the stops at 
    // the src and dest nodes will be placed in routes unordered set.
    bool RoutesByNodeIDs(TNodeID src, TNodeID dest, 
    std::unordered_set<std::shared_ptr<SRoute> > &routes) const noexcept{
        routes.clear();// we might use the same contianer multiple times and we don't want the elements in it to accumulate
        if(src == dest){
            return false;
        }
        else{
            std::pair<TNodeID,TNodeID> Key = std::make_pair(src,dest);
            auto it = DRoutesBetweenStops.find(Key);
            if(it != DRoutesBetweenStops.end()){
                routes.insert(it->second.begin(),it->second.end());
                return true;
            }
            else{
                return false;
            }
        }
        
    }

    // Returns true if at least one route exists between the stops at the src and 
    // dest node IDs. 
    // Implementation: Returns true only if there exists a route in which `src` appears
    // strictly before `dest`. The order of stops matters.
    // If `dest` appears before `src`, this function returns false.
    bool RouteBetweenNodeIDs(TNodeID src, TNodeID dest) const noexcept{
        if(src == dest){
            return false;
        }
        else{
            std::pair<TNodeID,TNodeID> Key = std::make_pair(src,dest);
            return DRoutesBetweenStops.find(Key) != DRoutesBetweenStops.end();
        }
    }
};

/////////////////////////CBusSystemIndexer Member function definition////////////////////////////

// Constructor for the Bus System Indexer
    CBusSystemIndexer::CBusSystemIndexer(std::shared_ptr<CBusSystem> bussystem){
        DImplementation = std::make_unique<SImplementation>(bussystem);
    }


    // Destructor for the Bus System Indexer
    CBusSystemIndexer::~CBusSystemIndexer(){

    }


    // Returns the number of stops in the CBusSystem being indexed
    std::size_t CBusSystemIndexer::StopCount() const noexcept{
        return DImplementation->StopCount();
    }


    // Returns the number of routes in the CBusSystem being indexed
    std::size_t CBusSystemIndexer::RouteCount() const noexcept{
        return DImplementation->RouteCount();
    }


    // Returns the SStop specified by the index where the stops are sorted by 
    // their ID, nullptr is returned if index is greater than equal to 
    // StopCount()
    std::shared_ptr<CBusSystem::SStop> CBusSystemIndexer::SortedStopByIndex(std::size_t index) const noexcept{
        return DImplementation->SortedStopByIndex(index);
    }


    // Returns the SRoute specified by the index where the routes are sorted by 
    // their Name, nullptr is returned if index is greater than equal to 
    // RouteCount()
    std::shared_ptr<CBusSystem::SRoute> CBusSystemIndexer::SortedRouteByIndex(std::size_t index) const noexcept{
        return DImplementation->SortedRouteByIndex(index);
    }


    // Returns the SStop associated with the specified node ID, nullptr is 
    // returned if no SStop associated with the node ID exists
    std::shared_ptr<CBusSystem::SStop> CBusSystemIndexer::StopByNodeID(TNodeID id) const noexcept{
        return DImplementation->StopByNodeID(id);
    }


    // Returns true if at least one route exists between the stops at the src and 
    // dest node IDs. All routes that have a route segment between the stops at 
    // the src and dest nodes will be placed in routes unordered set.
    bool CBusSystemIndexer::RoutesByNodeIDs(TNodeID src, TNodeID dest, std::unordered_set<std::shared_ptr<SRoute> > &routes) const noexcept{
        return DImplementation->RoutesByNodeIDs(src,dest,routes);
    }


    // Returns true if at least one route exists between the stops at the src and 
    // dest node IDs. 
    bool CBusSystemIndexer::RouteBetweenNodeIDs(TNodeID src, TNodeID dest) const noexcept{
        return DImplementation->RouteBetweenNodeIDs(src,dest);
    }
