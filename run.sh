#!/bin/bash

# 1. Create build dir if missing
mkdir -p build
cd build

# 2. Compile (using all CPU cores for speed)
cmake ..
make -j$(nproc)

# 3. Step back to root and Run with MangoHud
if [ $? -eq 0 ]; then
    cd ..
    
    # Check if mangohud is installed
    if command -v mangohud >/dev/null 2>&1; then
        echo "Launching with Vertical MangoHud..."
        
        # We override the horizontal layout and set a vertical stack
        # horizontal=0: Forces vertical mode
        # legacy_layout=0: Uses the modern, cleaner look
        MANGOHUD_CONFIG="horizontal=0,legacy_layout=0,fps,frame_timing,cpu_stats,gpu_stats,ram,vram,position=top-left" \
        mangohud ./build/ApexRacingEngine
    else
        echo "MangoHud not found. Launching normally..."
        ./build/ApexRacingEngine
    fi
else
    echo "Build failed. Check the errors above."
fi