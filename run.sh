#!/bin/bash

# 1. Create build dir if missing
mkdir -p build
cd build

# 2. Compile (only run cmake if needed, but safe to keep)
cmake ..
make -j$(nproc)

# 3. Step back to root and Run
if [ $? -eq 0 ]; then
    cd ..
    ./build/ApexRacingEngine
else
    echo "Build failed. Check the errors above."
fi