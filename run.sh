#!/bin/bash
# Usage: ./run.sh
# This script builds and runs the 2D Top-Down Shooting Game

set -e

# Create build directory if it doesn't exist
if [ ! -d "build" ]; then
    mkdir build
fi

cd build

# Configure project with CMake
cmake ..

# Build the project
make

# Run the game
./TopDownShooter 