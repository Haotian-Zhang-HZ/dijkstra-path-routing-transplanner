# Dijkstra Transportation Planner

**Course:** ECS34 WQ26, UC Davis  
**Project Date:** January – March 2026  

---

## Project Overview

For this project, I developed a **transportation planner** capable of computing the shortest and fastest routes in a city map using OpenStreetMap (OSM) data. The system supports multiple modes of transportation, including walking, biking, and buses. The goal was to simulate realistic urban navigation and optimize route planning for different transportation methods.

---

## Technical Highlights

- Implemented **Dijkstra’s Algorithm** in C++ to compute shortest paths efficiently.
- Integrated **third-party C libraries** (Expat XML) for parsing OSM XML data.
- Built a **modular system** with classes for bus routing, path finding, and command-line interaction.
- Developed and ran **automated tests** using GoogleTest to ensure correctness.
- Managed the project with **git**, maintaining full version history and collaboration with a teammate.
- Generated human-readable outputs and visualizations, including KML files compatible with Google Maps.

---

## My Contributions

- Designed and implemented the **CDijkstraPathRouter** class for efficient shortest-path computation.
- Developed the **CBusSystemIndexer** to organize bus routes and stops for quick lookup.
- Built the **CDijkstraTransportationPlanner** to integrate street maps, bus routes, and multiple transportation modes.
- Wrote **unit tests** to validate algorithms and data parsing.
- Collaborated closely with a partner, integrating code and ensuring consistent coding conventions and documentation.

---

## Project Outcomes

- Successfully computed shortest and fastest paths across realistic city maps.
- Supported multiple transportation modes with accurate constraints (e.g., one-way streets, bike restrictions).
- Produced visual outputs via KML for route visualization in Google Maps.
- Learned to integrate C++ with third-party C libraries and handle complex XML data.
- Gained experience in algorithm optimization, software testing, and modular system design.

---

## Skills Demonstrated

- C++ programming and object-oriented design
- Algorithm implementation (graph theory, Dijkstra’s Algorithm)
- Software testing and debugging
- XML and data file parsing
- Version control with git
- Technical documentation