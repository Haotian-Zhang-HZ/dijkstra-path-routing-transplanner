# CDijkstraPathRouter

## Overview
`CDijkstraPathRouter` is a concrete implementation of `CPathRouter` that computes shortest paths between vertices using Dijkstra's algorithm.  
It stores a graph with vertices and weighted edges and supports optional precomputation for faster queries.  

Vertices are identified by `TVertexID` and can have arbitrary associated tags (`std::any`).  
Edges have weights and can be either directed or bidirectional.

---

## Public Interface
`CDijkstraPathRouter` exposes the following functions:

- `CDijkstraPathRouter()`
    - Constructs an empty Dijkstra path router.

- `~CDijkstraPathRouter()`
    - Destructor.

- `std::size_t VertexCount() const noexcept`
    - Returns the number of vertices in the graph.

- `TVertexID AddVertex(std::any tag) noexcept`
    - Adds a new vertex with the given tag.
    - Returns the vertex ID of the newly added vertex.

- `std::any GetVertexTag(TVertexID id) const noexcept`
    - Returns the tag associated with the given vertex.
    - Returns `std::any()` if the vertex ID is invalid.

- `bool AddEdge(TVertexID src, TVertexID dest, double weight, bool bidir = false) noexcept`
    - Adds an edge from `src` to `dest` with the given weight.
    - If `bidir` is true, adds a reverse edge from `dest` to `src`.
    - Returns `true` if the edge was added successfully; `false` if `src` or `dest` are invalid or `weight` is negative.

- `bool Precompute(std::chrono::steady_clock::time_point deadline) noexcept`
    - Performs optional precomputation of shortest paths up to the given deadline.
    - Returns `true` if precomputation succeeds, `false` otherwise.

- `double FindShortestPath(TVertexID src, TVertexID dest, std::vector<TVertexID> &path) noexcept`
    - Computes the shortest path from `src` to `dest`.
    - Fills `path` with the sequence of vertex IDs along the shortest path.
    - Returns the total weight of the path, or `CPathRouter::NoPathExists` if no path exists.

---

## Behavior and Rules

- Graph representation:
    - `SVertex` — represents a vertex in the graph, storing a tag.
    - `SEdge` — represents a weighted edge between vertices.
    - `DVertices` stores all vertices.
    - `DAdjacencies` is an adjacency list storing edges for each vertex.
- Dijkstra’s algorithm is used to compute shortest paths:
    - Precomputation fills internal tables for distances and parent vertices.
    - `FindShortestPath` reconstructs paths using the parent table.
- Vertex IDs are indices in the internal vectors.
- Out-of-range vertices or invalid paths result in `NoPathExists`.
- Edge weights must be non-negative; otherwise, `AddEdge` fails.

---

## Usage Example

```cpp
#include "DijkstraPathRouter.h"
#include <cassert>

CDijkstraPathRouter Router;

// Add vertices
auto v0 = Router.AddVertex("Start");
auto v1 = Router.AddVertex("Middle");
auto v2 = Router.AddVertex("End");

// Add edges
Router.AddEdge(v0, v1, 5.0);
Router.AddEdge(v1, v2, 3.0);
Router.AddEdge(v0, v2, 10.0);

// Precompute shortest paths (optional)
Router.Precompute(std::chrono::steady_clock::now() + std::chrono::seconds(1));

// Query shortest path
std::vector<CPathRouter::TVertexID> Path;
double Distance = Router.FindShortestPath(v0, v2, Path);

assert(Distance == 8.0); // v0 -> v1 -> v2
assert(Path.size() == 3);
assert(Path[0] == v0 && Path[1] == v1 && Path[2] == v2);