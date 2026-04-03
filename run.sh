#!/bin/bash

# Create build dir if missing
mkdir -p build
cd build

# Compile
cmake ..
make -j$(nproc)

# Run if build succeeded
if [ $? -eq 0 ]; then
    ./ApexRacingEngine
else
    echo "Build failed. Check the errors above."
fi