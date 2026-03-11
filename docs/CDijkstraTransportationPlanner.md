# CDijkstraTransportationPlanner

## Overview
`CDijkstraTransportationPlanner` is a concrete implementation of `CTransportationPlanner`  
that uses Dijkstra's algorithm to compute shortest and fastest paths.  
It supports walking, biking, and bus routing, with precomputation(not included in my implementation) and optimizations for large street maps.

---

## Public Interface

### Inherited Types
- `using TNodeID = CTransportationPlanner::TNodeID`  
- `using TTripStep = CTransportationPlanner::TTripStep`  
- `using ETransportationMode = CTransportationPlanner::ETransportationMode`  

---

### Configuration
Inherits `SConfiguration` from `CTransportationPlanner`.  
Used to provide street map, bus system, and speed parameters.

---

### CDijkstraTransportationPlanner (class)

- `CDijkstraTransportationPlanner(std::shared_ptr<SConfiguration> config)`
    - Constructs a planner with the given configuration.
- `~CDijkstraTransportationPlanner()`
    - Destructor.
- `std::size_t NodeCount() const noexcept`
    - Returns the number of nodes in the street map.
- `std::shared_ptr<CStreetMap::SNode> SortedNodeByIndex(std::size_t index) const noexcept`
    - Returns node by sorted index.
- `double FindShortestPath(TNodeID src, TNodeID dest, std::vector<TNodeID> &path)`
    - Computes the shortest path (distance-based) between `src` and `dest`.
- `double FindFastestPath(TNodeID src, TNodeID dest, std::vector<TTripStep> &path)`
    - Computes the fastest path (time-based) between `src` and `dest`, considering walking, biking, bus stops, and street speed limits.
- `bool GetPathDescription(const std::vector<TTripStep> &path, std::vector<std::string> &desc) const`
    - Converts a path into human-readable steps.
    - Note: not includede in my current implementation
---

## Behavior and Rules

### Dijkstra Algorithm
- Finds shortest or fastest paths using a priority queue.  
- Optimized by storing structs directly in the queue (avoids heap allocations).  
- Supports walking, biking, and bus travel.  

### Bus Integration
- Adjacent bus stops may not correspond to adjacent street nodes.  
- Bus edges are weighted by actual travel time, including inter-stop time.  
- Switching between bus routes adds bus stop time.

### Path Construction
- `FindShortestPath` returns node IDs of the shortest distance path.  
- `FindFastestPath` returns `TTripStep` with mode and node ID for each step.  
- `GetPathDescription` is intended to generate human-readable step descriptions.

### Special Values
- `NoPathExists` (`std::numeric_limits<double>::max()`) indicates no valid path.

---

## Usage Example

```cpp
std::shared_ptr<CTransportationPlanner::SConfiguration> config = ...;
CDijkstraTransportationPlanner planner(config);

// Compute shortest path
std::vector<TNodeID> path;
double shortest_distance = planner.FindShortestPath(src, dest, path);

// Compute fastest path
std::vector<CTransportationPlanner::TTripStep> trip;
double travel_time = planner.FindFastestPath(startNode, endNode, trip);

// Convert to human-readable description
std::vector<std::string> desc;
if (planner.GetPathDescription(trip, desc)) {
    for(const auto& line : desc)
        std::cout << line << std::endl;
}