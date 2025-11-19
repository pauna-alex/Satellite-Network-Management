# Satellite Network Management

## Overview
This project implements a software solution for the Romanian Space Agency to manage a network of satellites. The network is structured as a **binary tree**, where the hierarchy is determined by the reporting frequency of each satellite.

The core logic involves prioritizing satellites with higher reporting frequencies (placing them closer to the root) and using **Min-Heaps** to construct the optimal structure. The project also handles message decoding/encoding, error recovery, and network extensions using multi-way trees.

## Implementation Details

### Auxiliary Functions
In addition to the main logic, auxiliary functions adapted from Data Structures and Algorithms (SDA) laboratories were utilized. These generic functions were modified to fit the specific structures used in this project (`sateliti_1`, `satelit`, `heap`).

### Main Functions Description

#### `heap_insert(heap *heap_sateliti, satelit *add)`
- **Purpose**: Inserts a new satellite into the heap while maintaining the min-heap property.
- **Parameters**:
  - `heap_sateliti`: The heap structure storing the satellites.
  - `add`: The satellite to be added.
- **Functionality**:
  - Checks if the heap requires resizing and doubles capacity if necessary.
  - Appends the new satellite to the end of the heap.
  - Performs the "sift up" operation to restore heap properties.
  - Compares frequencies to determine correct positioning.
  - Uses bitwise shift operations (`GO_UP`) for efficient index calculation.

#### `pop_min_lexicografic(heap *heap_sateliti)`
- **Purpose**: Extracts the satellite with the minimum frequency, applying lexicographical rules for tie-breaking.
- **Parameters**:
  - `heap_sateliti`: The source heap.
- **Functionality**:
  - Identifies the minimum frequency among all satellites.
  - Among satellites with the minimum frequency, selects the one with the lexicographically smallest name.
  - Removes the selected satellite and replaces it with the last element in the heap.
  - Returns the extracted satellite.

#### `build_tree(FILE *in)`
- **Purpose**: Constructs the binary tree network as required by Task 1.
- **Parameters**:
  - `in`: Input file containing satellite data.
- **Functionality**:
  - Reads the number of satellites and their data (frequency and name).
  - Initializes a heap with the read satellites.
  - Iteratively extracts the two satellites with the lowest frequencies.
  - Creates a parent node where:
    - **Frequency** = Sum of the two children's frequencies.
    - **Name** = Concatenation of the Left Child Name + Right Child Name.
  - Continues the process until only one node (the root) remains in the heap.
  - Returns the root of the constructed tree.

#### `find_node(struct satelit* root, char* name, char* codificare, int lvl)`
- **Purpose**: Searches for a satellite by name and records its binary encoding path.
- **Parameters**:
  - `root`: Tree root.
  - `name`: Target satellite name.
  - `codificare`: Buffer to store the binary path.
  - `lvl`: Current depth level.
- **Functionality**:
  - Recursively traverses the tree seeking the specified name.
  - Appends '0' for left branches and '1' for right branches.
  - Returns `1` if found, `0` otherwise.
  - Null-terminates the encoding string upon finding the node.

#### `all_names_found(struct satelit* root, char lista_sateliti[MAX][MAX], int nr)`
- **Purpose**: Verifies if a specific node acts as a parent to a list of given satellites.
- **Parameters**:
  - `root`: The node to check.
  - `lista_sateliti`: List of satellite names to search for.
  - `nr`: Number of satellites in the list.
- **Functionality**:
  - Since parent nodes contain the concatenated names of their children, this function checks if all names in the list are substrings of the current node's name.
  - Returns `1` only if all names are present.
  - Uses `strstr` for substring verification.

#### `convert_to_graph(struct satelit* root)`
- **Purpose**: Converts the binary tree into a graph structure for Task 5.
- **Parameters**:
  - `root`: Binary tree root.
- **Functionality**:
  - Creates a graph node for every tree node.
  - Transforms binary links (left/right) into a generic list of neighbors.
  - Recursively applies the transformation to all children.
  - Returns the root of the resulting graph.

#### `merge(FILE *in)`
- **Purpose**: Constructs the multi-way tree extension structure from input data.
- **Parameters**:
  - `in`: Input file containing multi-way tree data.
- **Functionality**:
  - Reads the root and its data.
  - Reads the number of parents, and for each parent:
    - Reads the parent name and number of children.
    - Locates the parent node in the graph.
    - Reads and adds every child to the parent's adjacency list.
  - Returns the root of the structure.

#### `get_path(sateliti_1 *root, char *name, sateliti_1 **path, int lvl)`
- **Purpose**: Finds the path from the root to a specific node in the graph.
- **Parameters**:
  - `root`: Graph root.
  - `name`: Target node name.
  - `path`: Array to store the nodes along the path.
  - `lvl`: Current level.
- **Functionality**:
  - Records the current node in the path.
  - Checks if the current node matches the target.
  - Recursively explores all neighbors.
  - Returns the level of the found node or `-1` if not found.

#### `distance_between_nodes(sateliti_1 *root, char *name1, char *name2)`
- **Purpose**: Calculates the distance between two satellites in the hybrid graph.
- **Parameters**:
  - `root`: Graph root.
  - `name1`: First satellite name.
  - `name2`: Second satellite name.
- **Functionality**:
  - Finds the paths from the root to both satellites.
  - Identifies the lowest common ancestor (the point where paths diverge).
  - Calculates distance based on path lengths relative to the intersection.
  - Adds 2 to include the actual nodes in the count.

#### `print_level(int n, struct satelit *root, FILE *out)`
- **Purpose**: Prints all nodes at a specific depth level.
- **Parameters**:
  - `n`: Target level.
  - `root`: Tree root.
  - `out`: Output file.
- **Functionality**:
  - Uses a queue for Breadth-First Search (BFS) traversal.
  - Counts levels during traversal.
  - Prints satellite info (frequency and name) when the target level is reached.

#### `find_lvl(struct satelit *root)`
- **Purpose**: Determines the height of the tree.
- **Parameters**:
  - `root`: Tree root.
- **Functionality**:
  - Recursively calculates left and right subtree heights.
  - Returns `max(left, right) + 1`.

## Task-Specific Implementations

### Task 1
- Constructs the binary tree using the Min-Heap logic.
- Determines the tree height and prints the tree level by level.

### Task 2
- Decodes binary messages (`0` = left, `1` = right) using the constructed tree.
- Traverses the tree according to the binary stream and prints the names of the satellites found at the leaves.



[Image of Binary Tree traversal]


### Task 3
- Encodes a list of satellite names into a binary string.
- Finds the path from the root to each specified satellite and concatenates the resulting binary codes.

### Task 4
- Identifies the **closest common parent** (Lowest Common Ancestor) for a set of satellites reporting errors.
- Validates the parent by ensuring its name contains the names of all target satellites (using the concatenation property).

### Task 5
- Transforms the main binary tree into a graph.
- Attaches multi-way trees to specific nodes in the main structure.
- Calculates the distance between two specified nodes within this hybrid structure.

## Memory Management
Dynamic memory is managed explicitly:
- Heap structures are allocated and freed appropriately.
- Tree nodes are recursively deallocated via `free_tree`.
- Graph structures have specialized deallocation functions via `free_graph`.
- Allocation errors are checked using a `DIE` macro.
- *Note:* There is a known issue in Task 4 (`00-task4`) regarding memory leaks where the exact cause of the loss was not identified.
