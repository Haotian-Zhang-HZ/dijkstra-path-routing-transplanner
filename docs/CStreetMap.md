# CStreetMap

## Overview
`CStreetMap` provides an abstract interface for representing a street map.  
It contains information about **nodes** (locations on the map) and **ways** (paths connecting nodes).  
Nodes are uniquely identified by IDs. Nodes must have associated attributes such as latitude, longitude, and may have other attributes stored as tags.(e.g.,"<tag k=\"highway\" v=\"traffic_signals\"/>")  
Ways are sequences of nodes that have unique IDs. Ways can also have attributes stored as tags (e.g.,"<tag k=\"highway\" v=\"residential\"/>").  

---

## Public Interface

### Types

- `using TNodeID = uint64_t`  
  - Type alias for node IDs.
- `using TWayID = uint64_t`  
  - Type alias for way IDs.
- `using TLocation = std::pair<double, double>`  
  - Latitude and longitude of a node.
- `static const TNodeID InvalidNodeID`  
  - Represents an invalid or non-existent node ID (`std::numeric_limits<TNodeID>::max()`).
- `static const TWayID InvalidWayID`  
  - Represents an invalid or non-existent way ID (`std::numeric_limits<TWayID>::max()`).

---

### SNode (inner struct)
Represents a single node in the street map.

- `virtual TNodeID ID() const noexcept`  
  - Returns the unique node ID.
- `virtual TLocation Location() const noexcept`  
  - Returns the `(latitude, longitude)` of the node.
- `virtual std::size_t AttributeCount() const noexcept`  
  - Returns the number of attributes associated with the node.
- `virtual std::string GetAttributeKey(std::size_t index) const noexcept`  
  - Returns the attribute key at the specified index.  
  - If `index` is out of bounds, returns an empty string.
- `virtual bool HasAttribute(const std::string &key) const noexcept`  
  - Returns `true` if the node has an attribute with the given key.
- `virtual std::string GetAttribute(const std::string &key) const noexcept`  
  - Returns the value of the attribute specified by key.  
  - Returns an empty string if the key does not exist.

---

### SWay (inner struct)
Represents a single way (path) in the street map.

- `virtual TWayID ID() const noexcept`  
  - Returns the unique way ID.
- `virtual std::size_t NodeCount() const noexcept`  
  - Returns the number of nodes in the way.
- `virtual TNodeID GetNodeID(std::size_t index) const noexcept`  
  - Returns the node ID at the given index in this way.  
  - If `index` is out of bounds, returns `InvalidNodeID`.
- `virtual std::size_t AttributeCount() const noexcept`  
  - Returns the number of attributes associated with the way.
- `virtual std::string GetAttributeKey(std::size_t index) const noexcept`  
  - Returns the attribute key at the specified index.  
  - If `index` is out of bounds, returns an empty string.
- `virtual bool HasAttribute(const std::string &key) const noexcept`  
  - Returns `true` if the way has an attribute with the given key.
- `virtual std::string GetAttribute(const std::string &key) const noexcept`  
  - Returns the value of the attribute specified by key.  
  - Returns an empty string if the key does not exist.

---

### CStreetMap

- `virtual std::size_t NodeCount() const noexcept`  
  - Returns the total number of nodes in the map.
- `virtual std::size_t WayCount() const noexcept`  
  - Returns the total number of ways in the map.
- `virtual std::shared_ptr<SNode> NodeByIndex(std::size_t index) const noexcept`  
  - Returns the node object at the given sequential index.  
  - Returns `nullptr` if `index >= NodeCount()`.
- `virtual std::shared_ptr<SNode> NodeByID(TNodeID id) const noexcept`  
  - Returns the node object corresponding to the given ID.  
  - Returns `nullptr` if the ID does not exist.
- `virtual std::shared_ptr<SWay> WayByIndex(std::size_t index) const noexcept`  
  - Returns the way object at the given sequential index.  
  - Returns `nullptr` if `index >= WayCount()`.
- `virtual std::shared_ptr<SWay> WayByID(TWayID id) const noexcept`  
  - Returns the way object corresponding to the given ID.  
  - Returns `nullptr` if the ID does not exist.

---

## Behavior and Rules

### Node and Way Identification
- Each node is uniquely identified by `TNodeID`.
- Each way is uniquely identified by `TWayID`.

### Attribute Rules
- Attributes are key-value pairs (both `std::string`).
- Duplicate keys are not allowed for the same node or way.

### Access Rules
- Index-based access is **0-indexing**.
- All nodes/ways will be cleared if parsing errors occur (e.g., duplicate IDs or invalid values).

---

## Usage Example
- N/A 
- CStreetMap is a abstract class that can't be instantiated