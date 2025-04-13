#!/bin/bash

# Create build directory
mkdir -p build
cd build

# Run CMake and make
cmake ..
make 