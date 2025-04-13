#include "topological_sort.hpp"
#include <mpi.h>

std::vector<int> topologicalSortMPI(const std::vector<std::vector<int>>& graph) {
    int numVertices = graph.size();
    std::vector<int> inDegree(numVertices, 0);
    std::vector<int> result;
    std::stack<int> zeroInDegree;

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Calculate in-degrees
    for (int u = rank; u < numVertices; u += size) {
        for (int v : graph[u]) {
            inDegree[v]++;
        }
    }

    // Gather in-degrees from all processes
    MPI_Allreduce(MPI_IN_PLACE, inDegree.data(), numVertices, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

    // Collect all vertices with zero in-degree
    if (rank == 0) {
        for (int i = 0; i < numVertices; ++i) {
            if (inDegree[i] == 0) {
                zeroInDegree.push(i);
            }
        }
    }

    // Broadcast zero in-degree vertices to all processes
    // Process vertices
    while (!zeroInDegree.empty()) {
        int u = zeroInDegree.top();
        zeroInDegree.pop();
        result.push_back(u);

        for (int v : graph[u]) {
            inDegree[v]--;
            if (inDegree[v] == 0) {
                zeroInDegree.push(v);
            }
        }
    }

    return result;
} 