# CPathRouter

## Overview
`CPathRouter` provides an abstract interface for a general path routing system.  
It manages a graph of vertices and weighted edges and allows computation of shortest paths between vertices.  

Vertices are identified by `TVertexID` and can have arbitrary associated tags (`std::any`).  
Edges have weights (typically representing distances or costs) and can be directed or bidirectional.  

---

## Public Interface

### Types

- `using TVertexID = std::size_t`  
    - Represents the unique ID of a vertex in the graph.
- `static constexpr TVertexID InvalidVertexID`  
    - Special constant indicating an invalid vertex.
- `static constexpr double NoPathExists`  
    - Returned when no path exists between two vertices.

---

### CPathRouter

- `virtual ~CPathRouter()`  
    - Virtual destructor for safe inheritance.

- `virtual std::size_t VertexCount() const noexcept`  
    - Returns the total number of vertices currently in the graph.

- `virtual TVertexID AddVertex(std::any tag) noexcept`  
    - Adds a new vertex to the graph with an associated tag.
    - Returns the `TVertexID` of the newly added vertex.

- `virtual std::any GetVertexTag(TVertexID id) const noexcept`  
    - Returns the tag associated with the given vertex.
    - Behavior is implementation-defined if the vertex ID is invalid.

- `virtual bool AddEdge(TVertexID src, TVertexID dest, double weight, bool bidir = false) noexcept`  
    - Adds an edge from `src` to `dest` with the given weight.
    - If `bidir` is true, adds a reverse edge as well.
    - Returns `true` if the edge was successfully added, `false` otherwise.

- `virtual bool Precompute(std::chrono::steady_clock::time_point deadline) noexcept`  
    - Allows precomputation of routing data before shortest path queries.
    - Precomputation can stop once `deadline` is reached.
    - Returns `true` if precomputation succeeded, `false` otherwise.

- `virtual double FindShortestPath(TVertexID src, TVertexID dest, std::vector<TVertexID> &path) noexcept`  
    - Finds the shortest path from `src` to `dest`.
    - Fills `path` with the sequence of vertex IDs along the shortest path.
    - Returns the total weight of the path, or `NoPathExists` if no path exists.

---

## Behavior and Rules

### Vertex and Edge Identification
- Each vertex is uniquely identified by a `TVertexID`.
- Each edge connects two vertices and has an associated weight.
- Graphs may be directed or bidirectional depending on the `bidir` flag when adding edges.

### Path Finding
- `FindShortestPath` must return the path with the minimal total weight.
- If no path exists, `NoPathExists` is returned and `path` may be empty.

### Precomputation
- Precomputation is optional; it may speed up future `FindShortestPath` queries.
- The system should respect the `deadline` and stop processing if time is exceeded.

---

## Usage Example
- N/A  
- `CPathRouter` is an abstract class and cannot be instantiated directly.  
- Implementations like `CDijkstraPathRouter` provide concrete behavior for shortest path computation.