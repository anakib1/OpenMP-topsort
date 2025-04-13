#!/bin/bash

# Navigate to the build directory
cd build

# Run the topsort executable
mpirun -np 4 ./topsort 