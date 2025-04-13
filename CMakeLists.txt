cmake_minimum_required(VERSION 3.10)
project(OpenMPTopsort)

# Enable C++11
set(CMAKE_CXX_STANDARD 11)

# Manually set OpenMP flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Xpreprocessor -fopenmp -I/opt/homebrew/opt/libomp/include")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L/opt/homebrew/opt/libomp/lib -lomp")

# Find OpenMP
# find_package(OpenMP REQUIRED)

# Find MPI
find_package(MPI REQUIRED)

# Include directories
include_directories(${MPI_INCLUDE_PATH})
include_directories(/opt/homebrew/opt/open-mpi/include)

# Add executables
add_executable(topsort_openmp src/main_openmp.cpp src/topological_sort_openmp.cpp)
add_executable(topsort_mpi src/main_mpi.cpp src/topological_sort_mpi.cpp)

target_link_libraries(topsort_openmp ${MPI_LIBRARIES})
target_link_libraries(topsort_mpi ${MPI_LIBRARIES}) 