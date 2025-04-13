#include "topological_sort.hpp"
#include <omp.h>

std::vector<int> topologicalSortOpenMP(const std::vector<std::vector<int>>& graph) {
    int numVertices = graph.size();
    std::vector<int> inDegree(numVertices, 0);
    std::vector<int> result;
    std::stack<int> zeroInDegree;

    // Calculate in-degrees
    #pragma omp parallel for
    for (int u = 0; u < numVertices; ++u) {
        for (int v : graph[u]) {
            #pragma omp atomic
            ++inDegree[v];
        }
    }

    // Collect all vertices with zero in-degree
    for (int i = 0; i < numVertices; ++i) {
        if (inDegree[i] == 0) {
            zeroInDegree.push(i);
        }
    }

    // Process vertices
    while (!zeroInDegree.empty()) {
        int u = zeroInDegree.top();
        zeroInDegree.pop();
        result.push_back(u);

        #pragma omp parallel for
        for (int v : graph[u]) {
            #pragma omp atomic
            --inDegree[v];
            if (inDegree[v] == 0) {
                #pragma omp critical
                zeroInDegree.push(v);
            }
        }
    }

    return result;
} 