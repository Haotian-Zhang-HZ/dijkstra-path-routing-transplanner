#include "DijkstraPathRouter.h"



struct CDijkstraPathRouter::SImplementation{

    SImplementation(){

    }
    ~SImplementation(){}

    // Returns the number of vertices in the path router
    std::size_t VertexCount() const noexcept{

    }

    // Adds a vertex with the tag provided. The tag can be of any type.
    TVertexID AddVertex(std::any tag) noexcept{

    }

    // Gets the tag of the vertex specified by id if id is in the path router.
    // A std::any() is returned if id is not a valid vertex ID.
    std::any GetVertexTag(TVertexID id) const noexcept{

    }

    // Adds an edge between src and dest vertices with a weight of weight. If 
    // bidir is set to true an additional edge between dest and src is added. If 
    // src or dest nodes do not exist, or if the weight is negative the AddEdge 
    // will return false, otherwise it returns true.
    bool AddEdge(TVertexID src, TVertexID dest, double weight, bool bidir = false) noexcept{

    }

    // Allows the path router to do any desired precomputation up to the deadline
    bool Precompute(std::chrono::steady_clock::time_point deadline) noexcept{

    }

    // Returns the path distance of the path from src to dest, and fills out path
    // with vertices. If no path exists NoPathExists is returned.
    double FindShortestPath(TVertexID src, TVertexID dest, std::vector<TVertexID> &path) noexcept{

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
    bool CDijkstraPathRouter::AddEdge(TVertexID src, TVertexID dest, double weight, bool bidir = false) noexcept{
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