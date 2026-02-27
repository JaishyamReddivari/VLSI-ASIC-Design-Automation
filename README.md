# VLSI / ASIC Design Automation Algorithms

A collection of core EDA (Electronic Design Automation) algorithms implemented in C++, covering design-space exploration, dependency analysis, netlist partitioning, and resource-constrained scheduling. These are foundational algorithms used in real VLSI/ASIC physical design and high-level synthesis toolchains.

## Algorithms

### 1. Pareto-Optimal Design Space Pruning

Identifies the **Pareto front** from a multi-dimensional design space by eliminating dominated points. A point is dominated if another point is equal or better on all metrics and strictly better on at least one.

**Application:** Trade-off analysis across competing design metrics — area vs. power vs. delay — during architectural exploration. Reduces the design space to only the non-dominated solutions worth investigating further.

**How it works:**

```
For each point P in the design space:
    For each other point Q:
        If Q ≤ P on ALL metrics AND Q < P on at least ONE → P is dominated
    If P is not dominated → P belongs to the Pareto front
```

Complexity is O(N² × M) where N = number of design points and M = number of metrics.

**Input format** (`points.txt`):

```
metric1,metric2,...,metricM
metric1,metric2,...,metricM
...
```

Each line is a comma-separated design point with M metrics (e.g., area, power, delay).

**Usage:**

```bash
g++ pareto.cpp -o pareto
./pareto points.txt <M> <N>
# M = number of metrics per point, N = number of points
```

**Output:** `out.txt` containing only the Pareto-optimal (non-dominated) design points.

---

### 2. Cycle Detection & Topological Sorting

Performs **Kahn's algorithm** (BFS-based topological sort) on a directed graph represented as an adjacency matrix. If the graph contains a cycle, it is detected and reported; otherwise, a valid topological ordering is produced.

**Application:** Dependency analysis in EDA flows — determining valid execution orders for operations in a datapath, validating that RTL dependency graphs are acyclic, and ordering synthesis/P&R steps.

**How it works:**

```
1. Compute in-degree for every vertex
2. Enqueue all vertices with in-degree 0
3. While queue is not empty:
     Dequeue vertex V → append to sorted order
     For each neighbor U of V:
       Decrement in-degree of U
       If in-degree of U == 0 → enqueue U
4. If sorted order contains fewer than N vertices → cycle exists
```

**Input format** (`input.txt`):

```
0 1 0 0
0 0 1 0
0 0 0 1
0 0 0 0
```

An N×N adjacency matrix where entry `[i][j] = 1` means there is a directed edge from vertex i to vertex j.

**Usage:**

```bash
g++ TPLSort.cpp -o topsort
./topsort <num_vertices> input.txt
```

**Output:** `out.txt` with vertices listed in topological order (1-indexed), or an error message if a cycle is detected.

---

### 3. FM (Fiduccia–Mattheyses) Min-Cut Partitioning

Implements the **FM heuristic** for 2-way min-cut partitioning of a hypergraph netlist. Iteratively moves cells between partitions to minimize the number of nets crossing the cut boundary.

**Application:** The core algorithm behind netlist partitioning in physical design — used for floorplanning, hierarchical placement, and managing design complexity by dividing large netlists into balanced, loosely-coupled sub-blocks.

**How it works:**

```
1. Initialize: assign cells to partitions alternately (balanced)
2. Compute gain for each cell:
     gain(c) = nets where c is the ONLY cell on its side (cutting_nets)
             - nets where all cells are on the same side as c (uncut_nets)
3. Repeat pass:
     a. Select highest-gain unlocked cell
     b. Move it to the opposite partition, lock it
     c. Incrementally update gains of affected neighbors
     d. Record cumulative cut size
4. Roll back to the state with the best observed cut size
5. Repeat passes until no improvement
```

The gain bucket data structure provides efficient O(1) access to the highest-gain cell.

**Input format** (`graph.rtf`):

```
net1: cell1, cell2, cell3
net2: cell2, cell4
...
```

Each line defines a net (hyperedge) followed by the cells it connects.

**Usage:**

```bash
g++ FM-partition.cpp -o fm_partition
./fm_partition <num_vertices> <num_nets> graph.rtf
```

**Output:** `out.txt` containing Partition A cells, Partition B cells, and the final cut size.

```
Partition A: 1 3 5
Partition B: 2 4 6
The cut size is 3.
```

---

### 4. Resource-Constrained List Scheduling

Schedules operations from a **Data Flow Graph (DFG)** onto heterogeneous hardware resources while respecting resource availability and operation latencies.

**Application:** High-level synthesis (HLS) — mapping algorithmic operations (add, multiply, etc.) to hardware functional units under area/resource constraints. Determines the cycle-by-cycle schedule that minimizes total latency given limited hardware.

**How it works:**

```
1. Build predecessor map from DFG edges
2. Initialize ready queue with operations having no unresolved dependencies
3. At each time step:
     a. Release resources from completed operations
     b. Resolve newly ready successors (all predecessors complete)
     c. For each ready operation:
        - Type 0 (no-op): schedule immediately, no resource consumed
        - Type N: if resource of type N is available → schedule, occupy for 'delay' cycles
        - Otherwise: defer to next time step
4. Advance time, repeat until all operations scheduled
```

Operations of type 0 are zero-latency pass-through nodes (e.g., graph source/sink). All other types consume a specific resource for a defined number of cycles.

**Input format:**

`DFG.txt`:
```
OP1,2: 3, 4
OP2,1: 5
OP3,0:
...
```

Format: `OP<id>,<resource_type>: <successor_ids>`. Resource type 0 = no resource needed.

`Resource.txt`:
```
R 1, 2, 3
R 2, 1, 5
```

Format: `R <type>, <count>, <delay>`. Specifies how many units of each type are available and their latency.

**Usage:**

```bash
g++ List.cpp -o scheduler
./scheduler DFG.txt Resource.txt
```

**Output** (stdout):

```
Operation Schedule:
OP 1: Start = 1, End = 4, Resource = 2
OP 2: Start = 1, End = 2, Resource = 1
OP 3: Start = 2, End = 2, Resource = None
...
```

## Project Structure

```
vlsi-eda-algorithms/
├── pareto.cpp          # Pareto-optimal design space pruning
├── TPLSort.cpp         # Cycle detection & topological sorting
├── FM-partition.cpp    # FM min-cut hypergraph partitioning
├── List.cpp            # Resource-constrained list scheduling
├── points.txt          # Sample design points for Pareto
├── input.txt           # Sample adjacency matrix for topological sort
├── graph.rtf           # Sample hypergraph netlist for FM
├── DFG.txt             # Sample data flow graph for scheduling
├── Resource.txt        # Sample resource specification
├── out.txt             # Generated output file
└── README.md
```

## Build & Run (Quick Reference)

| Algorithm | Compile | Run |
|---|---|---|
| Pareto Pruning | `g++ pareto.cpp -o pareto` | `./pareto points.txt <M> <N>` |
| Topological Sort | `g++ TPLSort.cpp -o topsort` | `./topsort <num_vertices> input.txt` |
| FM Partitioning | `g++ FM-partition.cpp -o fm_partition` | `./fm_partition <num_vertices> <num_nets> graph.rtf` |
| List Scheduling | `g++ List.cpp -o scheduler` | `./scheduler DFG.txt Resource.txt` |

All executables can be compiled with any C++11 (or later) compliant compiler.

## Algorithm Complexity

| Algorithm | Time Complexity | Space Complexity |
|---|---|---|
| Pareto Pruning | O(N² × M) | O(N × M) |
| Topological Sort | O(V² ) — adjacency matrix | O(V²) |
| FM Partitioning | O(P × N × E) — P passes, N cells, E nets | O(N + E) |
| List Scheduling | O(T × N) — T time steps, N operations | O(N + R) |

## Technologies

- **Language:** C++ (C++11)
- **Concepts:** Graph algorithms, hypergraph partitioning, multi-objective optimization, scheduling theory
- **Domain:** VLSI / ASIC design automation, EDA tool development, high-level synthesis

## Author

**Jaishyam Reddy Reddivari**
MS in Computer Engineering — ASIC / VLSI / Hardware Verification & Design Automation

## License

This project is provided as-is for educational and reference purposes.
