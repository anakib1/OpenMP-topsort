
# Topological Sort with OpenMP and MPI

This project implements the topological sort algorithm using two parallel computing paradigms: OpenMP and MPI. The goal is to compare the performance of these implementations on graphs of varying sizes.

## Topological Sort Algorithm

Topological sorting of a Directed Acyclic Graph (DAG) is a linear ordering of its vertices such that for every directed edge \( u \to v \), vertex \( u \) comes before \( v \) in the ordering. Topological sorting is not possible if the graph is not a DAG.

### Steps of the Algorithm:
1. **Calculate In-Degrees**: Compute the in-degree (number of incoming edges) for each vertex.
2. **Initialize Zero In-Degree Stack**: Collect all vertices with zero in-degree into a stack.
3. **Process Vertices**: Repeatedly remove a vertex from the stack, add it to the topological order, and decrease the in-degree of its neighbors. If a neighbor's in-degree becomes zero, add it to the stack.
4. **Check for Cycles**: If the topological order contains all vertices, the graph is a DAG. Otherwise, it contains a cycle.

## OpenMP Implementation

OpenMP is a parallel programming model for shared memory architectures. It uses compiler directives to parallelize loops and sections of code.

### Key Features:
- **Parallel In-Degree Calculation**: The in-degree calculation is parallelized using `#pragma omp parallel for`, allowing multiple threads to update the in-degree array concurrently.
- **Critical Section for Stack Operations**: The stack operations are protected by `#pragma omp critical` to ensure thread safety when modifying the stack.

## MPI Implementation

MPI (Message Passing Interface) is a standard for distributed memory parallel computing. It allows processes to communicate with each other by sending and receiving messages.

### Key Features:
- **Distributed In-Degree Calculation**: Each process calculates the in-degrees for a subset of vertices. The results are then combined using `MPI_Allreduce`.
- **Process Synchronization**: The processes work together to maintain the zero in-degree stack and ensure that the topological order is constructed correctly.

## Running the Project

1. **Generate Graphs**: Use the `generate_graph.py` script to create random DAGs of different sizes.
   ```bash
   python generate_graph.py
   ```

2. **Build the Project**: Use the `build.sh` script to compile the OpenMP and MPI implementations.
   ```bash
   ./build.sh
   ```

3. **Run Benchmarks**: Execute the `run_benchmark.py` script to run the benchmarks and save the results.
   ```bash
   python run_benchmark.py
   ```

The results, including execution times and histograms, will be saved in the project directory. 