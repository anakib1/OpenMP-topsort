#ifndef TOPOLOGICAL_SORT_HPP
#define TOPOLOGICAL_SORT_HPP


#pragma once
#include <vector>
#include <stack>

// Function to perform topological sort
std::vector<int> topologicalSort(const std::vector<std::vector<int>>& graph);
std::vector<int> topologicalSortOpenMP(const std::vector<std::vector<int>>& graph);
std::vector<int> topologicalSortMPI(const std::vector<std::vector<int>>& graph);
std::vector<int> topologicalSortSequential(const std::vector<std::vector<int>>& graph);

#endif // TOPOLOGICAL_SORT_HPP 