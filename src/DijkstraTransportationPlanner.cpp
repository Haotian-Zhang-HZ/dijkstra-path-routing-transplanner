#include "DijkstraTransportationPlanner.h"
#include "BusSystemIndexer.h"
#include "DijkstraPathRouter.h"
#include "PathRouter.h"
#include "GeographicUtils.h"
#include <unordered_map>
#include <limits>

struct CDijkstraTransportationPlanner::SImplementation{
    const std::string BIKETAG = "bicycle";
    
    static const double NoPathExists;
    using TNodeID = CStreetMap::TNodeID;
    using TTripStep = CTransportationPlanner::TTripStep;

    double DWalkSpeed;
    double DBikeSpeed;
    double DBusSpeed;

    struct SPlannerEdge{

        CPathRouter::TVertexID DDest; // Since we use adjacency list to represent a graph, we don't need to store the src(adj list just store a verctor of dest mapping to a src)
        double DTime;
        ETransportationMode Mode;

        SPlannerEdge(
            CPathRouter::TVertexID dest, double Time, ETransportationMode mode) : DDest(dest), DTime(Time), Mode(mode){
            // WalkTime = CanWalk ? DWeight / DWalkSpeed : std::numeric_limits<double>::infinity();
            // BikeTime = CanBike ? DWeight / DBikeSpeed : std::numeric_limits<double>::infinity();
            // BusTime  = CanBus  ? DWeight / DBusSpeed  : std::numeric_limits<double>::infinity();
            }

        ~SPlannerEdge(){}

        CPathRouter::TVertexID GetDestID() const{
            return DDest;
        }

        double GetTime() const{
            return DTime;
        }
    };

    std::shared_ptr<SConfiguration> DConfig;
    std::shared_ptr<CStreetMap> DStreetMap;
    std::shared_ptr<CBusSystem> DBusSystem;
    std::shared_ptr<CBusSystemIndexer> DBusIndexer;
    std::shared_ptr<CDijkstraPathRouter> DRouter;
    std::unordered_map<TNodeID, CPathRouter::TVertexID> DNodeToVertex;
    std::unordered_map<CPathRouter::TVertexID,TNodeID> DVertexToNode;
    std::vector<CStreetMap::TNodeID> DSortedNodeIDs;
    std::vector<std::vector<SPlannerEdge>> DAdjacencies; // Adjacency list of graph for planner

    // Initialize the streetmap and bussystem we use from the config and create a bus indexer and router in the constructor
    SImplementation(std::shared_ptr<SConfiguration> config) 
        :   DConfig(config),
            DStreetMap(config->StreetMap()),
            DBusSystem(config->BusSystem()),
            DBusIndexer(std::make_shared<CBusSystemIndexer>(DBusSystem)),
            DRouter(std::make_shared<CDijkstraPathRouter>()){
        
        // At this stage, we already know the number of vertices and by reserve() we save the time of copying everying and allocate new chunck of memory
        size_t NodeCount = DStreetMap->NodeCount();
        DSortedNodeIDs.reserve(NodeCount);
        DNodeToVertex.reserve(NodeCount);
        DVertexToNode.reserve(NodeCount);
        DAdjacencies.resize(NodeCount);

        // Construct dual-query relation between node id and vertex id
        for(size_t NodeIndex = 0; NodeIndex < DStreetMap->NodeCount(); NodeIndex++){
            // Construct the dual-query between vertexid and nodeid
            auto CurrNodePtr = DStreetMap->NodeByIndex(NodeIndex);
            auto NodeID = CurrNodePtr->ID();
            auto VertexID = DRouter->AddVertex(NodeID);// Create a new vertex in the router, the tag is NodeID
            DNodeToVertex[NodeID] = VertexID; // Store the mapping from nodeID to vertexID in the DNodeToVertex
            DVertexToNode[VertexID] = NodeID;
            DSortedNodeIDs.push_back(NodeID); 
        }

        // Sort nodeid by index
        std::sort(DSortedNodeIDs.begin(),DSortedNodeIDs.end()); 

        // Construct graph for planner
        // Traverse each way in the map and traverse each adjacent node pair to create an edge between them 
        for(size_t WayIndex = 0; WayIndex < DStreetMap->WayCount(); WayIndex++){
            auto Way = DStreetMap->WayByIndex(WayIndex);
            //Skip way that is not tagged with highway
            if(!Way->HasAttribute("highway")){
                continue;
            }

            // Check for one-way or not
            bool oneway = false;
            if(Way->HasAttribute("oneway") && Way->GetAttribute("oneway") == "yes"){
                oneway = true;
            }

            // Check for bike
            bool canBike = true;
            if(Way->HasAttribute("bicycle") && Way->GetAttribute("bicycle") == "no"){
                canBike = false;
            }

            for(size_t NodeIndex = 0; NodeIndex + 1 < Way->NodeCount(); NodeIndex++){
                auto SrcNodeID = Way->GetNodeID(NodeIndex);
                auto DestNodeID = Way->GetNodeID(NodeIndex+1);

                auto SrcVertexID = DNodeToVertex[SrcNodeID];
                auto DestVertexID = DNodeToVertex[DestNodeID];

                auto SrcNode = DStreetMap->NodeByID(SrcNodeID);
                auto DestNode = DStreetMap->NodeByID(DestNodeID);
                double distance = SGeographicUtils::HaversineDistanceInMiles(SrcNode->Location(), DestNode->Location()); 

                // Walk edge
                DAdjacencies[SrcVertexID].emplace_back(DestVertexID, distance, ETransportationMode::Walk);
                if(!oneway){
                    DAdjacencies[DestVertexID].emplace_back(SrcVertexID, distance, ETransportationMode::Walk);
                }
                // Bike edge
                if(canBike){
                    DAdjacencies[SrcVertexID].emplace_back(DestVertexID, distance, ETransportationMode::Bike);
                    if(!oneway){
                        DAdjacencies[DestVertexID].emplace_back(SrcVertexID, distance, ETransportationMode::Bike);
                    }
                }
                
            }
        }

        // Process bussystem and add bus edge to it
        for(size_t RouteIndex = 0; RouteIndex < DBusIndexer->RouteCount(); RouteIndex++){
            auto Route = DBusIndexer->SortedRouteByIndex(RouteIndex);

            if(!Route) {
                continue;
            }
            // Iterate every stop in the route
            for (size_t Stopindex = 0; Stopindex + 1 < Route->StopCount(); Stopindex++) {
                auto SrcStopID = Route->GetStopID(Stopindex);
                auto DestStopID = Route->GetStopID(Stopindex + 1);

                auto SrcStop = DBusSystem->StopByID(SrcStopID);
                auto DestStop = DBusSystem->StopByID(DestStopID);
                if (!SrcStop || !DestStop) {
                    continue;
                } // Check for existence

                TNodeID SrcNodeID = SrcStop->NodeID();
                TNodeID DestNodeID = DestStop->NodeID();

                auto SrcVertexID = DNodeToVertex[SrcNodeID];
                auto DestVertexID = DNodeToVertex[DestNodeID];

                auto SrcNode = DStreetMap->NodeByID(SrcNodeID);
                auto DestNode = DStreetMap->NodeByID(DestNodeID);
                double distance = SGeographicUtils::HaversineDistanceInMiles(SrcNode->Location(), DestNode->Location());

                DAdjacencies[SrcVertexID].emplace_back(DestVertexID, distance, ETransportationMode::Bus);
            }
        }
    }

    ~SImplementation(){

    }
    
    // Returns the number of nodes in the street map
    std::size_t NodeCount() const noexcept{
        return DStreetMap->NodeCount();
    }

    // Returns the street map node specified by index if index is less than the 
    // NodeCount(). nullptr is returned if index is greater than or equal to 
    // NodeCount(). The nodes are sorted by Node ID.
    std::shared_ptr<CStreetMap::SNode> SortedNodeByIndex(std::size_t index) const noexcept{
        if(index < NodeCount()){
            return DStreetMap->NodeByID(DSortedNodeIDs[index]);// DSortedNodeIDs[index] gets the sorted nodeid by index, NodeByID gets teh node ptr by nodeid
        }
        else{
            return nullptr;
        }
    }

    // Returns the distance in miles between the src and dest nodes of the 
    // shortest path if one exists. NoPathExists is returned if no path exists. 
    // The nodes of the shortest path are filled in the path parameter.
    double FindShortestPath(TNodeID src, TNodeID dest, std::vector< TNodeID > &path){
        path.clear();
        auto SrcIt = DNodeToVertex.find(src);
        auto DestIt = DNodeToVertex.find(dest);

        // vertex doesn't exist retrun nopathexists
        if(SrcIt == DNodeToVertex.end() || DestIt == DNodeToVertex.end()){
            return NoPathExists;
        }

        CPathRouter::TVertexID SrcVertexID = SrcIt->second;
        CPathRouter::TVertexID DestVertexID = DestIt->second;
        
        std::vector< CPathRouter::TVertexID > VertexPath;
        double dist = DRouter->FindShortestPath(SrcVertexID,DestVertexID,VertexPath);
        //path stores a vector of Vertex ids so we need to change it back to nodeID
        if(dist == NoPathExists){
            return NoPathExists;
        }
        else{
            for(CPathRouter::TVertexID VertexID : VertexPath){
                path.push_back(DVertexToNode[VertexID]);
            }
            return dist;
        }
    }

    // Returns the time in hours for the fastest path between the src and dest 
    // nodes of the if one exists. NoPathExists is returned if no path exists. 
    // The transportation mode and nodes of the fastest path are filled in the 
    // path parameter.
    // Implementation:
    // Create a new graph with time as the weight of edge
    double FindFastestPath(TNodeID src, TNodeID dest, std::vector< TTripStep > &path){
        path.clear();

        // Traverse all edge in the Dstreetmap and calculate different time for this edge via different transportation
        for()

    }

    // Returns true if the path description is created. Takes the trip steps path
    // and converts it into a human readable set of steps.
    bool GetPathDescription(const std::vector< TTripStep > &path, std::vector<std::string> &desc) const {
        return true;
    }

};

const double CDijkstraTransportationPlanner::SImplementation::NoPathExists = std::numeric_limits<double>::max();


// CDijkstraTransportationPlanner member functions
// Constructor for the Dijkstra Transportation Planner
CDijkstraTransportationPlanner::CDijkstraTransportationPlanner(std::shared_ptr<SConfiguration> config){
    DImplementation = std::make_unique<SImplementation>(config);
}

// Destructor for the Dijkstra Transportation Planner
CDijkstraTransportationPlanner::~CDijkstraTransportationPlanner(){
    
};

// Returns the number of nodes in the street map
std::size_t CDijkstraTransportationPlanner::NodeCount() const noexcept {
    return DImplementation->NodeCount();
}

// Returns the street map node specified by index if index is less than the 
// NodeCount(). nullptr is returned if index is greater than or equal to 
// NodeCount(). The nodes are sorted by Node ID.
std::shared_ptr<CStreetMap::SNode> CDijkstraTransportationPlanner::SortedNodeByIndex(std::size_t index) const noexcept {
    return DImplementation->SortedNodeByIndex(index);
}

// Returns the distance in miles between the src and dest nodes of the 
// shortest path if one exists. NoPathExists is returned if no path exists. 
// The nodes of the shortest path are filled in the path parameter.
double CDijkstraTransportationPlanner::FindShortestPath(TNodeID src, TNodeID dest, std::vector< TNodeID > &path) {
    return DImplementation->FindShortestPath(src,dest,path);
}

// Returns the time in hours for the fastest path between the src and dest 
// nodes of the if one exists. NoPathExists is returned if no path exists. 
// The transportation mode and nodes of the fastest path are filled in the 
// path parameter.
double CDijkstraTransportationPlanner::FindFastestPath(TNodeID src, TNodeID dest, std::vector< TTripStep > &path) {
    return DImplementation->FindFastestPath(src,dest,path);
}

// Returns true if the path description is created. Takes the trip steps path
// and converts it into a human readable set of steps.
bool CDijkstraTransportationPlanner::GetPathDescription(const std::vector< TTripStep > &path, std::vector< std::string > &desc) const {
    return DImplementation->GetPathDescription(path,desc);
}
