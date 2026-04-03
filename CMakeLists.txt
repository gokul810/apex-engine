cmake_minimum_required(VERSION 3.10)
project(ApexRacingEngine C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 1. Find System Packages (Only OpenGL and GLFW now)
find_package(OpenGL REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW REQUIRED glfw3)

# 2. Setup Directories
# We add 'deps' to the include path so we can do #include <glad/glad.h>
include_directories(src deps ${GLFW_INCLUDE_DIRS})

# 3. Collect Source Files
# This will find main.c AND your new glad.c automatically
file(GLOB_RECURSE SOURCES "src/*.c")

# 4. Create the executable
add_executable(ApexRacingEngine ${SOURCES})

# 5. Link Libraries
# We removed ${GLEW_LIBRARIES} since GLAD is compiled directly into your app
target_link_libraries(ApexRacingEngine 
    OpenGL::GL 
    ${GLFW_LIBRARIES} 
    m
)