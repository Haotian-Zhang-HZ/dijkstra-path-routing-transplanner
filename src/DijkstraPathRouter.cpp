#include "DijkstraPathRouter.h"
#include <limits>
#include <queue>
#include <stack>
#include <utility>

double Inf = std::numeric_limits<double>::infinity();// or max() as prof implemented

struct CDijkstraPathRouter::SImplementation{

    ////////////////////////////Define Data Structure for Graphs////////////////////
    struct SVertex{

        std::any DTag; // Nodeid/stopid in other system

        SVertex(std::any tag) : DTag(tag) {}

        ~SVertex(){}

        std::any GetTag(){
            return DTag;
        }        
    };

    struct SEdge{

        TVertexID DDest; // Since we use adjacency list to represent a graph, we don't need to store the src(adj list just store a verctor of dest mapping to a src)
        double DWeight;

        // enum class ETransportationMode {Walk, Bike, Bus};
        // ETransportationMode DMode; 

        SEdge(TVertexID dest, double weight) : DDest(dest), DWeight(weight){}

        ~SEdge(){}

        TVertexID GetDestID(){
            return DDest;
        }

        double GetWeight(){
            return DWeight;
        }

    };

    // A graph is composed by edges and vertices
    std::vector< std::shared_ptr<SVertex> > DVertices;
    std::vector< std::vector<std::shared_ptr<SEdge>> > DAdjacencies; // Adjacency list is a vector of (A vector of edges connecting to this src)
                                                  // I use a vector because it's faster (O(1)) to access an element than map (O(logV))
                                                  // And the instruction says that he vertex IDs do not have to match the node or stop IDs used by the other classes
    
                                                  // Below are used for precompting the whole graoh with dijkstra, nolonger used
    // std::vector<std::vector<double>> DDistanceBtwEveryPair; // create a dp table by using 2d array 
    //                                                         // DDistanceBtwEveryPair[src][dest] stores the shortest distance btw src and dest where both src and dest are TVertexID which are just the indexes of this 2d array                                          
    // //std::vector<std::vector<std::vector<TVertexID>>> DPathBtwEveryPair; // DPathBtwEveryPair[src][dest] stores a vector of vertices, which include the path from src to dest
    // std::vector<std::vector<TVertexID>> DParentBtwEveryPair;// DParentBtwEveryPair[src][dest] stores the previous node of dest in the shortest path from src to dest and we can create the path by going from the dest to src(push into a stack and pop will give the right order)
    
    SImplementation(){
            
    }
    ~SImplementation(){

    }

    // Returns the number of vertices in the path router
    // T(n) = O(1)
    std::size_t VertexCount() const noexcept{
        return DVertices.size();
    }

    // Adds a vertex with the tag provided. The tag can be of any type.
    // T(n) = O(1)
    TVertexID AddVertex(std::any tag) noexcept{
        std::shared_ptr<SVertex> NewVertex = std::make_shared<SVertex>(tag);
        DVertices.push_back(NewVertex);
        DAdjacencies.push_back({}); // When we create a vertex, also need to open up space for it in the adjacency list
        return DVertices.size() - 1; // newvertex always at the end of vector
    }

    // Gets the tag of the vertex specified by id if id is in the path router.
    // A std::any() is returned if id is not a valid vertex ID.
    std::any GetVertexTag(TVertexID id) const noexcept{
        if (id < VertexCount()){
            return DVertices[id]->DTag;
        }
        else{
            return std::any();
        }
    }

    // Adds an edge between src and dest vertices with a weight of weight. If 
    // bidir is set to true an additional edge between dest and src is added. If 
    // src or dest nodes do not exist, or if the weight is negative the AddEdge 
    // will return false, otherwise it returns true.
    bool AddEdge(TVertexID src, TVertexID dest, double weight, bool bidir = false) noexcept{
        if( src < VertexCount() && dest < VertexCount() && weight >= 0){
            std::shared_ptr<SEdge> NewEdge = std::make_shared<SEdge>(dest,weight);
            DAdjacencies[src].push_back(NewEdge);
            if(bidir){
                std::shared_ptr<SEdge> ReversedEdge = std::make_shared<SEdge>(src,weight);
                DAdjacencies[dest].push_back(ReversedEdge);
            }
            return true;
        }
        else{
            return false;
        }
    }    

    // Allows the path router to do any desired precomputation up to the deadline
    bool Precompute(std::chrono::steady_clock::time_point deadline) noexcept{
        return true;
        /*
        // Initialize 2 dp tables (size: VertexCount()*VertexCount()) -- Version 2
        // use constructor to set every elenemt of DAdjacencies to inf meaning distance btw src and dest is infinity
        // use constructor toset every elenemt of DPathBtwEveryPair to empty vector meaning path btw src and dest is null.
        DDistanceBtwEveryPair = std::vector<std::vector<double>>(VertexCount(),std::vector<double>(VertexCount(),Inf));
        //DPathBtwEveryPair = std::vector<std::vector<std::vector<TVertexID>>>(VertexCount(),std::vector<std::vector<TVertexID>>(VertexCount(),std::vector<TVertexID>()));
        DParentBtwEveryPair = std::vector<std::vector<TVertexID>>(VertexCount(),std::vector<TVertexID>(VertexCount(),InvalidVertexID));

        // Use a nested for loop to initialize 2 dp tables -- Version 1
        // for (TVertexID i = 0; i < VertexCount(); i++){
        //     for (TVertexID j = 0; j < VertexCount(); j++){
        //         DDistanceBtwEveryPair[i][j] = Inf;
        //         DPathBtwEveryPair[i][j] = {};
        //     }
        // }

        // std::vector<double> DistanceBtwSrcAndDest; // This vector will store the dist btw src and dest in every execution of dijkstra 
        // std::vector<std::vector<TVertexID>> PathFromSrcToDest; // This vector will store a vector of vertixes indicating path from src to dest in every execution of dijkstra 
        
        // Run Dijkstra's algorithm on every vertex to find the shortest path/diatance from src to rest of the vertices in the graph and fill up the table
        for(TVertexID src = 0; src < VertexCount(); src++){
            if(Dijkstra(src,DDistanceBtwEveryPair,DParentBtwEveryPair)){
                continue;
                // Previous version -- create new containers instead of directly modifying DDistanceBtwEveryPair and DPathBtwEveryPair
                // This will cost more space and time for copying, thus discard this method
                // // update the result (shortest path and distance from the src to rest of the vertices) after running dijkstra on each src
                // for(TVertexID dest = 0; dest < VertexCount(); dest++ ){
                //     DDistanceBtwEveryPair[src][dest] = DistanceBtwSrcAndDest[dest];
                //     DPathBtwEveryPair[src][dest] = PathFromSrcToDest[dest];
                // }
            }
            else{
                return false;
            }
        }
        return true;
        */
    }

    // Returns the path distance of the path from src to dest, and fills out path
    // with vertices. If no path exists NoPathExists is returned.
    double FindShortestPath(TVertexID src, TVertexID dest, std::vector<TVertexID> &path) noexcept{
        // Sanity check
        if(src >= VertexCount() || dest >=VertexCount()){
            return NoPathExists;
        }

        path.clear();
        // Single Dijkstra
        std::vector<double> Distance(VertexCount(), Inf);
        std::vector<TVertexID> Parent(VertexCount(), InvalidVertexID);

        // Initialize Src vertex
        Distance[src] = 0;
        Parent[src] = src;

        // Initialize a priority queue
        using PriorityQElement = std::pair<double, TVertexID>;
        std::priority_queue<PriorityQElement, std::vector<PriorityQElement>, std::greater<PriorityQElement>> PriorityQ;
        PriorityQ.emplace(0, src);

        while (!PriorityQ.empty()) {
            auto PriorityQPair = PriorityQ.top(); 
            double CurrDist = PriorityQPair.first;
            TVertexID CurrVertexID = PriorityQPair.second;
            PriorityQ.pop();

            // When we pop the dest the shortest path of it has been finalized,just end here
            if (CurrVertexID == dest) {
                break;
            }
            //****** Important!!! Since our Priority Queue doesn't support decreasekey and might contain outdated shortest dist value so we want to skip that to speed up!
            // What we can do is simply compare (the distance btw src and CurrVertexID)(-> this is what we get from priority queue) with the current best distance (-> this is stored in Distance)
            // If the recorded shortest distance beats the current distance(get from Q), this current pair must be outdated data, so we could safely skip it.
            if (CurrDist > Distance[CurrVertexID]) {
                continue;
            }

            for (const auto& AdjEdge : DAdjacencies[CurrVertexID]) {

                TVertexID AdjVertexID = AdjEdge->GetDestID();// Get the vertex ID of the adjacent vertex

                // And relax edge if necessary
                double Weight = AdjEdge->GetWeight();
                if (Distance[AdjVertexID] > CurrDist + Weight) {
                    Distance[AdjVertexID] = CurrDist + Weight;//
                    Parent[AdjVertexID] = CurrVertexID; 
                    PriorityQ.emplace(Distance[AdjVertexID], AdjVertexID);
                }
            }
        }

        if (Distance[dest] == Inf) {
            return NoPathExists;
        }

        // backtrack path with Parent 
        TVertexID CurrID = dest;
        while (CurrID != src) {
            if (CurrID == InvalidVertexID) {
                return NoPathExists;
            } 
            path.push_back(CurrID);
            CurrID = Parent[CurrID];
        }
        // Need to add the src
        path.push_back(src);
        std::reverse(path.begin(), path.end());

        return Distance[dest];
    }
};

/////////////////////////CDijkstraPathRouter Member function definition////////////////////////////


    // Constructor for the Dijkstra Path Router
    CDijkstraPathRouter::CDijkstraPathRouter(){
        DImplementation = std::make_unique<SImplementation>();
    }

    // Destructor for the Dijkstra Path Router
    CDijkstraPathRouter::~CDijkstraPathRouter(){

    }

    // Returns the number of vertices in the path router
    std::size_t CDijkstraPathRouter::VertexCount() const noexcept{
        return DImplementation->VertexCount();
    }

    // Adds a vertex with the tag provided. The tag can be of any type.
    CPathRouter::TVertexID CDijkstraPathRouter::AddVertex(std::any tag) noexcept{
        return DImplementation->AddVertex(tag);
    }

    // Gets the tag of the vertex specified by id if id is in the path router.
    // A std::any() is returned if id is not a valid vertex ID.
    std::any CDijkstraPathRouter::GetVertexTag(TVertexID id) const noexcept{
        return DImplementation->GetVertexTag(id);
    }

    // Adds an edge between src and dest vertices with a weight of weight. If 
    // bidir is set to true an additional edge between dest and src is added. If 
    // src or dest nodes do not exist, or if the weight is negative the AddEdge 
    // will return false, otherwise it returns true.
    bool CDijkstraPathRouter::AddEdge(TVertexID src, TVertexID dest, double weight, bool bidir) noexcept{
        return DImplementation->AddEdge(src, dest, weight, bidir);
    }

    // Allows the path router to do any desired precomputation up to the deadline
    bool CDijkstraPathRouter::Precompute(std::chrono::steady_clock::time_point deadline) noexcept{
        return DImplementation->Precompute(deadline);
    }

    // Returns the path distance of the path from src to dest, and fills out path
    // with vertices. If no path exists NoPathExists is returned.
    double CDijkstraPathRouter::FindShortestPath(TVertexID src, TVertexID dest, std::vector<TVertexID> &path) noexcept{
        return DImplementation->FindShortestPath(src, dest, path);
    }

    /* This dijkstra uesed to be desighed for precomputing the whole graph, no longer used
    // Helper function that run Dijkstra on the vertex passed in and update the shortest distance ang path btw the src and dest(rest of the verttices in the graph)
    // What I've changed/done to make it faster:
    //      1. pass in reference of the original dp table instead of storing the shortest dist and path relate to curr src in 2 extra containers that are passed in into Dijkstra
    //          and then update the global dp table with those 2 containers. This saves both space and time (of coping everything)
    //      2. safely skip outdated data by checking whether the distance in queue is larger than recorded distance stored in the global dp table. This saves time.
    //      3. delete the duplicated initialization for dp table. This save time.
    //      4. Avoid redundant 2D lookup by store it into a variable and call the variable multiple times
    bool Dijkstra(const TVertexID src, std::vector<std::vector<double>> &Distance, std::vector<std::vector<TVertexID>> &Parent){
        
        // Sanity check for inputs
        if(src >= VertexCount() || Distance[src].size() != VertexCount() || Parent[src].size() != VertexCount()){
            return false;
        }
        // // Initialization for Dijkstra
        // // I just notice that we have already initialized these in precompute, but if we want to use this function anywhere else, we might still need this. 
        // // Initialize the Distance vetor and Path
        // for (TVertexID dest = 0; dest < VertexCount();dest++){
        //     Distance[src][dest] = Inf;
        //     Path[src][dest].clear();
        // } 
        Distance[src][src] = 0; // set the distance to src as 0
        Parent[src][src] = src;

        // Initialize a priority queue
        std::priority_queue<std::pair<double, TVertexID>,std::vector<std::pair<double, TVertexID>>,std::greater<std::pair<double, TVertexID>>> PriorityQ; // priority_queue<Type, Container, Compare> default Compare is std::less<Type> thus creating a max-heap
                                                                                          // In this priority queue, we need to sort by distance from the src while keeping track of the vertexID
                                                                                          // So, I chooswe to use a pair to store the combination of dist and vertex id and set dist as the first element in pair because priority queue will sort by first element by default
        // Push the src vertex into the priority queue
        PriorityQ.push({0,src});

        // Main Routine of Dijkstra's Algorithm
        while (!PriorityQ.empty()){

            // Extract and remove the vertex with min distance from the src
            auto MinVertexPair = PriorityQ.top(); // std::pair<double, TVertexID>
            PriorityQ.pop();
            auto CurrVertexID = MinVertexPair.second;
            //****** Important!!! Since our Priority Queue doesn't support decreasekey and might contain outdated shortest dist value so we want to skip that to speed up!
            // What we can do is simply compare (the distance btw src and CurrVertexID)(-> this is what we get from priority queue) with the current best distance (-> this is stored in Distance)
            // If the recorded shortest distance beats the current distance(get from Q), this current pair must be outdated data, so we could safely skip it.
            auto DistFromSrcToCurr = MinVertexPair.first;
            if( DistFromSrcToCurr > Distance[src][CurrVertexID]){
                continue;
            }
            // Iterate through all adjacencies of the MinVertex (represented by SEdge)
            // Use range-based for here, where MinVertexPair.second is vertex id and use indexing of DAdjacencies to find the adjacencies of curr vertex (a vector of vertex ids)
            // use reference to save the time of copying
            for (const auto& AdjEdge : DAdjacencies[CurrVertexID]){

                // AdjEdge is ptr to SEdge 
                auto AdjVertexID = AdjEdge->GetDestID(); // Get the vertex ID of the adjacent vertex

                // And relax edge if necessary
                double& DistToAdj = Distance[src][AdjVertexID];
                double& DistToCurr = Distance[src][CurrVertexID];
                if (DistToAdj > DistToCurr + AdjEdge->GetWeight()){
                    DistToAdj = DistToCurr + AdjEdge->GetWeight(); // update the shortest distance to this vertex.
                    Parent[src][AdjVertexID] = CurrVertexID; // update the parent node of this adj node in the shortest path.
                    // Path[src][AdjVertexID] = Path[src][CurrVertexID];     // update shortest path to this adjacent vertex to "the path to the current vertex plus the edge between current edge and this adjacency. 
                    // Path[src][AdjVertexID].push_back(AdjVertexID);   // We connot merge these two lines because pushback returns void instead of a reference 
                    //**** Remember, we pass in refernce of the DistanceBtwSrcAndDest and PathFromSrcToDest specific for this src and we shouldn't change the  DDistanceBtwEveryPair and DPathBtwEveryPair directly
                    // std::pair<double, TVertexID> UpdatedAdjVertexPair = std::make_pair(DistToAdj,AdjVertexID);
                    // PriorityQ.push(UpdatedAdjVertexPair);
                    PriorityQ.emplace(DistToAdj,AdjVertexID);// this will save the cost of creating a new pair and copying everything
                }
            }
        }
        return true;
    }
    */