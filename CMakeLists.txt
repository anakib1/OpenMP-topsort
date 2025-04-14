cmake_minimum_required(VERSION 3.10)
project(OpenMPTopsort)

# Enable C++11
set(CMAKE_CXX_STANDARD 11)

# Find OpenMP
find_package(OpenMP REQUIRED)

# Find MPI
find_package(MPI REQUIRED)

# Include directories
include_directories(${MPI_INCLUDE_PATH})

# Add executables
add_executable(topsort_sequential src/main_sequential.cpp src/topological_sort_sequential.cpp)
add_executable(topsort_openmp src/main_openmp.cpp src/topological_sort_openmp.cpp)
add_executable(topsort_mpi src/main_mpi.cpp src/topological_sort_mpi.cpp)

# Link OpenMP
target_link_libraries(topsort_openmp OpenMP::OpenMP_CXX)

# Link MPI
target_link_libraries(topsort_mpi MPI::MPI_CXX) 