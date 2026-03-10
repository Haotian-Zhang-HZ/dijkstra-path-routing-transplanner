#include "DijkstraTransportationPlanner.h"
#include "BusSystemIndexer.h"
#include "DijkstraPathRouter.h"
#include "PathRouter.h"
#include "GeographicUtils.h"
#include <unordered_map>
#include <queue>
#include <limits>

struct CDijkstraTransportationPlanner::SImplementation{
    const std::string BIKETAG = "bicycle";
    
    static const double NoPathExists;
    using TNodeID = CStreetMap::TNodeID;
    using TTripStep = CTransportationPlanner::TTripStep;

    double DWalkSpeed;
    double DBikeSpeed;
    double DBusSpeed;

    
    static constexpr size_t InvalidBusIndex = std::numeric_limits<size_t>::max(); // Invalid busindex as default value
    // In Dijkstra's priority queue, we want to store 1.traveling time to dest, 2.curr vertex, 3.Curr Busroute 4.number of intermidiate bus stops(for calculate stoptime)
    // So, we need a new datastructure
    struct PlannerVertex{
        double TimeTraveled;
        CPathRouter::TVertexID Vertex; 
        size_t CurrBusIndex = InvalidBusIndex;
        // size_t BusStopsPassed = 0; 
        // Getter function interface

        double GetTime() const {
            return TimeTraveled;
        }

        CPathRouter::TVertexID GetVertexID() const{
            return Vertex;
        }

        size_t GetBusIndex() const{
            return CurrBusIndex;
        }

        // size_t GetStopsPassed() const {
        //     return BusStopsPassed;
        // }
    };

    struct SPlannerEdge{

        CPathRouter::TVertexID DDest; // Since we use adjacency list to represent a graph, we don't need to store the src(adj list just store a verctor of dest mapping to a src)
        double DTime;
        ETransportationMode Mode;
        size_t BusRouteIndex;

        SPlannerEdge(
            CPathRouter::TVertexID dest, double Time, ETransportationMode mode, size_t busindex = InvalidBusIndex) : DDest(dest), DTime(Time), Mode(mode),BusRouteIndex(busindex) {

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

    struct pairhash{
        std::size_t operator()(const std::pair<TNodeID,TNodeID> &pair) const {
            std::size_t First = pair.first;
            std::size_t Second = pair.second;
            return First ^ (Second<<1); 
        }
    };
    std::unordered_map<std::pair<TNodeID,TNodeID>, double, pairhash> DMaxSpeedBtwNodes;

    // Initialize the streetmap and bussystem we use from the config and create a bus indexer and router in the constructor
    SImplementation(std::shared_ptr<SConfiguration> config) 
        :   DConfig(config),
            DStreetMap(config->StreetMap()),
            DBusSystem(config->BusSystem()),
            DBusIndexer(std::make_shared<CBusSystemIndexer>(DBusSystem)),
            DRouter(std::make_shared<CDijkstraPathRouter>()),
            DWalkSpeed(config->WalkSpeed()),
            DBikeSpeed(config->BikeSpeed()),
            DBusSpeed(config->DefaultSpeedLimit()){
        
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

            // Extract speed limit if provided
            double SpeedLimit = DBusSpeed; 
            if(Way->HasAttribute("maxspeed")){
                std::string SpeedStr = Way->GetAttribute("maxspeed");
                SpeedLimit = std::stod(SpeedStr); // <tag k="maxspeed" v="25 mph"/>
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

                auto SrcIt = DNodeToVertex.find(SrcNodeID);
                auto DestIt = DNodeToVertex.find(DestNodeID);

                if(SrcIt == DNodeToVertex.end() || DestIt == DNodeToVertex.end()){
                    continue;
                }

                auto SrcVertexID = SrcIt->second;
                auto DestVertexID = DestIt->second;

                auto SrcNode = DStreetMap->NodeByID(SrcNodeID);
                auto DestNode = DStreetMap->NodeByID(DestNodeID);
                double Distance = SGeographicUtils::HaversineDistanceInMiles(SrcNode->Location(), DestNode->Location()); 

                // Add edge to router (weight = physical distance) 
                DRouter->AddEdge(SrcVertexID, DestVertexID, Distance);
                if (!oneway) {
                    DRouter->AddEdge(DestVertexID, SrcVertexID, Distance);
                }
                
                // Walk edge for Planner time-weighted
                double WalkTime = Distance / DWalkSpeed;
                DAdjacencies[SrcVertexID].emplace_back(DestVertexID, WalkTime, ETransportationMode::Walk);
                if(!oneway){
                    DAdjacencies[DestVertexID].emplace_back(SrcVertexID, WalkTime, ETransportationMode::Walk);
                }

                // Bike edge for planner time-weighted
                if(canBike){
                    double BikeTime = Distance / DBikeSpeed;
                    DAdjacencies[SrcVertexID].emplace_back(DestVertexID, BikeTime, ETransportationMode::Bike);
                    if(!oneway){
                        DAdjacencies[DestVertexID].emplace_back(SrcVertexID, BikeTime, ETransportationMode::Bike);
                    }
                }
                
                // Record street segment speed limit
                DMaxSpeedBtwNodes[{SrcNodeID,DestNodeID}] = SpeedLimit;
                if(!oneway){
                    DMaxSpeedBtwNodes[{DestNodeID,SrcNodeID}] = SpeedLimit;
                }

            }
        }

        // Process bussystem and add bus edge to it
        // Note: we only record edge between busstop with the weight being the time traveled
        // Adjacent busstop might not be adjacent nodes in the street map!
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

                if(!SrcStop || !DestStop){
                    continue;
                }

                TNodeID SrcNodeID = SrcStop->NodeID();
                TNodeID DestNodeID = DestStop->NodeID();

                // Adjacent stops mioght not be adjacent nodes in the map, 
                // and bus follows the shortest path, so use Findshortest path to find the actual bus route on the map
                // thus we can calculate the actual time between two busstops
                std::vector<CPathRouter::TVertexID> StreetPath; // Bus path on street map- vector of vertex in grapph 
                double StreetDistance = DRouter->FindShortestPath(DNodeToVertex[SrcNodeID],DNodeToVertex[DestNodeID],StreetPath);

                if(StreetDistance == NoPathExists){
                    continue;
                }
                double BusTime = 0;
                // NodeTraveledByBus is the node index traveled by bus in the path
                // Calculate the time and distance btw each segment(adjacent nodes in map) of path
                for(size_t NodeTraveledByBus = 0; NodeTraveledByBus + 1 < StreetPath.size(); NodeTraveledByBus++){
                    TNodeID CurrNode = DVertexToNode[StreetPath[NodeTraveledByBus]];
                    TNodeID NextNode = DVertexToNode[StreetPath[NodeTraveledByBus + 1]];
                    double Distance = SGeographicUtils::HaversineDistanceInMiles(DStreetMap->NodeByID(CurrNode)->Location(), DStreetMap->NodeByID(NextNode)->Location());

                    double SpeedLimit = DBusSpeed; 
                    auto It = DMaxSpeedBtwNodes.find({CurrNode, NextNode});
                    if(It != DMaxSpeedBtwNodes.end()) {
                        SpeedLimit = It->second;
                    }
                    BusTime += Distance / SpeedLimit;
                }
                if (!SrcStop || !DestStop) {
                    continue;
                } // Check for existence

                CPathRouter::TVertexID SrcVertexID = DNodeToVertex[SrcNodeID];
                CPathRouter::TVertexID DestVertexID = DNodeToVertex[DestNodeID];

                DAdjacencies[SrcVertexID].emplace_back(DestVertexID, BusTime, ETransportationMode::Bus,RouteIndex);
            }
        }
        
        //DRouter->Precompute(std::chrono::steady_clock::now() + std::chrono::seconds(30));
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
    // Traverse the graph with traveling time as the weight of edge(busstoptime is computed in running time)
    double FindFastestPath(TNodeID src, TNodeID dest, std::vector< TTripStep > &path){
        path.clear();
        // Sanity check for src and dest
        if(DNodeToVertex.find(src) == DNodeToVertex.end() || DNodeToVertex.find(dest) == DNodeToVertex.end()){
            return NoPathExists;
        }

        // Get vertex id from the node id passed in.
        CPathRouter::TVertexID SrcVertexID = DNodeToVertex[src];
        CPathRouter::TVertexID DestVertexID = DNodeToVertex[dest];

        // To store user-defined struct in priority Q, we need ca ustom comparator struct
        struct ComparePlannerVertexPtr {
            bool operator()(const std::shared_ptr<PlannerVertex> &a, const std::shared_ptr<PlannerVertex> &b) const {
                return a->TimeTraveled > b->TimeTraveled; // min-heap
            }
        };

        std::priority_queue< std::shared_ptr<PlannerVertex>, std::vector<std::shared_ptr<PlannerVertex>>, ComparePlannerVertexPtr> PriorityQ;

        // Initialization for Dijkstra: Start vertex
        auto SrcVertex = std::make_shared<PlannerVertex>();
        SrcVertex->Vertex = SrcVertexID;
        SrcVertex->TimeTraveled = 0;
        SrcVertex->CurrBusIndex = InvalidBusIndex;
        // SrcVertex->BusStopsPassed = 0;

        PriorityQ.push(SrcVertex);


        // Store the mintime from src to every vertex in the graph
        std::unordered_map<CPathRouter::TVertexID, double> MinTime;
        MinTime[SrcVertexID] = 0;

        // Create a new struct For parent info - make it easier to backtrack and get the info we need
        struct ParentInfo {
            CPathRouter::TVertexID PrevVertex;
            ETransportationMode Mode;
            size_t BusRouteIndex;
        };
        // Parent map for backtrack
        std::unordered_map<CPathRouter::TVertexID, ParentInfo> Parent;

        // Traverse all edge in the Dstreetmap and calculate different time for this edge via different transportation
        while(!PriorityQ.empty()){
            // Extract and remove the vertex with min time from the src
            auto CurrPtr = PriorityQ.top();
            PriorityQ.pop();

            auto CurrVertexID = CurrPtr->Vertex;
            double CurrTime = CurrPtr->TimeTraveled;
            size_t CurrBus = CurrPtr->CurrBusIndex;
            // size_t StopsPassed = CurrPtr->BusStopsPassed;

            // If the recorded min time beats the current time(get from Q), this current pair must be outdated data, so we could safely skip it.
            if (MinTime.find(CurrVertexID) != MinTime.end() && CurrTime > MinTime[CurrVertexID]) {
                continue;
            }

            // Stop if reach dest
            if (CurrVertexID == DestVertexID) {
                break;
            }
            for (const auto &Edge : DAdjacencies[CurrVertexID]) {

                CPathRouter::TVertexID NextVertex = Edge.GetDestID();
                double TravelTime = Edge.GetTime();// Traveling time of this edge
                ETransportationMode Mode = Edge.Mode;// Travelin mode of this edge

                double NextTime = CurrTime;
                size_t NextBus = CurrBus;
                // size_t NextStops = StopsPassed;

                // Process stoptime if in bus mode
                if (Mode == ETransportationMode::Bus) {
                    // Get on a new bus, update bus info for next edge 
                    if (CurrBus != Edge.BusRouteIndex) {
                        NextBus = Edge.BusRouteIndex;
                    }
                    // add one stoptime for each edge traversed, in this way, we only add stoptime for intermidiate stops
                    NextTime += DConfig->BusStopTime() / 3600.0;
                }
                // reset busindex if switch to walk
                else if(Mode == ETransportationMode::Walk){
                    NextBus = InvalidBusIndex;
                }

                NextTime += TravelTime;

                // relax edge if possible
                if (MinTime.find(NextVertex) == MinTime.end() || NextTime < MinTime[NextVertex]) {

                    MinTime[NextVertex] = NextTime;

                    auto NextPtr = std::make_shared<PlannerVertex>();
                    NextPtr->Vertex = NextVertex;
                    NextPtr->TimeTraveled = NextTime;
                    NextPtr->CurrBusIndex = NextBus;

                    Parent[NextVertex] = {CurrVertexID, Mode, NextBus};

                    PriorityQ.push(NextPtr);
                }
            }
        }
        if (MinTime.find(DestVertexID) == MinTime.end()) {
            return NoPathExists;
        }

        // Backtrack and construct path 
        std::vector<TTripStep> ReverseTripSteps;

        CPathRouter::TVertexID CurrVertexID = DestVertexID;

        while (CurrVertexID != SrcVertexID) {
            auto it = Parent.find(CurrVertexID);
            
            // if cannot find Parent，path doesn;t exist
            if (it == Parent.end()) {           
                path.clear();
                return NoPathExists;
            }
            ParentInfo info = it->second;

             // push current node with edge mode to previous node
            ReverseTripSteps.push_back({info.Mode, DVertexToNode[CurrVertexID]});

            // move to previous vertex
            CurrVertexID = info.PrevVertex;
        }

        // Determine src node's mode based on first edge
        if (!ReverseTripSteps.empty()) {
            ETransportationMode FirstMode = ReverseTripSteps.back().first;
            if (FirstMode == ETransportationMode::Bike) {
                ReverseTripSteps.push_back({ETransportationMode::Bike, DVertexToNode[SrcVertexID]});
            } else {
                ReverseTripSteps.push_back({ETransportationMode::Walk, DVertexToNode[SrcVertexID]});
            }
        } else {
            // src == dest, assume Walk
            ReverseTripSteps.push_back({ETransportationMode::Walk, DVertexToNode[SrcVertexID]});
        }
        
        // reverse to get the order from src to dest
        std::reverse(ReverseTripSteps.begin(), ReverseTripSteps.end());

        // O(1) time move to path
        path = std::move(ReverseTripSteps);

        return MinTime[DestVertexID];
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
