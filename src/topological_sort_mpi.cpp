#include "topological_sort.hpp"
#include <mpi.h>

std::vector<int> topologicalSortMPI(const std::vector<std::vector<int>>& graph) {
    int numVertices = graph.size();
    std::vector<int> inDegree(numVertices, 0);
    std::vector<int> result;
    std::vector<int> zeroInDegreeVec;  // Using vector instead of stack for easier MPI communication

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

    // Each process finds its portion of zero in-degree vertices
    for (int i = rank; i < numVertices; i += size) {
        if (inDegree[i] == 0) {
            zeroInDegreeVec.push_back(i);
        }
    }

    // Process vertices until no more zero in-degree vertices exist
    while (true) {
        // Share the number of zero in-degree vertices each process has
        int localZeroCount = zeroInDegreeVec.size();
        int totalZeroCount;
        MPI_Allreduce(&localZeroCount, &totalZeroCount, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

        if (totalZeroCount == 0) {
            break;  // No more vertices to process
        }

        // Process one vertex from each process that has zero in-degree vertices
        if (!zeroInDegreeVec.empty()) {
            int currentVertex = zeroInDegreeVec.back();
            zeroInDegreeVec.pop_back();
            result.push_back(currentVertex);

            // Create temporary array for degree updates
            std::vector<int> degreeUpdates(numVertices, 0);
            
            // Update in-degrees for neighbors
            for (int v : graph[currentVertex]) {
                degreeUpdates[v]--;
            }

            // Combine all degree updates
            MPI_Allreduce(MPI_IN_PLACE, degreeUpdates.data(), numVertices, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
            
            // Apply updates and check for new zero in-degree vertices
            for (int v = rank; v < numVertices; v += size) {
                inDegree[v] += degreeUpdates[v];
                if (inDegree[v] == 0 && degreeUpdates[v] != 0) {
                    zeroInDegreeVec.push_back(v);
                }
            }
        }

        // Synchronize to ensure all processes have consistent state
        MPI_Barrier(MPI_COMM_WORLD);
    }

    // Gather all partial results to all processes
    int localResultSize = result.size();
    std::vector<int> allSizes(size);
    MPI_Allgather(&localResultSize, 1, MPI_INT, allSizes.data(), 1, MPI_INT, MPI_COMM_WORLD);

    int totalSize = 0;
    for (int s : allSizes) {
        totalSize += s;
    }

    std::vector<int> globalResult(totalSize);
    std::vector<int> displacements(size);
    int currentDisp = 0;
    for (int i = 0; i < size; i++) {
        displacements[i] = currentDisp;
        currentDisp += allSizes[i];
    }

    MPI_Allgatherv(result.data(), localResultSize, MPI_INT,
                   globalResult.data(), allSizes.data(), displacements.data(),
                   MPI_INT, MPI_COMM_WORLD);

    return globalResult;
} 