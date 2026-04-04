cmake_minimum_required(VERSION 3.10)
# Changed project to include CXX (C++) support
project(ApexRacingEngine C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11) # ImGui needs C++11
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 1. Find System Packages
find_package(OpenGL REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW REQUIRED glfw3)

# 2. Setup Directories
include_directories(
    src 
    deps 
    deps/cglm/include 
    src/vendor/imgui
    src/vendor/imgui/backends
    ${GLFW_INCLUDE_DIRS}
)

# 3. Collect Source Files
# We need to collect both .c (your code) and .cpp (ImGui code)
file(GLOB_RECURSE SOURCES "src/*.c")
file(GLOB_RECURSE IMGUI_SOURCES "src/vendor/imgui/*.cpp")
file(GLOB_RECURSE BRIDGE_SOURCES "src/*.cpp")

# 4. Create the executable
add_executable(ApexRacingEngine ${SOURCES} ${IMGUI_SOURCES} ${BRIDGE_SOURCES})

# 5. Link Libraries
target_link_libraries(ApexRacingEngine 
    OpenGL::GL 
    ${GLFW_LIBRARIES} 
    m
    dl # ImGui/GLAD often needs the dynamic linker library on Linux
)