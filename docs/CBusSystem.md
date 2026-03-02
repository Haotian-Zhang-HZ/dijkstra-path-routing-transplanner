# CBusSystem

## Overview
`CBusSystem` provides an abstract interface for bus system. 
It contains information about stops and routes in the network. 
Stops are identified by unique IDs and correspond to nodes in a street map, 
while routes are sequences of stops with a name.  

---

## Public Interface

### Types

- `using TStopID = uint64_t`
- `using TWayID = uint64_t`
- `static const TStopID InvalidStopID` 

---

### SStop (inner struct)
Represents a single stop in the bus network.

- `virtual TStopID ID() const noexcept`
    - Returns the unique stop ID of this stop.
- `virtual CStreetMap::TNodeID NodeID() const noexcept`
    - Returns the node ID in the street map associated with this stop.

---

### SRoute (inner struct)
Represents a single bus route as a sequence of stops.

- `virtual std::string Name() const noexcept`
    - Returns the name of the route.
- `virtual std::size_t StopCount() const noexcept`
    - Returns the number of stops in the route.
- `virtual TStopID GetStopID(std::size_t index) const noexcept`
    - Returns the stop ID at the specified index.
    - If index is out of bounds, behavior depends on the implementation (would return `InvalidStopID` for my CCSVBusSystem).

---

### CBusSystem 

- `virtual std::size_t StopCount() const noexcept`
    - Returns the total number of stops in the system.
- `virtual std::size_t RouteCount() const noexcept`
    - Returns the total number of routes in the system.
- `virtual std::shared_ptr<SStop> StopByIndex(std::size_t index) const noexcept`
    - Returns the stop object at the given index.
- `virtual std::shared_ptr<SStop> StopByID(TStopID id) const noexcept`
    - Returns the stop object corresponding to the given stop ID.
- `virtual std::shared_ptr<SRoute> RouteByIndex(std::size_t index) const noexcept`
    - Returns the route object at the given index.
- `virtual std::shared_ptr<SRoute> RouteByName(const std::string &name) const noexcept`
    - Returns the route object corresponding to the given name.

---

## Behavior and Rules

### Stop and Route Identification
- Each stop is uniquely identified by `TStopID`.
- Each route has a unique name (`std::string`) and a sequence of stops.

### Access Rules
- Index-based access assumes 0-based indexing.
- Accessing a stop or route by ID or name assumes the ID/name exists; otherwise behavior depends on implementation.

---

## Usage Example
- N/A 
- CBusSystem is a abstract class that can't be instantiated