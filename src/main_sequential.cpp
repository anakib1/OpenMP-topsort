#include "topological_sort.hpp"
#include <iostream>
#include <vector>
#include <chrono>
#include <fstream>

std::vector<std::vector<int>> readGraphFromFile(const std::string& filename) {
    std::ifstream file(filename);
    std::vector<std::vector<int>> graph;
    int u, v;
    while (file >> u >> v) {
        if (u >= graph.size()) graph.resize(u + 1);
        if (v >= graph.size()) graph.resize(v + 1);
        graph[u].push_back(v);
    }
    return graph;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <graph_file>" << std::endl;
        return 1;
    }

    // Read graph from file
    std::vector<std::vector<int>> graph = readGraphFromFile(argv[1]);

    // Measure time for sequential implementation
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<int> resultSeq = topologicalSortSequential(graph);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> durationSeq = end - start;
    std::cout << "Sequential Topological Sort Duration: " << durationSeq.count() << " seconds\n";

    return 0;
} 