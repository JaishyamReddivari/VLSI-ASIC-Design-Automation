## VLSI / ASIC Design Automation Algorithms (C++)
# Overview
- This project implements core VLSI/ASIC design automation algorithms in C++, focusing on design-space optimization, graph-based analysis, partitioning, and scheduling. The goal is to model and automate key steps used in EDA toolchains for physical and architectural design.

# Features Implemented
# 1. Pareto-Optimal Design Space Pruning
- Identifies Pareto-optimal solutions from a multi-dimensional design space
- Eliminates dominated design points to reduce exploration complexity
- Useful for trade-off analysis across metrics such as area, power, and delay
# 2. Graph Cycle Detection & Topological Sorting
- Detects cycles in directed graphs representing dependencies
- Performs topological ordering for acyclic graphs
- Forms the foundation for scheduling and dependency analysis in EDA flows
# 3. FM (Fiduccia–Mattheyses) Min-Cut Partitioning
- Implements a 2-way FM min-cut partitioner for netlist hypergraphs
- Achieves balanced partitioning while minimizing cut size
- Applicable to physical design and hierarchical partitioning problems
# 4. Resource-Constrained List Scheduling
- Schedules operations in a Data Flow Graph (DFG)
- Respects resource availability and latency constraints
- Maps operations to heterogeneous hardware units efficiently

# Input Files
- points.txt – Design points for Pareto optimization
- graph.rtf / input.txt – Graph definitions for cycle detection and topological sort
- DFG.txt – Data Flow Graph description
- Resource.txt – Available hardware resources
- out.txt – Generated outputs and results

# How to Build and Run
```
 g++ pareto.cpp -o pareto
```
 ```
 g++ TPLSort.cpp -o topsort
```
 ```
 g++ FM-partition.cpp -o fm_partition
```
 ```
 g++ List.cpp -o scheduler
```
# Run each executable with its corresponding input file:
```
./pareto points.txt
```
```
./topsort input.txt
```
```
./fm_partition graph.rtf
```
```
./scheduler DFG.txt Resource.txt
```

# Key Learning Outcomes
- Practical understanding of EDA algorithms
- Strong exposure to graph algorithms used in VLSI flows
- Experience modeling physical design and scheduling constraints
- Improved proficiency in C++ for systems and tool development

# Technologies Used
- C++
- Graph Algorithms
- Hypergraph Partitioning
- Scheduling Algorithms
- VLSI / ASIC Design Automation Concepts

# Author
- **Jaishyam Reddy Reddivari**
- **MS in Computer Engineering**
- **Focus: ASIC / VLSI / Hardware Verification & Design Automation**
