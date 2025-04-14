#include "topological_sort.hpp"
#include <queue>

std::vector<int> topologicalSortSequential(const std::vector<std::vector<int>>& graph) {
    int numVertices = graph.size();
    std::vector<int> inDegree(numVertices, 0);
    std::vector<int> result;
    std::queue<int> zeroInDegree;  // Using queue for sequential processing

    // Calculate in-degrees
    for (int u = 0; u < numVertices; ++u) {
        for (int v : graph[u]) {
            ++inDegree[v];
        }
    }

    // Find all vertices with in-degree 0
    for (int i = 0; i < numVertices; ++i) {
        if (inDegree[i] == 0) {
            zeroInDegree.push(i);
        }
    }

    // Process vertices in order
    while (!zeroInDegree.empty()) {
        int currentVertex = zeroInDegree.front();
        zeroInDegree.pop();
        result.push_back(currentVertex);

        // Update in-degrees of neighbors
        for (int v : graph[currentVertex]) {
            --inDegree[v];
            if (inDegree[v] == 0) {
                zeroInDegree.push(v);
            }
        }
    }

    return result;
} 