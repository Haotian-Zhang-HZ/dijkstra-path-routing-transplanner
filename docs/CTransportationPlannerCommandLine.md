# CTransportationPlannerCommandLine

## Overview
`CTransportationPlannerCommandLine` provides a command-line interface for interacting with a `CTransportationPlanner`.  
It reads commands from a data source, executes routing queries (shortest/fastest paths), and outputs results or errors to designated sinks.  

---

## Public Interface

### CTransportationPlannerCommandLine (class)

- `CTransportationPlannerCommandLine(std::shared_ptr<CDataSource> cmdsrc, std::shared_ptr<CDataSink> outsink, std::shared_ptr<CDataSink> errsink, std::shared_ptr<CDataFactory> results, std::shared_ptr<CTransportationPlanner> planner)`  
  Constructs a command-line interface using the given sources and sinks.

- `~CTransportationPlannerCommandLine()`  
  Destructor.

- `bool ProcessCommands()`  
  Reads and executes commands from the input source until `"exit"` command.  
  Supported commands include:
  - `help` – Display help menu.
  - `exit` – Exit the command loop.
  - `count` – Output the number of nodes.
  - `node [index]` – Show node ID and location at the given index.
  - `fastest [src] [dest]` – Compute fastest path between nodes.
  - `shortest [src] [dest]` – Compute shortest distance path between nodes.
  - `save` – Save last computed path to a file.
  - `print` – Print last computed path.

---

## Behavior and Rules

### Command Processing
- Commands are read line-by-line from the `CDataSource`.  
- Input lines are separated by whitespace.  
- Each command triggers interaction with the underlying `CTransportationPlanner` to compute paths or fetch data.  
- Results and errors are written to `OutSink` and `ErrSink`.

### Path Storage
- Stores the most recent path internally:
  - `PathSrcID`, `PathDestID` – source and destination node IDs.
  - `PathTime`, `PathDistance` – travel time or distance.
  - `Path` – vector of `TTripStep` objects describing each step.
  - `Shortest` / `Fastest` – flags indicating the type of last computed path.

### Special Values
- `NoPathExists` (`std::numeric_limits<double>::max()`) indicates no valid path found.

### File Output
- `"save"` command writes the last path to a CSV file in `<results>/`.  
- Filename format depends on path type:
  - Shortest: `<src>_<dest>_<distance>mi.csv`  
  - Fastest: `<src>_<dest>_<time>hr.csv`  
- Each row: `mode,node_id`

---

## Usage Example

```cpp
#include "TransportationPlannerCommandLine.h"
#include "StringDataSource.h"
#include "StringDataSink.h"

auto cmdsrc = std::make_shared<CStringDataSource>("commands.txt");
auto outsink = std::make_shared<CStringDataSink>();
auto errsink = std::make_shared<CStringDataSink>();
auto results = std::make_shared<CFileDataFactory>();
auto planner = std::make_shared<CDijkstraTransportationPlanner>(config);

CTransportationPlannerCommandLine cli(cmdsrc, outsink, errsink, results, planner);
cli.ProcessCommands();

// Access output
std::cout << "Output:\n" << outsink->GetData();
std::cout << "Errors:\n" << errsink->GetData();