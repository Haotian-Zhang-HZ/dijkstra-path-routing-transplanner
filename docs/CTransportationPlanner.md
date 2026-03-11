# CTransportationPlanner

## Overview
`CTransportationPlanner` is an abstract interface for transportation planning.  
It provides methods to find shortest and fastest paths in a street map using multiple transportation modes: walking, biking, and bus.  
It relies on a street map (`CStreetMap`) and a bus system (`CBusSystem`) for routing.

---

## Public Interface

### Types

- `using TNodeID = CStreetMap::TNodeID`
- `using TTripStep = std::pair<ETransportationMode, TNodeID>`

---

### ETransportationMode (enum class)
Specifies the mode of transportation for a trip step.

- `Walk` — walking  
- `Bike` — biking  
- `Bus` — bus

---

### SConfiguration (inner struct)
Provides configuration options for the planner.

- `virtual ~SConfiguration()`
    - Destructor
- `virtual std::shared_ptr<CStreetMap> StreetMap() const noexcept`
    - Returns the street map used by the planner.
- `virtual std::shared_ptr<CBusSystem> BusSystem() const noexcept`
    - Returns the bus system used by the planner (may be `nullptr` if unused).
- `virtual double WalkSpeed() const noexcept`
    - Returns walking speed in units per time.
- `virtual double BikeSpeed() const noexcept`
    - Returns biking speed in units per time.
- `virtual double DefaultSpeedLimit() const noexcept`
    - Returns default speed limit for streets without explicit limits.
- `virtual double BusStopTime() const noexcept`
    - Returns the time spent at a bus stop.
- `virtual int PrecomputeTime() const noexcept`
    - Returns precomputation time in seconds for routing (optional optimization).

---

### CTransportationPlanner (class)

- `virtual ~CTransportationPlanner()`
    - Destructor
- `virtual std::size_t NodeCount() const noexcept`
    - Returns total number of nodes in the map.
- `virtual std::shared_ptr<CStreetMap::SNode> SortedNodeByIndex(std::size_t index) const noexcept`
    - Returns the node corresponding to a sorted index.
- `virtual double FindShortestPath(TNodeID src, TNodeID dest, std::vector<TNodeID> &path)`
    - Finds the shortest path (distance-based) between `src` and `dest`.  
      Fills `path` with node IDs and returns the total distance.
- `virtual double FindFastestPath(TNodeID src, TNodeID dest, std::vector<TTripStep> &path)`
    - Finds the fastest path (time-based) between `src` and `dest`.  
      Fills `path` with trip steps and returns travel time.
- `virtual bool GetPathDescription(const std::vector<TTripStep> &path, std::vector<std::string> &desc) const`
    - Converts a path into human-readable descriptions.  
      Returns `true` if successful.

---

## Behavior and Rules

### Path Computation
- `FindShortestPath` considers only distance, ignoring travel speed.  
- `FindFastestPath` considers speed per transportation mode, bus stop time, and street limits.  
- If no path exists, return value may indicate infinity.

### Node Access
- Nodes can be accessed via sorted index for deterministic ordering.  
- Node IDs must correspond to nodes in the `CStreetMap`.

### Trip Steps
- `TTripStep` records both the transportation mode and the node ID.  
- Paths using buses include stops as nodes and walking/biking as needed.

---

## Usage Example

```cpp
std::shared_ptr<CTransportationPlanner::SConfiguration> config = ...;
std::unique_ptr<CTransportationPlanner> planner = CreatePlanner(config);

// Find the fastest path
std::vector<CTransportationPlanner::TTripStep> trip;
double travel_time = planner->FindFastestPath(startNode, endNode, trip);

// Get human-readable description
std::vector<std::string> desc;
if (planner->GetPathDescription(trip, desc)) {
    for(const auto& line : desc)
        std::cout << line << std::endl;
}