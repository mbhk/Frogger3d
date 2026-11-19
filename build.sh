#!/bin/bash

# Build script for 3D Frogger game

echo "Building 3D Frogger game..."

# Create build directory
mkdir -p build
cd build

# Run CMake to configure the build
echo "Configuring build..."
cmake ..

# Build the project
echo "Building project..."
make

# Check if build was successful
if [ $? -eq 0 ]; then
    echo "Build successful!"
    echo "To run the game, execute: ./Frogger3D"
else
    echo "Build failed!"
    exit 1
fi