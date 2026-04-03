cmake_minimum_required(VERSION 3.10)
project(ApexRacingEngine C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 1. Find System Packages
find_package(OpenGL REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW REQUIRED glfw3)

# 2. Setup Directories
# We added 'deps/cglm/include' so we can use #include <cglm/cglm.h>
include_directories(
    src 
    deps 
    deps/cglm/include 
    ${GLFW_INCLUDE_DIRS}
)

# 3. Collect Source Files
# This automatically picks up src/main.c and src/renderer/glad.c
file(GLOB_RECURSE SOURCES "src/*.c")

# 4. Create the executable
add_executable(ApexRacingEngine ${SOURCES})

# 5. Link Libraries
# 'm' is the math library—even though cglm is header-only, 
# some functions might still need the linker's help.
target_link_libraries(ApexRacingEngine 
    OpenGL::GL 
    ${GLFW_LIBRARIES} 
    m
)