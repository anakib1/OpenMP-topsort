#include "topological_sort.hpp"
#include <iostream>
#include <vector>
#include <chrono>
#include <mpi.h>
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
    // Read graph from file
    std::vector<std::vector<int>> graph = readGraphFromFile("random_dag.txt");

    // Measure time for OpenMP implementation
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<int> resultOpenMP = topologicalSortOpenMP(graph);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> durationOpenMP = end - start;
    std::cout << "OpenMP Topological Sort Duration: " << durationOpenMP.count() << " seconds\n";

    // Initialize MPI
    MPI_Init(&argc, &argv);

    // Measure time for MPI implementation
    start = std::chrono::high_resolution_clock::now();
    std::vector<int> resultMPI = topologicalSortMPI(graph);
    end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> durationMPI = end - start;
    std::cout << "MPI Topological Sort Duration: " << durationMPI.count() << " seconds\n";

    // Finalize MPI
    MPI_Finalize();

    return 0;
} 