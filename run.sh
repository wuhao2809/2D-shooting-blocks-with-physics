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

cd ..

# Set DYLD_LIBRARY_PATH for SDL3 (Homebrew)
export DYLD_LIBRARY_PATH=/opt/homebrew/lib:$DYLD_LIBRARY_PATH

# Run the game from the root directory so gamedata.json is found
./build/TopDownShooter 