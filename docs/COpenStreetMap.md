# COpenStreetMap

## Overview
`COpenStreetMap` is a concrete implementation of `CStreetMap` that reads street map data from an OSM (OpenStreetMap) XML file using a `CXMLReader`.  
It provides access to nodes and ways through indices and IDs, following the `CStreetMap` interface.

- **Nodes** represent points on the map (latitude/longitude) and can have attributes.  
- **Ways** represent paths connecting nodes and can also have attributes.

---

## Public Interface

`COpenStreetMap` exposes the following functions:

- `COpenStreetMap(std::shared_ptr<CXMLReader> src)`
    - Constructs a `COpenStreetMap` by parsing the OSM XML provided by the reader.
    - Parameters:
        - `src`: a shared pointer to a `CXMLReader` that reads OSM XML data.

- `std::size_t NodeCount() const noexcept`
    - Returns the number of nodes parsed from the OSM data.

- `std::size_t WayCount() const noexcept`
    - Returns the number of ways parsed from the OSM data.

- `std::shared_ptr<CStreetMap::SNode> NodeByIndex(std::size_t index) const noexcept`
    - Returns the node object at the given sequential index.
    - Returns `nullptr` if `index >= NodeCount()`.

- `std::shared_ptr<CStreetMap::SNode> NodeByID(TNodeID id) const noexcept`
    - Returns the node object corresponding to the given node ID.
    - Returns `nullptr` if the node ID does not exist.

- `std::shared_ptr<CStreetMap::SWay> WayByIndex(std::size_t index) const noexcept`
    - Returns the way object at the given sequential index.
    - Returns `nullptr` if `index >= WayCount()`.

- `std::shared_ptr<CStreetMap::SWay> WayByID(TWayID id) const noexcept`
    - Returns the way object corresponding to the given way ID.
    - Returns `nullptr` if the way ID does not exist.

---

## Behavior and Rules

- **XML Parsing**
    - The class reads an OSM XML file with `<node>` and `<way>` elements.
    - Nodes must have attributes: `id`, `lat`, and `lon`.
    - Ways must have attribute `id` and reference nodes using `<nd ref="nodeID"/>`.
    - Node and way attributes (e.g., `<tag k="highway" v="residential"/>`) are stored and accessible.

- **Duplicate Handling**
    - Duplicate node IDs or way IDs will clear the container and cause parsing to fail.
    - Duplicate attributes in a node or way will also cause parsing to fail.

- **Error Handling**
    - Invalid numeric conversions (e.g., non-integer IDs) will cause parsing to fail.
    - Ways referencing nonexistent nodes will cause parsing to fail.
    - On any parsing error, all parsed nodes and ways are cleared.

- **Access Rules**
    - Index-based access is 0-indexing, will return `nullptr` for index out of range.
    - ID-based access returns `nullptr` for non-existent IDs.
    - Node and way attributes can be queried using `HasAttribute` and `GetAttribute`. Out-of-range indices or non-existing keys return empty strings or `false`.

---

## Usage Example

```cpp
#include <gtest/gtest.h>
#include "OpenStreetMap.h"
#include "StringDataSource.h"

auto OSMDataSource = std::make_shared<CStringDataSource>(
    "<?xml version='1.0' encoding='UTF-8'?>\n"
    "<osm version=\"0.6\" generator=\"osmconvert 0.8.5\">\n"
    "  <node id=\"1\" lat=\"38.5\" lon=\"-121.7\">\n"
    "       <tag k=\"highway\" v=\"traffic_signals\"/>\n"
    "  </node>\n"
    "  <node id=\"2\" lat=\"38.5\" lon=\"-121.8\"/>\n"
    "  <way id=\"100\">\n"
    "    <nd ref=\"1\"/>\n"
    "    <nd ref=\"2\"/>\n"
    "    <tag k=\"highway\" v=\"residential\"/>\n"
    "    <tag k=\"name\" v=\"Hacienda Avenue\"/>\n"
    "  </way>\n"
    "</osm>"
);
auto OSMReader = std::make_shared<CXMLReader>(OSMDataSource);
COpenStreetMap Map(OSMReader);

// Node access
EXPECT_EQ(Map.NodeCount(), 2);
auto Node1 = Map.NodeByID(1);
EXPECT_EQ(Node1->ID(), 1);
EXPECT_EQ(Node1->Location(), std::make_pair(38.5, -121.7));
EXPECT_TRUE(Node1->HasAttribute("highway"));
EXPECT_EQ(Node1->GetAttribute("highway"), "traffic_signals");

auto Node2 = Map.NodeByIndex(1);
EXPECT_EQ(Node2->ID(), 2);
EXPECT_EQ(Node2->AttributeCount(), 0);

// Way access
EXPECT_EQ(Map.WayCount(), 1);
auto Way100 = Map.WayByID(100);
EXPECT_EQ(Way100->ID(), 100);
EXPECT_EQ(Way100->NodeCount(), 2);
EXPECT_EQ(Way100->GetNodeID(0), 1);
EXPECT_EQ(Way100->GetNodeID(1), 2);
EXPECT_TRUE(Way100->HasAttribute("highway"));
EXPECT_EQ(Way100->GetAttribute("name"), "Hacienda Avenue");