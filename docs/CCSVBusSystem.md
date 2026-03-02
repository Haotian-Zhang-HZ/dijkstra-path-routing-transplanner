# CCSVBusSystem

## Overview
`CCSVBusSystem` is a concrete implementation of `CBusSystem` that reads bus stops and routes from CSV files using a `CDSVReader`.  
It provides access to bus stops and routes through indices, IDs, and names, following the `CBusSystem` interface.

---

## Public Interface
`CCSVBusSystem` exposes the following functions:

- `CCSVBusSystem(std::shared_ptr<CDSVReader> stopsrc, std::shared_ptr<CDSVReader> routesrc)`
    - Constructs a `CCSVBusSystem` by reading stops and routes from the given CSV readers.
    - Parameters:
        - `stopsrc`: CSV reader for stops
        - `routesrc`: CSV reader for routes

- `std::size_t StopCount() const noexcept`
    - Returns the number of bus stops.

- `std::size_t RouteCount() const noexcept`
    - Returns the number of routes.

- `std::shared_ptr<CBusSystem::SStop> StopByIndex(std::size_t index) const noexcept`
    - Returns the stop object at the given index.

- `std::shared_ptr<CBusSystem::SStop> StopByID(TStopID id) const noexcept`
    - Returns the stop object corresponding to the given stop ID.

- `std::shared_ptr<CBusSystem::SRoute> RouteByIndex(std::size_t index) const noexcept`
    - Returns the route object at the given index.

- `std::shared_ptr<CBusSystem::SRoute> RouteByName(const std::string &name) const noexcept`
    - Returns the route object corresponding to the given route name.
    - Returns `nullptr` if the route name does not exist.

---

## Behavior and Rules

- CSV files must have headers that correctly identify columns:
    - Stops: `"stop_id"` and `"node_id"`
    - Routes: `"route"` and `"stop_id"`
- If headers are missing or incorrect, stops or routes may not be loaded.
- Route stop indexing is safe:
    - `GetStopID(index)` returns `InvalidStopID` for out-of-range indices.
- Stops and routes are stored internally and accessible via indices or IDs/names.
- Empty CSV files or missing columns result in zero stops or routes.

---

## Usage Example

```cpp
#include <gtest/gtest.h>
#include "CSVBusSystem.h"
#include "StringDataSource.h"

auto StopDataSource = std::make_shared<CStringDataSource>(
    "stop_id,node_id\n"
    "1,123\n"
    "2,124"
);
auto StopReader = std::make_shared<CDSVReader>(StopDataSource, ',');

auto RouteDataSource = std::make_shared<CStringDataSource>(
    "route,stop_id\n"
    "A,1\n"
    "A,2"
);
auto RouteReader = std::make_shared<CDSVReader>(RouteDataSource, ',');

CCSVBusSystem BusSystem(StopReader, RouteReader);

// Access stops
EXPECT_EQ(BusSystem.StopCount(), 2);
auto Stop0 = BusSystem.StopByIndex(0);
EXPECT_EQ(Stop0->ID(), 1);
EXPECT_EQ(Stop0->NodeID(), 123);

auto Stop1 = BusSystem.StopByID(2);
EXPECT_EQ(Stop1->ID(), 2);
EXPECT_EQ(Stop1->NodeID(), 124);

// Access routes
EXPECT_EQ(BusSystem.RouteCount(), 1);
auto RouteA = BusSystem.RouteByName("A");
EXPECT_EQ(RouteA->Name(), "A");
EXPECT_EQ(RouteA->StopCount(), 2);
EXPECT_EQ(RouteA->GetStopID(0), 1);
EXPECT_EQ(RouteA->GetStopID(1), 2);

// Edge cases
EXPECT_EQ(BusSystem.RouteByName("NonExistent"), nullptr);
```