#include "topological_sort.hpp"
#include <omp.h>
#include <vector>
#include <deque>
#include <algorithm>

std::vector<int> topologicalSortOpenMP(const std::vector<std::vector<int>>& graph) {
    const int numVertices = graph.size();
    std::vector<int> inDegree(numVertices, 0);
    std::vector<int> result;
    result.reserve(numVertices);

    // Calculate in-degrees using parallel reduction
    #pragma omp parallel
    {
        const int chunk_size = 1024;
        std::vector<int> local_inDegree(numVertices, 0);

        #pragma omp for schedule(dynamic, chunk_size) nowait
        for (int u = 0; u < numVertices; ++u) {
            for (int v : graph[u]) {
                local_inDegree[v]++;
            }
        }

        // Reduce local counts to global inDegree
        #pragma omp critical
        {
            for (int i = 0; i < numVertices; ++i) {
                inDegree[i] += local_inDegree[i];
            }
        }
    }

    // Thread-local storage for better cache utilization
    const int num_threads = omp_get_max_threads();
    std::vector<std::deque<int>> thread_queues(num_threads);
    std::vector<std::vector<int>> thread_results(num_threads);
    std::vector<std::vector<int>> thread_updates(num_threads);

    // Pre-allocate memory for thread-local storage
    #pragma omp parallel
    {
        int thread_id = omp_get_thread_num();
        thread_results[thread_id].reserve(numVertices / num_threads);
        thread_updates[thread_id].reserve(1024);
    }

    // Initialize zero in-degree vertices in parallel
    const int chunk_size = 1024;
    #pragma omp parallel
    {
        int thread_id = omp_get_thread_num();
        #pragma omp for schedule(dynamic, chunk_size)
        for (int i = 0; i < numVertices; ++i) {
            if (inDegree[i] == 0) {
                thread_queues[thread_id].push_back(i);
            }
        }
    }

    // Process vertices in parallel
    std::vector<bool> processed(numVertices, false);
    bool processing = true;

    while (processing) {
        processing = false;

        #pragma omp parallel
        {
            int thread_id = omp_get_thread_num();
            auto& local_queue = thread_queues[thread_id];
            auto& local_result = thread_results[thread_id];
            auto& local_updates = thread_updates[thread_id];
            local_updates.clear();

            while (!local_queue.empty()) {
                int u = local_queue.front();
                local_queue.pop_front();

                bool should_process = false;
                #pragma omp critical
                {
                    if (!processed[u]) {
                        processed[u] = true;
                        should_process = true;
                    }
                }

                if (should_process) {
                    processing = true;
                    local_result.push_back(u);

                    // Collect vertices that need degree updates
                    for (int v : graph[u]) {
                        local_updates.push_back(v);
                    }
                }
            }

            // Apply updates in batches
            if (!local_updates.empty()) {
                #pragma omp critical
                {
                    for (int v : local_updates) {
                        inDegree[v]--;
                        if (inDegree[v] == 0) {
                            local_queue.push_back(v);
                        }
                    }
                }
            }
        }

        // Work stealing
        if (processing) {
            #pragma omp parallel
            {
                int thread_id = omp_get_thread_num();
                auto& local_queue = thread_queues[thread_id];

                if (local_queue.empty()) {
                    for (int i = 1; i < num_threads && local_queue.empty(); ++i) {
                        int src_thread = (thread_id + i) % num_threads;
                        auto& src_queue = thread_queues[src_thread];

                        #pragma omp critical
                        {
                            if (!src_queue.empty()) {
                                // Steal half of the work
                                size_t steal_size = src_queue.size() / 2;
                                for (size_t j = 0; j < steal_size; ++j) {
                                    local_queue.push_back(src_queue.front());
                                    src_queue.pop_front();
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // Combine results from all threads
    result.clear();
    for (const auto& local_result : thread_results) {
        result.insert(result.end(), local_result.begin(), local_result.end());
    }

    return result;
} 