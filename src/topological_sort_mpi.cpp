#include "topological_sort.hpp"
#include <mpi.h>
#include <vector>
#include <deque>
#include <algorithm>

std::vector<int> topologicalSortMPI(const std::vector<std::vector<int>>& graph) {
    const int numVertices = graph.size();
    std::vector<int> inDegree(numVertices, 0);
    std::vector<int> result;
    result.reserve(numVertices / 4);  // Estimate for local results

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Calculate chunk size for each process
    const int chunk_size = (numVertices + size - 1) / size;
    const int start_vertex = rank * chunk_size;
    const int end_vertex = std::min(start_vertex + chunk_size, numVertices);
    const int local_vertices = end_vertex - start_vertex;

    // Calculate local in-degrees efficiently
    std::vector<int> local_inDegree(numVertices, 0);
    for (int u = start_vertex; u < end_vertex; ++u) {
        for (int v : graph[u]) {
            local_inDegree[v]++;
        }
    }

    // Sum up all in-degrees using MPI_Allreduce
    MPI_Allreduce(local_inDegree.data(), inDegree.data(), numVertices, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

    // Local processing queue and updates buffer
    std::deque<int> local_queue;
    std::vector<int> degree_updates(numVertices, 0);
    std::vector<int> global_updates(numVertices);

    // Initialize local queue with zero in-degree vertices
    for (int i = start_vertex; i < end_vertex; ++i) {
        if (inDegree[i] == 0) {
            local_queue.push_back(i);
        }
    }

    // Process vertices in batches
    std::vector<bool> processed(numVertices, false);
    const int update_batch_size = 1024;
    std::vector<int> local_result;
    local_result.reserve(local_vertices);

    while (true) {
        int local_processed = 0;
        degree_updates.assign(numVertices, 0);

        // Process a batch of vertices
        while (!local_queue.empty() && local_processed < update_batch_size) {
            int u = local_queue.front();
            local_queue.pop_front();

            if (!processed[u]) {
                processed[u] = true;
                local_result.push_back(u);
                local_processed++;

                // Accumulate degree updates
                for (int v : graph[u]) {
                    degree_updates[v]--;
                }
            }
        }

        // Share updates less frequently
        MPI_Allreduce(degree_updates.data(), global_updates.data(), numVertices, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

        // Apply updates and find new zero in-degree vertices
        bool found_new = false;
        for (int v = start_vertex; v < end_vertex; ++v) {
            if (!processed[v]) {
                inDegree[v] += global_updates[v];
                if (inDegree[v] == 0) {
                    local_queue.push_back(v);
                    found_new = true;
                }
            }
        }

        // Check if all processes are done
        int local_done = (local_queue.empty() && !found_new) ? 1 : 0;
        int global_done;
        MPI_Allreduce(&local_done, &global_done, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
        
        if (global_done == size) break;
    }

    // Gather results efficiently
    int local_size = local_result.size();
    std::vector<int> sizes(size);
    MPI_Allgather(&local_size, 1, MPI_INT, sizes.data(), 1, MPI_INT, MPI_COMM_WORLD);

    std::vector<int> displacements(size);
    int total_size = 0;
    for (int i = 0; i < size; i++) {
        displacements[i] = total_size;
        total_size += sizes[i];
    }

    std::vector<int> global_result(total_size);
    MPI_Allgatherv(local_result.data(), local_size, MPI_INT,
                   global_result.data(), sizes.data(), displacements.data(),
                   MPI_INT, MPI_COMM_WORLD);

    return global_result;
} 